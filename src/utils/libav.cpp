//#include "libav.h"

//#include "easylogging++.h"

//namespace libav {


//std::string Metadata::name( Metadata::Enum tag ) {
//    switch ( tag ) {
//    case TITLE: return "title";
//    case ALBUM: return "album,";
//    case ARTIST: return "artist";
//    case COMPOSER: return "album_artist";
//    case PERFORMER: return "performer"; //TODO does not exist in ffmpeg
//    case COMMENT: return "comments"; //TODO does not exist in ffmpeg
//    case TRACK: return "track";
//    case DISC: return "disc";
//    case GENRE: return "genre";
//    case PUBLISHER: return "publisher";
//    case YEAR: return "date";
//    }
//}

//std::string& Metadata::get ( const Metadata::Enum& key ) {
//    return tags[ key ];
//}

//std::vector< Metadata::Enum > Metadata::tag_names () {
//    std::vector< Enum > _keys;
//    for( auto const& k: tags )
//        _keys.push_back( k.first );
//    return _keys;
//}

//MediaType::Enum MediaType::get( AVMediaType type ) {
//    switch( type ) {
//    case AVMEDIA_TYPE_VIDEO:    return VIDEO;
//    case AVMEDIA_TYPE_AUDIO:    return AUDIO;
//    case AVMEDIA_TYPE_DATA:     return DATA;
//    case AVMEDIA_TYPE_SUBTITLE: return SUBTITLE;
//    case AVMEDIA_TYPE_ATTACHMENT: return ATTACHMENT;
//    case AVMEDIA_TYPE_NB:       return NB;
//    default: return UNKNOWN;
//    }
//}

///** The output bit rate in kbit/s */
//#define OUTPUT_BIT_RATE 96000
///** The number of output channels */
//#define OUTPUT_CHANNELS 2

//Codec::Enum Codec::get( AVCodecID codec_id ) {
//    switch( codec_id ) {
//    case AV_CODEC_ID_MP3:  return MP3;
//    case AV_CODEC_ID_FLAC:  return FLAC;
//    case AV_CODEC_ID_AAC:  return AAC;
//    case AV_CODEC_ID_VORBIS:  return VORBIS;
//    default: return NONE;
//    }
//}
//AVCodecID Codec::get( Codec::Enum codec ) {
//    switch( codec ) {
//    case MP3: return AV_CODEC_ID_MP3;
//    case FLAC: return AV_CODEC_ID_FLAC;
//    case AAC: return AV_CODEC_ID_AAC;
//    case VORBIS: return AV_CODEC_ID_VORBIS;
//    default: return AV_CODEC_ID_NONE;
//    }
//}

//MediaInfo Libav::media_info ( const int index ) {
//    Metadata _metadata;
//    AVDictionaryEntry *tag = NULL;

//    tag = av_dict_get( input_format_context->metadata, "title", nullptr, AV_DICT_MATCH_CASE|AV_DICT_IGNORE_SUFFIX );
//    if( tag ) { std::cout << "TAG title " << tag->value << std::endl; }

//    tag = av_dict_get( input_format_context->metadata, "album", nullptr, AV_DICT_MATCH_CASE|AV_DICT_IGNORE_SUFFIX );
//    if( tag ) { std::cout << "TAG album " << tag->value << std::endl; }

//    tag = av_dict_get( input_format_context->metadata, "artist", nullptr, AV_DICT_MATCH_CASE|AV_DICT_IGNORE_SUFFIX );
//    if( tag ) { std::cout << "TAG artist " << tag->value << std::endl; }

//    while ( ( tag = av_dict_get( input_format_context->metadata, "", tag, AV_DICT_MATCH_CASE|AV_DICT_IGNORE_SUFFIX ) ) ) {
//        _metadata.set( tag->key, tag->value );
//    }
//    AVCodecContext* codec = input_format_context->streams[index]->codec;
//    return (
//        MediaInfo {
//            MediaType::get( codec->codec_type ),
//            Codec::get( codec->codec_id ),
//            codec->bit_rate,
//            codec->sample_rate,
//            codec->channels,
//            input_format_context->streams[index]->duration,
//            codec->bits_per_coded_sample,
//            codec->width,
//            codec->height,
//            _metadata
//        }
//    );
//}

