#ifndef DYNAMICRANGE_H
#define DYNAMICRANGE_H

#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include "libavutil/error.h"

#ifdef __cplusplus
 #define __STDC_CONSTANT_MACROS
 #ifdef _STDINT_H
  #undef _STDINT_H
 #endif
 # include <stdint.h>
#endif
}

#define MAX_CHANNELS 32
#define MAX_FRAGMENTS 32768 // more than 24h

// The length of the window over which the RMS and peak are calculated.
// Specified in milliseconds. Don't change this!
#define FRAGMENT_LENGTH 3000

#define FACTOR8 ((sample)1.0 / (sample)(1 << 7))
#define FACTOR16 ((sample)1.0 / (sample)(1 << 15))
#define FACTOR32 ((sample)1.0 / (sample)(1UL << 31))

namespace media {

class DynamicRange {
public:
    DynamicRange();

    int calculate ( const std::string& filename );

private:
    typedef double sample;
    const char throbbler[5] = "/-\\|";

    struct stream_context {
        AVFormatContext *format_ctx;
        int stream_index; // the stream we are decoding
        AVPacket real_pkt;
        AVPacket pkt;
        enum {
            STATE_UNINITIALIZED,
            STATE_INITIALIZED,
            STATE_OPEN,
            STATE_VALID_PACKET,
            STATE_NEED_FLUSH,
            STATE_CLOSED,
        } state;
        void *buf;
        size_t buf_size; // the number of bytes present
        size_t buf_alloc_size; // the number of bytes allocated
    };

    struct dr_meter {
        sample *rms_values[MAX_CHANNELS];
        sample *peak_values[MAX_CHANNELS];

        sample sum[MAX_CHANNELS];
        sample peak[MAX_CHANNELS];

        int channels;
        int sample_rate;
        int sample_fmt;
        int sample_size;

        size_t fragment; // The index of the current fragment
        size_t fragment_size; // The size of a fragment in samples
        size_t fragment_read; // The number of samples scanned so far
        bool fragment_started;
    };

    void sc_init(struct stream_context *self);
    int sc_open(struct stream_context *self, const char *filename);
    AVCodecContext *sc_get_codec(struct stream_context *self);
    void sc_close(struct stream_context *self);
    bool sc_eof(struct stream_context *self);
    int sc_start_stream(struct stream_context *self, int stream_index);
    int sc_get_next_frame(struct stream_context *self);
    static int compare_samples(const void *s1, const void *s2);
    sample get_sample(void *buf, size_t i, enum AVSampleFormat sample_fmt);
    sample to_db(const sample linear);
    void meter_init(struct dr_meter *self);
    int meter_start(struct dr_meter *self, int channels, int sample_rate, int sample_fmt);
    static int meter_fragment_start(struct dr_meter *self);
    static void meter_fragment_finish(struct dr_meter *self);
    /*static*/ inline void meter_scan_internal(struct dr_meter *self, void *buf, size_t samples, enum AVSampleFormat sample_fmt);
    int meter_feed(struct dr_meter *self, void *buf, size_t buf_size);
    int meter_finish(struct dr_meter *self);
    void meter_free(struct dr_meter *self);
    int print_av_error(const char *function_name, int error);

};
}//namespace media
#endif // DYNAMICRANGE_H
