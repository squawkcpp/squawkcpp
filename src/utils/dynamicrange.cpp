#include "dynamicrange.h"

#include <assert.h>
#include <iostream>

namespace media {

#define min(a, b) ((a) < (b) ? (a) : (b))

DynamicRange::DynamicRange() {
    av_register_all();
}

int DynamicRange::calculate ( const std::string& filename ) {
    struct stream_context sc;
    struct dr_meter meter;
    int err;

    meter_init(&meter);

    err = sc_open( &sc, filename.c_str() );
    if (err < 0) { return print_av_error("sc_open", err); }

    int stream_index = err = av_find_best_stream(
        sc.format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
//    if (err < 0) { print_av_error("av_find_best_stream", err); goto cleanup; }

    err = sc_start_stream(&sc, stream_index);
//    if (err < 0) { print_av_error("sc_start_stream", err); goto cleanup; }

    // Print out the stream info
    AVCodecContext *codec_ctx = sc_get_codec(&sc);
    char codecinfobuf[256];
    avcodec_string(codecinfobuf, sizeof(codecinfobuf), codec_ctx, 0);
    fprintf(stderr, "%.256s\n", codecinfobuf);


    err = meter_start(&meter, codec_ctx->channels, codec_ctx->sample_rate, codec_ctx->sample_fmt);
//    if (err) { goto cleanup; }

    fprintf(stderr, "Collecting fragments information...\n");

    size_t fragment = 0;
    int throbbler_stage = 0;
    while (!sc_eof(&sc)) {
        err = sc_get_next_frame(&sc);
        if (err < 0) {
            print_av_error("sc_get_next_frame", err);
  //          goto cleanup;
        }

        err = meter_feed(&meter, sc.buf, sc.buf_size);
//        if (err) { goto cleanup; }

        if (fragment < meter.fragment) {
            fragment = meter.fragment;
            if ((throbbler_stage % 4) == 0) {
                fprintf(stderr, "\033[1K\033[1G %c  %2i:%02i ",
                        throbbler[throbbler_stage / 4],
                        (fragment * 3) / 60,
                        (fragment * 3) % 60);
            }
            throbbler_stage += 1;
            throbbler_stage %= 16;
        }
    }

//    int _final_range = 0;
//    meter_finish(&meter, &_final_range );

    meter_finish( &meter );
cleanup:
    meter_free(&meter);
    sc_close(&sc);

    if (err < 0) {
        return err;
    }

    return 0;
}

/******************************************************************************/

void DynamicRange::sc_init(struct stream_context *self) {
    self->format_ctx = NULL;
    self->stream_index = 0;
    self->buf = NULL;
    self->buf_size = 0;
    self->buf_alloc_size = 0;
    self->state = stream_context::STATE_INITIALIZED;
}

int DynamicRange::sc_open(struct stream_context *self, const char *filename) {
    int err;

    sc_init(self);
    err = avformat_open_input(&self->format_ctx, filename, NULL, NULL);
    if (err < 0) { return err; }
    err = avformat_find_stream_info(self->format_ctx, NULL);
    if (err < 0) { return err; }

    self->state = stream_context::STATE_OPEN;

    return 0;
}

/* return the AVCodecContext for the active stream */
AVCodecContext * DynamicRange::sc_get_codec(struct stream_context *self) {
    return self->format_ctx->streams[self->stream_index]->codec;
}

void DynamicRange::sc_close(struct stream_context *self) {
    if ( stream_context::STATE_OPEN <= self->state && self->state != stream_context::STATE_CLOSED) {
        avcodec_close(sc_get_codec(self));
        avformat_close_input(&
        self->format_ctx);
        self->state = stream_context::STATE_CLOSED;
    }
}

bool DynamicRange::sc_eof(struct stream_context *self) {
    return self->state == stream_context::STATE_CLOSED;
}

int DynamicRange::sc_start_stream(struct stream_context *self, int stream_index) {
    self->stream_index = stream_index;
    AVCodecContext *ctx = sc_get_codec(self);
    AVCodec *codec = avcodec_find_decoder(ctx->codec_id);
    if (codec == NULL) {
        return AVERROR_DECODER_NOT_FOUND;
    }

    // Allocate a buffer.
    size_t buf_size = 192000; //changed
    if (self->buf_alloc_size < buf_size) {
        av_free(self->buf);
        // NOTE: the buffer MUST be allocated with av_malloc;
        // ffmpeg has some very strict alignment requirements.
        self->buf = av_malloc(buf_size);
        self->buf_size = 0;
        self->buf_alloc_size = buf_size;
    }
    if (self->buf == NULL) {
        return AVERROR(ENOMEM);
    }

    /* XXX check codec */
    return avcodec_open2(ctx, codec,NULL);
}

// Decode one frame of audio
int DynamicRange::sc_get_next_frame(struct stream_context *self) {
    int err;

    if (self->state == stream_context::STATE_CLOSED) {
        return AVERROR_EOF;
    }

    // Grab a new packet, if necessary
    while (self->state == stream_context::STATE_OPEN) {
        err = av_read_frame(self->format_ctx, &self->real_pkt);
        if (err == AVERROR_EOF || url_feof(self->format_ctx->pb)) {
            av_init_packet(&self->pkt);
            self->pkt.data = NULL;
            self->pkt.size = 0;
            self->state = stream_context::STATE_NEED_FLUSH;
        } else if (err < 0) {
            return err;
        } else if (self->real_pkt.stream_index == self->stream_index) {
            self->pkt = self->real_pkt;
            self->state = stream_context::STATE_VALID_PACKET;
        } else {
            // we don't care about this frame; try another
            av_free_packet(&self->real_pkt);
        }
    }

    AVCodecContext *codec_ctx = sc_get_codec(self);

    // Decode the audio.

    // The third parameter gives the size of the output buffer, and is set
    // to the number of bytes used of the output buffer.
    // The return value is the number of bytes read from the packet.
    // The codec is not required to read the entire packet, so we may need
    // to keep it around for a while.
    int buf_size = self->buf_alloc_size;
    err = avcodec_decode_audio3(codec_ctx, static_cast< int16_t* >( self->buf ), &buf_size, &self->pkt);
    if (err < 0) { return err; }
    int bytes_used = err;

    self->buf_size = buf_size;

    if (self->state == stream_context::STATE_VALID_PACKET) {
        if (0 < bytes_used && bytes_used < self->pkt.size) {
            self->pkt.data += bytes_used;
            self->pkt.size -= bytes_used;
        } else  {
            self->state = stream_context::STATE_OPEN;
            av_free_packet(&self->real_pkt);
        }
    } else if (self->state == stream_context::STATE_NEED_FLUSH) {
        avcodec_close(codec_ctx);
        self->state = stream_context::STATE_CLOSED;
    }

    return 0;
}

/******************************************************************************/

int DynamicRange::compare_samples(const void *s1, const void *s2) {
    sample rms1 = *(sample *)s1;
    sample rms2 = *(sample *)s2;
    if (rms1 > rms2) return -1;
    else if (rms1 < rms2) return 1;
    return 0;
}

DynamicRange::sample DynamicRange::get_sample(void *buf, size_t i, enum AVSampleFormat sample_fmt) {
    switch(sample_fmt) {
    case AV_SAMPLE_FMT_U8: return (sample)(((uint8_t *)buf)[i] - 0x80) * FACTOR8;
    case AV_SAMPLE_FMT_S16: return (sample)(((int16_t *)buf)[i]) * FACTOR16;
    case AV_SAMPLE_FMT_S32: return (sample)(((int32_t *)buf)[i]) * FACTOR32;
    case AV_SAMPLE_FMT_FLT: return (sample)(((float *)buf)[i]);
    case AV_SAMPLE_FMT_DBL: return (sample)(((double *)buf)[i]);
    default: return 0.0;
    }
}

DynamicRange::sample DynamicRange::to_db(const sample linear) {
    return 20.0 * log10(linear);
}

void DynamicRange::meter_init(struct dr_meter *self) {
    for (int ch = 0; ch < MAX_CHANNELS; ch++) {
        self->rms_values[ch] = NULL;
        self->peak_values[ch] = NULL;
        self->sum[ch] = 0;
        self->peak[ch] = 0;
    }
    self->channels = 0;
    self->sample_rate = 0;
    self->sample_size = 0;
    self->sample_fmt = 0;
    self->fragment = 0;
    self->fragment_size = 0;
    self->fragment_read = 0;
    self->fragment_started = 0;
}

int DynamicRange::meter_start(struct dr_meter *self, int channels, int sample_rate, int sample_fmt) {
    if (channels > MAX_CHANNELS) {
        fprintf(stderr, "FATAL ERROR: Too many channels! Max channels %is.\n", MAX_CHANNELS);
        return 240; // ???
    }

    if (sample_fmt != AV_SAMPLE_FMT_U8 &&
        sample_fmt != AV_SAMPLE_FMT_S16 &&
        sample_fmt != AV_SAMPLE_FMT_S32 &&
        sample_fmt != AV_SAMPLE_FMT_FLT &&
        sample_fmt != AV_SAMPLE_FMT_DBL) {
        std::cerr << "FATAL ERROR: Unsupported sample format: %s\n" << av_get_sample_fmt_name( static_cast< AVSampleFormat >( sample_fmt ) ) << std::endl;
        return 240;
    }

    // Allocate RMS and peak storage
    for (int ch = 0; ch < channels; ch++) {
        self->rms_values[ch] =
            static_cast< sample * >( malloc(MAX_FRAGMENTS * sizeof(*self->rms_values[ch])) );
        self->peak_values[ch] =
            static_cast< sample * >( malloc(MAX_FRAGMENTS * sizeof(*self->peak_values[ch])));
        if (self->rms_values[ch] == NULL ||
            self->peak_values[ch] == NULL) {
            return AVERROR(ENOMEM);
        }
    }
    self->channels = channels;
    self->sample_rate = sample_rate;
    self->sample_fmt = sample_fmt;
    self->sample_size = av_get_bytes_per_sample( static_cast< AVSampleFormat >( sample_fmt ) );
    self->fragment_size = ((long)sample_rate * FRAGMENT_LENGTH / 1000);
    assert(self->fragment_size > 0);
    return 0;
}

int DynamicRange::meter_fragment_start(struct dr_meter *self) {
    if (self->fragment >= MAX_FRAGMENTS) {
        fprintf(stderr, "FATAL ERROR: Input too long! Max length %is.\n", MAX_FRAGMENTS*3);
        return 240;
    }

    for (int ch = 0; ch < self->channels; ch++) {
        self->sum[ch] = 0;
        self->peak[ch] = 0;
    }

    self->fragment_read = 0;
    self->fragment_started = true;

    return 0;
}

void DynamicRange::meter_fragment_finish(struct dr_meter *self) {
    for (int ch = 0; ch < self->channels; ch++) {
        self->rms_values[ch][self->fragment] =
            sqrt(2.0 * self->sum[ch] / self->fragment_read);
        self->peak_values[ch][self->fragment] = self->peak[ch];
    }
    self->fragment++;
    self->fragment_started = false;
}

void DynamicRange::meter_scan_internal(struct dr_meter *self, void *buf, size_t samples, enum AVSampleFormat sample_fmt) {
    for (size_t i = 0; i < samples; i++) {
        for (int ch = 0; ch < self->channels; ch++) {
            sample value = get_sample(buf, i * self->channels + ch, sample_fmt);
            self->sum[ch] += value * value;

            value = fabs(value);
            if (self->peak[ch] < value) {
                self->peak[ch] = value;
            }
        }
    }
}

/* Feed the meter. Scan a single frame of audio. */
int DynamicRange::meter_feed(struct dr_meter *self, void *buf, size_t buf_size) {
    size_t samples = buf_size / (self->sample_size * self->channels);
    int err;

    while (samples) {
        if (!self->fragment_started) {
            err = meter_fragment_start(self);
            if (err) return err;
        }

        size_t fragment_left = self->fragment_size - self->fragment_read;
        size_t to_scan = min(fragment_left, samples);
        #define CASE(fmt) case fmt: meter_scan_internal(self, buf, to_scan, fmt); break
        switch (self->sample_fmt) {
        CASE(AV_SAMPLE_FMT_U8);
        CASE(AV_SAMPLE_FMT_S16);
        CASE(AV_SAMPLE_FMT_S32);
        CASE(AV_SAMPLE_FMT_FLT);
        CASE(AV_SAMPLE_FMT_DBL);
        default:
            meter_scan_internal(self, buf, to_scan, static_cast< AVSampleFormat >( self->sample_fmt) );
        }
        #undef CASE
        buf = (char *)buf + self->sample_size * self->channels * to_scan;
        self->fragment_read += to_scan;

        if (self->fragment_size <= self->fragment_read) {
            meter_fragment_finish(self);
        }

        samples -= to_scan;
    }

    return 0;
}

int DynamicRange::meter_finish(struct dr_meter *self) {
    if (self->fragment_started) {
        meter_fragment_finish(self);
    }
    fprintf(stderr, "\nDoing some statistics...\n");
    sample rms_score[MAX_CHANNELS];
    sample rms[MAX_CHANNELS];
    sample peak_score[MAX_CHANNELS];
    sample dr_channel[MAX_CHANNELS];
    sample dr_sum = 0;
    for (int ch = 0; ch < self->channels; ch++) {
        qsort(self->rms_values[ch], self->fragment, sizeof(**self->rms_values), compare_samples);
        sample rms_sum = 0;
        size_t values_to_use = self->fragment / 5;
        for (size_t i = 0; i < values_to_use; i++) {
            sample value = self->rms_values[ch][i];
            rms_sum += value * value;
        }
        rms_score[ch] = sqrt(rms_sum / values_to_use);

        rms_sum = 0;
        for (size_t i = 0; i < self->fragment; i++) {
            sample value = self->rms_values[ch][i];
            rms_sum += value * value;
        }
        rms[ch] = sqrt(rms_sum / self->fragment);

        qsort(self->peak_values[ch], self->fragment, sizeof(*self->peak_values[ch]), compare_samples);
        peak_score[ch] = self->peak_values[ch][min(1, self->fragment)];

        dr_channel[ch] = to_db(peak_score[ch] / rms_score[ch]);
        printf("Ch. %2i:  Peak %8.2f (%8.2f)   RMS %8.2f (%8.2f)   DR = %6.2f\n",
               ch,
               to_db(self->peak_values[ch][0]),
               to_db(peak_score[ch]),
               to_db(rms[ch]),
               to_db(rms_score[ch]),
               dr_channel[ch]);
        dr_sum += dr_channel[ch];
    }
    printf("Overall dynamic range: DR%i\n",
           (int)round(dr_sum / ((sample)self->channels)));
    return 0;
}

void DynamicRange::meter_free(struct dr_meter *self) {
    for (int ch = 0; ch < self->channels; ch++) {
        free(self->rms_values[ch]);
        free(self->peak_values[ch]);
        self->rms_values[ch] = NULL;
        self->peak_values[ch] = NULL;
    }
}

/******************************************************************************/

int DynamicRange::print_av_error(const char *function_name, int error) {
    char errorbuf[128];
    char *error_ptr = errorbuf;
    if (av_strerror(error, errorbuf, sizeof(errorbuf)) < 0) {
        error_ptr = strerror(AVUNERROR(error));
    }
    fprintf(stderr, "dr_meter: %s: %s\n", function_name, error_ptr);
    return error;
}
}//namespace media