//Libav::Libav () {
//    std::call_once( av_register_flag, av_register_all );

//    avcodec_register_all();
//        av_register_all();
//}

//Libav::~Libav() {}std::error_code Libav::input ( const std::string& filename ) {

//    int error;
//    std::error_code _errc;

//    /** Open the input file to read from it. */
//    AVFormatContext* _if_ctx = nullptr;
//    if ( ( error = avformat_open_input( &_if_ctx, filename.c_str(), NULL, NULL)) < 0 )
//    { return make_error_code ( error ); }
//    input_format_context = std::shared_ptr< AVFormatContext >( _if_ctx,
//        [] ( AVFormatContext* p ) { if ( p ) avformat_close_input( &p ); } );

//    /** Get information on the input file (number of streams etc.). */
//    if ( ( error = avformat_find_stream_info( input_format_context.get(), NULL ) ) < 0 )
//    { return make_error_code ( error ); };

//    /** Make sure that there is only one stream in the input file. */
////TODO    if ( ( input_format_context.get() )->nb_streams != 1 ) {
////        //TODO remove
////        fprintf(stderr, "Expected one audio input stream, but found %d\n",
////                (input_format_context.get())->nb_streams);
////        return std::error_code ( av_errc::UNKNOWN, av_category );
////    }

//    /** Find a decoder for the audio stream. */
//    AVCodec* input_codec = avcodec_find_decoder( input_format_context.get()->streams[0]->codec->codec_id );
//    if ( !input_codec ) {
//        CLOG(ERROR, "cds") << "Could not find input codec (" << filename << ")";

//        fprintf(stderr, "\n");
//        return make_error_code ( AV_DECODER_NOT_FOUND );
//    }

//    /** Open the decoder for the audio stream to use it later. */
//    if ((error = avcodec_open2( input_format_context.get()->streams[0]->codec,
//                               input_codec, NULL)) < 0) {
////        fprintf(stderr, "Could not open input codec (error '%s')\n", get_error_text(error));
//        return std::error_code ( error, av_category );
////        avformat_close_input(input_format_context);
////        return error;
//    }

//    /** Save the decoder context for easier access later. */
//    input_codec_context = std::shared_ptr< AVCodecContext >( input_format_context->streams[0]->codec,
//        [] ( AVCodecContext* p ) { if ( p ) avcodec_close( p ); } );

//    return std::error_code();
//}

//#define FLAGS (o->type == AV_OPT_TYPE_FLAGS && (arg[0]=='-' || arg[0]=='+')) ? AV_DICT_APPEND : 0

//std::error_code Libav::output ( const std::string& filename ) {

////    av_opt_show2(output_format_context.get()->streams[0]->codec, NULL, AV_OPT_FLAG_ENCODING_PARAM, 0);
////    av_opt_set( output_format_context.get()->, "id3v2_version", "3", AV_OPT_SEARCH_CHILDREN );


//    int error;

//    /** Open the output file to write to it. */
//    AVIOContext* output_io_context = nullptr;
//    if ((error = avio_open(&output_io_context, filename.c_str(), AVIO_FLAG_WRITE)) < 0)
//    {         return std::error_code ( error, av_category ); }
////        fprintf(stderr, "Could not open output file '%s' (error '%s')\n",
////                filename, get_error_text(error));
////        return error;
////    }

//    /** Create a new format context for the output container format. */
//    AVFormatContext* _ofmt_ctx = nullptr;
//    if ( ! ( _ofmt_ctx = avformat_alloc_context() ) ) {
//        fprintf(stderr, "Could not allocate output format context\n");
//        return std::error_code ( ENOMEM, av_category );
//        //return AVERROR(ENOMEM);
//    }
//    output_format_context = std::shared_ptr< AVFormatContext >( _ofmt_ctx,
//        [] ( AVFormatContext* p ) { if ( p ) avformat_close_input( &p ); } );

//    /** Associate the output file (pointer) with the container format context. */
//    output_format_context.get()->pb = output_io_context;

//    /** Guess the desired container format based on the file extension. */
//    if ( !( output_format_context.get()->oformat = av_guess_format( NULL, filename.c_str(), NULL))) {
//        fprintf(stderr, "Could not find output file format\n");
//        return std::error_code ( ENOMEM, av_category );
////        goto cleanup;
//    }

//    av_strlcpy( output_format_context.get()->filename, filename.c_str(), sizeof( output_format_context.get()->filename ) );

//    /** Find the encoder to be used by its name. */
//    AVCodec* output_codec = nullptr;
//    if ( !(output_codec = avcodec_find_encoder( AV_CODEC_ID_MP3 ) ) ) {
//        fprintf(stderr, "Could not find an AAC encoder.\n");
//        return std::error_code ( ENOMEM, av_category );
////        goto cleanup;
//    }

//    /** Create a new audio stream in the output file container. */
//    AVStream *stream = nullptr;
//    if (!(stream = avformat_new_stream( output_format_context.get(), output_codec))) {
//        fprintf(stderr, "Could not create new stream\n");
//        return std::error_code ( ENOMEM, av_category );
////        error = AVERROR(ENOMEM);
////        goto cleanup;
//    }

//    /** Save the encoder context for easier access later. */
//    output_codec_context = std::shared_ptr< AVCodecContext >( stream->codec,
//        [] ( AVCodecContext* /*p*/ ) { /* if ( p ) avformat_close_input( &p ); */ } );

//    /**
//     * Set the basic encoder parameters.
//     * The input file's sample rate is used to avoid a sample rate conversion.
//     */
//    output_codec_context.get()->channels       = OUTPUT_CHANNELS;
//    output_codec_context.get()->channel_layout = av_get_default_channel_layout(OUTPUT_CHANNELS);
//    output_codec_context.get()->sample_rate    = input_codec_context.get()->sample_rate;
//    output_codec_context.get()->sample_fmt     = output_codec->sample_fmts[0];
//    output_codec_context.get()->bit_rate       = OUTPUT_BIT_RATE;

//    /** Allow the use of the experimental AAC encoder */
//    output_codec_context.get()->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

//    /** Set the sample rate for the container. */
//    stream->time_base.den = input_codec_context->sample_rate;
//    stream->time_base.num = 1;

//    /**
//     * Some container formats (like MP4) require global headers to be present
//     * Mark the encoder so that it behaves accordingly.
//     */
//    if ( output_format_context.get()->oformat->flags & AVFMT_GLOBALHEADER)
//        output_codec_context.get()->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;


//    /** Open the encoder for the audio stream to use it later. */
//    if ((error = avcodec_open2( output_codec_context.get(), output_codec, NULL)) < 0) {
//        return std::error_code ( error, av_category );
////        fprintf(stderr, "Could not open output codec (error '%s')\n",
////                get_error_text(error));
////        goto cleanup;
//    }

//    return std::error_code ();

////cleanup: //TODO do the cleanup
////    avio_closep(&(*output_format_context)->pb);
////    avformat_free_context(*output_format_context);
////    *output_format_context = NULL;
////    return error < 0 ? error : AVERROR_EXIT;

//}

//std::error_code Libav::transcode ( MediaInfo& media_info ) {

//    std::error_code error;
//    /** Initialize the resampler to be able to convert audio sample formats. */
//    if ( ( error = init_resampler () ) )
//        return error;
//    /** Initialize the FIFO buffer to store audio samples to be encoded. */
//    if ( ( error = init_fifo( ) ) )
//         return error;
//    /** Write the header of the output file container. */

//    //https://www.ffmpeg.org/doxygen/3.0/muxing_8c-example.html#_a81


//    AVDictionary *d = output_format_context->metadata;           // "create" an empty dictionary

////    for( auto _tag : media_info.metadata.tag_names() ) {
////        std::cout << ">" <<  Metadata::name(_tag ).c_str() << "=" << media_info.metadata.get( _tag ).c_str() << std::endl;
////        av_dict_set(&d, Metadata::name(_tag ).c_str(), media_info.metadata.get( _tag ).c_str(), 0); // add an entry
//////        av_dict_set(&(output_format_context->metadata), Metadata::name(_tag ).c_str(), media_info.metadata.get( _tag ).c_str(), 0);
////    }
//    av_dict_set(&d, "title", "Test Title", 0); // add an entry
//    output_format_context->metadata = d;

//    // https://www.ffmpeg.org/doxygen/3.2/group__lavf__encoding.html#ga169cfa28508e22e138c5b99be8517ea4

//    AVDictionary *format_opts = nullptr;
//    av_dict_set( &format_opts, "id3v2_version", "3", 0 );

//    int err;
//    if ( ( err = avformat_write_header(output_format_context.get(), &format_opts ) ) < 0) {
//        return std::error_code( err, av_category );
//    }
////    av_dict_free(&opt);

//    av_dump_format(output_format_context.get(), 0, "", 1);

//    /**
//     * Loop as long as we have input samples to read or output samples
//     * to write; abort as soon as we have neither.
//     */
//    while (1) {
//        /** Use the encoder's desired frame size for processing. */
//        const int output_frame_size = output_codec_context->frame_size;
//        int finished                = 0;

//        /**
//         * Make sure that there is one frame worth of samples in the FIFO
//         * buffer so that the encoder can do its work.
//         * Since the decoder's and the encoder's frame size may differ, we
//         * need to FIFO buffer to store as many frames worth of input samples
//         * that they make up at least one frame worth of output samples.
//         */
//        while ( av_audio_fifo_size(fifo.get() ) < output_frame_size ) {
//            /**
//             * Decode one frame worth of audio samples, convert it to the
//             * output sample format and put it into the FIFO buffer.
//             */
//            if ( ( error = read_decode_convert_and_store( &finished ) ) )
//                return error;

//            /**
//             * If we are at the end of the input file, we continue
//             * encoding the remaining audio samples to the output file.
//             */
//            if (finished)
//                break;
//        }

//        /**
//         * If we have enough samples for the encoder, we encode them.
//         * At the end of the file, we pass the remaining samples to
//         * the encoder.
//         */
//        while ( av_audio_fifo_size( fifo.get() ) >= output_frame_size ||
//               (finished && av_audio_fifo_size( fifo.get() ) > 0))
//            /**
//             * Take one frame worth of audio samples from the FIFO buffer,
//             * encode it and write it to the output file.
//             */
//            if ( ( error = load_encode_and_write() ) )
//                return error;

//        /**
//         * If we are at the end of the input file and have encoded
//         * all remaining samples, we can exit this loop and finish.
//         */
//        if (finished) {
//            int data_written;
//            /** Flush the encoder as it may have delayed frames. */
//            do {
//                if ( ( error = encode_audio_frame(NULL, &data_written ) ) )
//                    return error;
//            } while (data_written);
//            break;
//        }
//    }

//    /** Write the trailer of the output file container. */
//    if ( ( err = av_write_trailer(output_format_context.get() ) ) < 0 ) {
////        fprintf(stderr, "Could not write output file trailer (error '%s')\n",
////                get_error_text(error));
//        return std::error_code ( err, av_category );
//    }

//    return std::error_code ();
//}

///**
// * Initialize the audio resampler based on the input and output codec settings.
// * If the input and output sample formats differ, a conversion is required
// * libswresample takes care of this, but requires initialization.
// */
//std::error_code Libav::init_resampler() {
//    int error;

//    /**
//     * Create a resampler context for the conversion.
//     * Set the conversion parameters.
//     * Default channel layouts based on the number of channels
//     * are assumed for simplicity (they are sometimes not detected
//     * properly by the demuxer and/or decoder).
//     */
//    SwrContext *re_context;
//    re_context = swr_alloc_set_opts(NULL,
//                                          av_get_default_channel_layout(output_codec_context->channels),
//                                          output_codec_context->sample_fmt,
//                                          output_codec_context->sample_rate,
//                                          av_get_default_channel_layout(input_codec_context->channels),
//                                          input_codec_context->sample_fmt,
//                                          input_codec_context->sample_rate,
//                                          0, NULL);
//    if ( !re_context ) {
//        fprintf(stderr, "Could not allocate resample context\n");
//        //return AVERROR(ENOMEM);
//        return std::error_code ( ENOMEM, av_category );
//    }

//    /**
//    * Perform a sanity check so that the number of converted samples is
//    * not greater than the number of samples to be converted.
//    * If the sample rates differ, this case has to be handled differently
//    */
//    av_assert0(output_codec_context->sample_rate == input_codec_context->sample_rate);

//    /** Open the resampler with the specified parameters. */
//    if ( ( error = swr_init( re_context ) ) < 0 ) {
//        fprintf(stderr, "Could not open resample context\n");
//        swr_free( &re_context );
//        return std::error_code ( error, av_category );
//    }

//    resample_context = std::shared_ptr< SwrContext >( re_context,
//        [] ( SwrContext* p ) { if ( p ) swr_free(&p); } );

//    return std::error_code ();
//}

///** Initialize a FIFO buffer for the audio samples to be encoded. */
//std::error_code Libav::init_fifo() {
//    /** Create the FIFO buffer based on the specified output sample format. */
//    AVAudioFifo* _f = NULL;
//    if ( ! (_f = av_audio_fifo_alloc( output_codec_context->sample_fmt,
//                                      output_codec_context->channels, 1 ) ) ) {
//        fprintf(stderr, "Could not allocate FIFO\n");
//        return std::error_code ( ENOMEM, av_category );
//    }

//    fifo = std::shared_ptr< AVAudioFifo >( _f,
//        [] ( AVAudioFifo* p ) { if ( p ) av_audio_fifo_free( p ); } );

//    return std::error_code ();
//}

///**
// * Read one audio frame from the input file, decodes, converts and stores
// * it in the FIFO buffer.
// */
//std::error_code Libav::read_decode_convert_and_store( int *finished) {
//    /** Temporary storage of the input samples of the frame read from the file. */
//    AVFrame *input_frame = NULL;
//    /** Temporary storage for the converted input samples. */
//    uint8_t **converted_input_samples = NULL;
//    int data_present;
//    int ret = AVERROR_EXIT; //TODO remove
//    int error;

//    /** Initialize temporary storage for one input frame. */
//    if ( !( input_frame = av_frame_alloc() ) ) {
//        fprintf(stderr, "Could not allocate input frame\n");
//        return std::error_code( ENOMEM, av_category );
//    } //TODO cleanup

//    /** Decode one frame worth of audio samples. */
//    if ( decode_audio_frame( input_frame, &data_present, finished ) )
//        goto cleanup;
//    /**
//     * If we are at the end of the file and there are no more samples
//     * in the decoder which are delayed, we are actually finished.
//     * This must not be treated as an error.
//     */
//    if ( *finished && !data_present ) {
//        ret = 0;
//        goto cleanup;
//    }
//    /** If there is decoded data, convert and store it */
//    if (data_present) {
//        /** Initialize the temporary storage for the converted input samples. */
//        if ( init_converted_samples(&converted_input_samples, input_frame->nb_samples))
//            goto cleanup;

//        /**
//         * Convert the input samples to the desired output sample format.
//         * This requires a temporary storage provided by converted_input_samples.
//         */
//        std::error_code err; //TODO
//        if ( ( err = convert_samples ( ( const uint8_t**)input_frame->extended_data, converted_input_samples,
//                            input_frame->nb_samples ) ) )
//            return err; //TODO cleanup

//        /** Add the converted input samples to the FIFO buffer for later processing. */
//        if ( add_samples_to_fifo(fifo.get(), converted_input_samples,
//                                 input_frame->nb_samples))
//            goto cleanup;
//        ret = 0;
//    }
//    ret = 0;

//cleanup:
//    if (converted_input_samples) {
//        av_freep(&converted_input_samples[0]);
//        free(converted_input_samples);
//    }
//    av_frame_free(&input_frame);

//    return std::error_code( ret, av_category );
//}

///** Decode one audio frame from the input file. */
//std::error_code Libav::decode_audio_frame( AVFrame *frame, int *data_present, int *finished) {
//    /** Packet used for temporary storage. */
//    AVPacket input_packet; //TODO cleanup
//    int error;

//    av_init_packet(&input_packet);
//    /** Set the packet data and size so that it is recognized as being empty. */
//    input_packet.data = NULL;
//    input_packet.size = 0;

//    /** Read one audio frame from the input file into a temporary packet. */
//    if ((error = av_read_frame(input_format_context.get(), &input_packet)) < 0) {
//        /** If we are at the end of the file, flush the decoder below. */
//        if (error == AVERROR_EOF)
//            *finished = 1;
//        else {
//            return std::error_code( error, av_category );
//        }
//    }

//    /**
//     * Decode the audio frame stored in the temporary packet.
//     * The input audio stream decoder is used to do this.
//     * If we are at the end of the file, pass an empty packet to the decoder
//     * to flush it.
//     */
//    if ((error = avcodec_decode_audio4(input_codec_context.get(), frame,
//                                       data_present, &input_packet)) < 0) {
//        av_free_packet(&input_packet);
//        return std::error_code( error, av_category );
//    }

//    /**
//     * If the decoder has not been flushed completely, we are not finished,
//     * so that this function has to be called again.
//     */
//    if (*finished && *data_present)
//        *finished = 0;
//    av_free_packet(&input_packet);
//    return std::error_code ();
//}

///**
// * Initialize a temporary storage for the specified number of audio samples.
// * The conversion requires temporary storage due to the different format.
// * The number of audio samples to be allocated is specified in frame_size.
// */
//std::error_code Libav::init_converted_samples(uint8_t ***converted_input_samples, int frame_size) {
//    int error;

//    /**
//     * Allocate as many pointers as there are audio channels.
//     * Each pointer will later point to the audio samples of the corresponding
//     * channels (although it may be NULL for interleaved formats).
//     */
//    if (!(*converted_input_samples = (uint8_t**)calloc(output_codec_context->channels,
//                                            sizeof(**converted_input_samples)))) {
//        fprintf(stderr, "Could not allocate converted input sample pointers\n");
//        return std::error_code ( ENOMEM, av_category );
//    }

//    /**
//     * Allocate memory for the samples of all channels in one consecutive
//     * block for convenience.
//     */
//    if ((error = av_samples_alloc(*converted_input_samples, NULL,
//                                  output_codec_context->channels,
//                                  frame_size,
//                                  output_codec_context->sample_fmt, 0)) < 0) {
//        av_freep(&(*converted_input_samples)[0]);
//        free(*converted_input_samples);
//        return std::error_code ( error, av_category );
//    }
//    return std::error_code();
//}

///**
// * Convert the input audio samples into the output sample format.
// * The conversion happens on a per-frame basis, the size of which is specified
// * by frame_size.
// */
//std::error_code Libav::convert_samples(const uint8_t **input_data,
//                           uint8_t **converted_data, const int frame_size ) {
//    int error;

//    /** Convert the samples using the resampler. */
//    if ( ( error = swr_convert(resample_context.get(),
//                             converted_data, frame_size,
//                             input_data    , frame_size ) ) < 0 ) {

//        return std::error_code( error, av_category );
//    }

//    return std::error_code();
//}

///** Add converted input audio samples to the FIFO buffer for later processing. */
//std::error_code Libav::add_samples_to_fifo(AVAudioFifo *fifo,
//                               uint8_t **converted_input_samples,
//                               const int frame_size) {
//    int error;

//    /**
//     * Make the FIFO as large as it needs to be to hold both,
//     * the old and the new samples.
//     */
//    if ((error = av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + frame_size)) < 0) {
//        fprintf(stderr, "Could not reallocate FIFO\n");
//        return std::error_code( error, av_category );
//    }

//    /** Store the new samples in the FIFO buffer. */
//    if (av_audio_fifo_write(fifo, (void **)converted_input_samples,
//                            frame_size) < frame_size) {
//        fprintf(stderr, "Could not write data to FIFO\n");
//        return std::error_code( error, av_category );
//    }
//    return std::error_code ();
//}

///**
// * Load one audio frame from the FIFO buffer, encode and write it to the
// * output file.
// */
//std::error_code Libav::load_encode_and_write () {
//    /** Temporary storage of the output samples of the frame written to the file. */
//    AVFrame *output_frame;
//    /**
//     * Use the maximum number of possible samples per frame.
//     * If there is less than the maximum possible frame size in the FIFO
//     * buffer use this number. Otherwise, use the maximum possible frame size
//     */
//    const int frame_size = FFMIN(av_audio_fifo_size( fifo.get() ),
//                                 output_codec_context->frame_size);
//    int data_written;

//    /** Initialize temporary storage for one output frame. */
//    std::error_code errc;
//    if ( ( errc = init_output_frame(&output_frame, frame_size ) ) )
//        return errc;

//    /**
//     * Read as many samples from the FIFO buffer as required to fill the frame.
//     * The samples are stored in the frame temporarily.
//     */
//    if (av_audio_fifo_read(fifo.get(), (void **)output_frame->data, frame_size) < frame_size) {
//        fprintf(stderr, "Could not read data from FIFO\n");
//        av_frame_free(&output_frame);
//        return std::error_code( AVERROR_EXIT, av_category );
//    }

//    /** Encode one frame worth of audio samples. */
//    if (encode_audio_frame(output_frame, &data_written)) {
//        av_frame_free(&output_frame);
//        return std::error_code( AVERROR_EXIT, av_category );
//    }
//    av_frame_free(&output_frame);
//    return std::error_code ();
//}

///**
// * Initialize one input frame for writing to the output file.
// * The frame will be exactly frame_size samples large.
// */
//std::error_code Libav::init_output_frame(AVFrame **frame, int frame_size) {
//    int error;

//    /** Create a new frame to store the audio samples. */
//    if (!(*frame = av_frame_alloc())) {
//        fprintf(stderr, "Could not allocate output frame\n");
//        return std::error_code( AVERROR_EXIT, av_category );
//    }

//    /**
//     * Set the frame's parameters, especially its size and format.
//     * av_frame_get_buffer needs this to allocate memory for the
//     * audio samples of the frame.
//     * Default channel layouts based on the number of channels
//     * are assumed for simplicity.
//     */
//    (*frame)->nb_samples     = frame_size;
//    (*frame)->channel_layout = output_codec_context->channel_layout;
//    (*frame)->format         = output_codec_context->sample_fmt;
//    (*frame)->sample_rate    = output_codec_context->sample_rate;

//    /**
//     * Allocate the samples of the created frame. This call will make
//     * sure that the audio frame can hold as many samples as specified.
//     */
//    if ((error = av_frame_get_buffer(*frame, 0)) < 0) {
//        av_frame_free(frame);
//        return std::error_code( error, av_category );
//    }

//    return std::error_code();
//}

///** Encode one frame worth of audio to the output file. */
//std::error_code Libav::encode_audio_frame(AVFrame *frame, int *data_present) {
//    /** Packet used for temporary storage. */
//    AVPacket output_packet;
//    int error;

//    av_init_packet(&output_packet);
//    /** Set the packet data and size so that it is recognized as being empty. */
//    output_packet.data = NULL;
//    output_packet.size = 0;

//    /** Set a timestamp based on the sample rate for the container. */
//    if (frame) {
//        frame->pts = pts;
//        pts += frame->nb_samples;
//    }

//    /**
//     * Encode the audio frame and store it in the temporary packet.
//     * The output audio stream encoder is used to do this.
//     */
//    if ((error = avcodec_encode_audio2(output_codec_context.get(), &output_packet,
//                                       frame, data_present)) < 0) {
////        fprintf(stderr, "Could not encode frame (error '%s')\n",
////                get_error_text(error));
//        av_free_packet(&output_packet);
//        return std::error_code( error, av_category );
//    }

//    /** Write one audio frame from the temporary packet to the output file. */
//    if (*data_present) {
//        if ((error = av_write_frame(output_format_context.get(), &output_packet)) < 0) {
////            fprintf(stderr, "Could not write frame (error '%s')\n",
////                    get_error_text(error));
//            av_free_packet(&output_packet);
//            return std::error_code( error, av_category );
//        }

//        av_free_packet(&output_packet);
//    }

//    return std::error_code ();
//}


//}//namespace libav
