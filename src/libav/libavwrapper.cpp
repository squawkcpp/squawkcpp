//#include "libavwrapper.h"

//namespace libav {
//std::once_flag LibavWrapper::_register_flag;
//LibavWrapper::LibavWrapper() {
//    std::call_once ( LibavWrapper::_register_flag, []() { av_register_all(); } );
//}
//std::error_code LibavWrapper::make_input_format_context ( const char* filename ) {
//    int _error;
//    /** Open the input file to read from it. */
//    AVFormatContext* _input_format_context = nullptr;

//    if ( ( _error = avformat_open_input ( &_input_format_context, filename, NULL, NULL ) ) < 0 )
//    { return libav::make_error_code ( _error ); }

//    input_format_context = std::shared_ptr< AVFormatContext > ( _input_format_context,
//    [] ( AVFormatContext* p ) { if ( p ) { avformat_close_input ( &p ); } } );

//    /** Get information on the input file (number of streams etc.). */
//    if ( ( _error = avformat_find_stream_info ( input_format_context.get(), NULL ) ) < 0 )
//    { return libav::make_error_code ( _error ); };

//    return std::error_code ();
//}
//std::error_code LibavWrapper::make_input_format_context ( std::istream* istream ) {
//    input_io_context = std::make_shared< LibavIoContext >();
//    AVFormatContext* _input_format_context = avformat_alloc_context();
//    input_io_context->init_input_format_context ( _input_format_context, istream );

//    input_format_context = std::shared_ptr< AVFormatContext > ( _input_format_context,
//    [] ( AVFormatContext* p ) { if ( p ) { avformat_close_input ( &p ); } } );

//    int _error = 0;

//    if ( ( _error = avformat_open_input ( &_input_format_context, "", nullptr, nullptr ) ) != 0 )
//    { return libav::make_error_code ( _error ); };

//    if ( ( _error = avformat_find_stream_info ( _input_format_context, nullptr ) ) < 0 ) {
//        { return libav::make_error_code ( _error ); };
//    }

//    return std::error_code ();
//}
//std::error_code LibavWrapper::make_output_format_context ( const char* filename ) {

//    int error;

//    /** Open the output file to write to it. */
//    AVIOContext* _output_io_context = nullptr;

//    if ( ( error = avio_open ( &_output_io_context, filename, AVIO_FLAG_WRITE ) ) < 0 )
//    { return make_error_code ( error ); }

//    /** Create a new format context for the output container format. */
//    AVFormatContext* _output_format_context = nullptr;

//    if ( ! ( _output_format_context  = avformat_alloc_context() ) )
//    { return make_error_code ( error ); }

//    /** Associate the output file (pointer) with the container format context. */
//    _output_format_context->pb = _output_io_context ;

//    /** Guess the desired container format based on the file extension. */
//    if ( ! ( _output_format_context ->oformat = av_guess_format ( NULL, filename, NULL ) ) )
//    { return make_error_code ( error ); }

//    av_strlcpy ( _output_format_context->filename, filename, sizeof ( _output_format_context->filename ) );

//    output_format_context = std::shared_ptr< AVFormatContext > ( _output_format_context ,
//    [] ( AVFormatContext* p ) { if ( p ) { avformat_close_input ( &p ); } } );

//    return std::error_code ();
//}
//std::error_code LibavWrapper::make_output_format_context ( Codec::Enum codec ) {

//    output_io_context = std::make_shared< LibavIoContext >();
//    AVFormatContext* _output_format_context = avformat_alloc_context();
//    _output_format_context->pb = output_io_context->av_io_context_;
//    _output_format_context->flags |= AVFMT_FLAG_CUSTOM_IO;

//    AVOutputFormat* of = av_guess_format ( Codec::name ( codec ).c_str(), NULL, NULL );

//    if ( of == NULL ) { make_error_code ( AV_ENCODER_NOT_FOUND );  }

//    _output_format_context->oformat = of;

//    if ( of->audio_codec == AV_CODEC_ID_NONE ) { make_error_code ( AV_ENCODER_NOT_FOUND );  }

//    output_io_context->init_output_format_context ( _output_format_context );

//    output_format_context = std::shared_ptr< AVFormatContext > ( _output_format_context,
//    [] ( AVFormatContext* p ) { if ( p ) { avformat_close_input ( &p ); } } );

//    return std::error_code();
//}
//std::error_code LibavWrapper::make_input_codec_context ( const int stream_number ) {

//    /** Find a decoder for the audio stream. */
//    AVCodec* codec_ = nullptr;

//    if ( ! ( codec_ = avcodec_find_decoder ( input_format_context->streams[stream_number]->codec->codec_id ) ) ) {
//        return make_error_code ( AV_DECODER_NOT_FOUND );
//    }

//    int error;

//    /** Open the decoder for the audio stream to use it later. */
//    if ( ( error = avcodec_open2 ( input_format_context.get()->streams[stream_number]->codec, codec_, NULL ) ) < 0 ) {
//        return make_error_code ( error );
//    }

//    //store the codec for direct access
//    input_codec_context = std::shared_ptr< AVCodecContext > ( input_format_context->streams[stream_number]->codec,
//    [] ( AVCodecContext* p ) { if ( p ) { avcodec_close ( p ); } } );

//    return std::error_code ();
//}
//std::error_code LibavWrapper::make_output_codec_context ( const MediaInfo& media_info ) {

//    /** Find the encoder to be used by its name. */
//    AVCodec* _output_codec_context = nullptr;

//    if ( ! ( _output_codec_context = avcodec_find_encoder ( av_codec_id ( media_info.codec ) ) ) )
//    { return make_error_code ( AV_ENCODER_NOT_FOUND ); }

//    /** Create a new audio stream in the output file container. */
//    AVStream *_stream = nullptr;

//    if ( ! ( _stream = avformat_new_stream ( output_format_context.get(), _output_codec_context ) ) )
//    { return make_error_code ( ENOMEM ); }

//    /** Save the encoder context for easier access later. */
//    output_codec_context = _stream->codec;

//    /* Set the basic encoder parameters. */
//    output_codec_context->channels       = media_info.channels;
//    output_codec_context->channel_layout = av_get_default_channel_layout ( media_info.channels );
//    output_codec_context->sample_rate    = media_info.samplerate;
//    output_codec_context->sample_fmt     = _output_codec_context->sample_fmts[0];
//    output_codec_context->bit_rate       = media_info.bitrate;

//    /* Allow the use of the experimental AAC encoder */
//    output_codec_context->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

//    /** Set the sample rate for the container. */
//    _stream->time_base.den = media_info.samplerate;
//    _stream->time_base.num = 1;

//    /**
//     * Some container formats (like MP4) require global headers to be present
//     * Mark the encoder so that it behaves accordingly.
//     */
//    if ( output_format_context->oformat->flags & AVFMT_GLOBALHEADER )
//    { output_codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; }

//    /** Open the encoder for the audio stream to use it later. */
//    int error;

//    if ( ( error = avcodec_open2 ( output_codec_context, _output_codec_context, NULL ) ) < 0 )
//    { return make_error_code ( error ); }

//    return std::error_code ();
//}
//std::error_code LibavWrapper::make_resampler_context() {

//    int error;

//    /* Create a resampler context for the conversion. */
//    SwrContext *re_context;
//    re_context = swr_alloc_set_opts ( NULL,
//                                      av_get_default_channel_layout ( output_codec_context->channels ),
//                                      output_codec_context->sample_fmt,
//                                      output_codec_context->sample_rate,
//                                      av_get_default_channel_layout ( input_codec_context->channels ),
//                                      input_codec_context->sample_fmt,
//                                      input_codec_context->sample_rate,
//                                      0, NULL );

//    if ( !re_context )
//    { return make_error_code ( ENOMEM ); }

//    resample_context = std::shared_ptr< SwrContext > ( re_context,
//    [] ( SwrContext* p ) { if ( p ) { swr_free ( &p ); } } );

//    if ( ( error = swr_init ( re_context ) ) < 0 ) {
//        swr_free ( &re_context );
//        return make_error_code ( error );
//    }

//    return std::error_code ();
//}
//std::error_code LibavWrapper::make_fifo() {

//    /** Create the FIFO buffer based on the specified output sample format. */
//    AVAudioFifo* _fifo = NULL;

//    if ( ! ( _fifo = av_audio_fifo_alloc ( output_codec_context->sample_fmt,
//                                           output_codec_context->channels, 1 ) ) )
//    { return make_error_code ( ENOMEM ); }

//    fifo = std::shared_ptr< AVAudioFifo > ( _fifo,
//    [] ( AVAudioFifo* p ) { if ( p ) { av_audio_fifo_free ( p ); } } );

//    return std::error_code ();
//}
//std::error_code LibavWrapper::decode_audio_frame ( AVFrame *frame, int *data_present, int *finished ) {

//    int error;

//    /** Packet used for temporary storage. */
//    std::shared_ptr< AVPacket > input_packet =
//    std::shared_ptr< AVPacket > ( new AVPacket(), [] ( AVPacket* p ) { if ( p ) { av_free_packet ( p ); } } );
//    av_init_packet ( input_packet.get() );
//    input_packet->data = NULL;
//    input_packet->size = 0;

//    /** Read one audio frame from the input file into a temporary packet. */
//    if ( ( error = av_read_frame ( input_format_context.get(), input_packet.get() ) ) < 0 ) {
//        /** If we are at the end of the file, flush the decoder below. */
//        if ( error == AVERROR_EOF ) { *finished = 1; }

//        else { return make_error_code ( error ); }
//    }

//    /**
//     * Decode the audio frame stored in the temporary packet.
//     * The input audio stream decoder is used to do this.
//     * If we are at the end of the file, pass an empty packet to the decoder
//     * to flush it.
//     */
//    if ( ( error = avcodec_decode_audio4 ( input_codec_context.get(), frame,
//                                           data_present, input_packet.get() ) ) < 0 ) {
//        return make_error_code ( error );
//    }

//    /**
//     * If the decoder has not been flushed completely, we are not finished,
//     * so that this function has to be called again.
//     */
//    if ( *finished && *data_present )
//    { *finished = 0; }

//    return std::error_code ();
//}
//std::error_code LibavWrapper::add_samples_to_fifo ( AVAudioFifo *fifo,
//        uint8_t **converted_input_samples,
//        const int frame_size ) {
//    int error;

//    if ( ( error = av_audio_fifo_realloc ( fifo, av_audio_fifo_size ( fifo ) + frame_size ) ) < 0 )
//    { return make_error_code ( error ); }

//    if ( av_audio_fifo_write ( fifo, ( void ** ) converted_input_samples, frame_size ) < frame_size )
//    { return make_error_code ( error ); }

//    return std::error_code ();
//}

//std::error_code LibavWrapper::read_decode_convert_and_store ( int *finished ) {

//    int data_present;
//    std::error_code error;

//    /** Initialize temporary storage for one input frame. */
//    std::shared_ptr< AVFrame > input_frame = std::shared_ptr< AVFrame > ( av_frame_alloc(),
//    [] ( AVFrame* p ) { if ( p ) { av_frame_free ( &p ); } } );

//    if ( ! input_frame ) { return std::error_code ( ENOMEM, av_category ); }

//    /** Decode one frame worth of audio samples. */
//    if ( ( error = decode_audio_frame ( input_frame.get(), &data_present, finished ) ) )
//    { return error; }

//    /* If we are at the end of the file and there are no more samples */
//    if ( *finished && !data_present )
//    { return std::error_code (); }

//    /** If there is decoded data, convert and store it */
//    if ( data_present ) {
//        //TODO reuse or resize
//        sample_buffer = std::make_shared< sample_t > ( input_codec_context, output_codec_context, input_frame->nb_samples );

//        if ( sample_buffer->error ) {
//            return sample_buffer->error;
//        }

//        /* convert to destination format */
//        int ret;

//        if ( ( ret = swr_convert ( resample_context.get(),
//                                   sample_buffer->data,
//                                   sample_buffer->dst_nb_samples,
//                                   ( const uint8_t** ) input_frame->extended_data,
//                                   input_frame->nb_samples ) ) < 0 )
//        { return make_error_code ( ret ); }

//        /** Add the converted input samples to the FIFO buffer for later processing. */
//        if ( ( error = add_samples_to_fifo ( fifo.get(), sample_buffer->data, sample_buffer->dst_nb_samples ) ) )
//        { return error; }
//    }

//    return std::error_code();
//}
//std::error_code LibavWrapper::init_output_frame ( AVFrame **frame, int frame_size ) {

//    int error;

//    if ( ! ( *frame = av_frame_alloc() ) )
//    { return make_error_code ( ENOMEM ); }

//    ( *frame )->nb_samples     = frame_size;
//    ( *frame )->channel_layout = output_codec_context->channel_layout;
//    ( *frame )->format         = output_codec_context->sample_fmt;
//    ( *frame )->sample_rate    = output_codec_context->sample_rate;

//    if ( ( error = av_frame_get_buffer ( *frame, 0 ) ) < 0 ) {
//        av_frame_free ( frame ); //TODO
//        return make_error_code ( error );
//    }

//    return std::error_code();
//}

//int64_t pts = 0;
///** Encode one frame worth of audio to the output file. */
//std::error_code LibavWrapper::encode_audio_frame ( AVFrame *frame, int *data_present ) {
//    /** Packet used for temporary storage. */
//    AVPacket output_packet;
//    int error;

//    av_init_packet ( &output_packet );
//    /** Set the packet data and size so that it is recognized as being empty. */
//    output_packet.data = NULL;
//    output_packet.size = 0;

//    /** Set a timestamp based on the sample rate for the container. */
//    if ( frame ) {
//        frame->pts = pts;
//        pts += frame->nb_samples;
//    }

//    /**
//     * Encode the audio frame and store it in the temporary packet.
//     * The output audio stream encoder is used to do this.
//     */
//    if ( ( error = avcodec_encode_audio2 ( output_codec_context, &output_packet,
//                                           frame, data_present ) ) < 0 ) {
//        av_free_packet ( &output_packet );
//        return std::error_code ( error, av_category );
//    }

//    /** Write one audio frame from the temporary packet to the output file. */
//    if ( *data_present ) {
//        if ( ( error = av_write_frame ( output_format_context.get(), &output_packet ) ) < 0 ) {
//            av_free_packet ( &output_packet );
//            return std::error_code ( error, av_category );
//        }

//        av_free_packet ( &output_packet );
//    }

//    return std::error_code ();
//}
//std::error_code LibavWrapper::load_encode_and_write () {
//    /** Temporary storage of the output samples of the frame written to the file. */
//    AVFrame *output_frame;
//    /**
//     * Use the maximum number of possible samples per frame.
//     * If there is less than the maximum possible frame size in the FIFO
//     * buffer use this number. Otherwise, use the maximum possible frame size
//     */
//    const int frame_size = FFMIN ( av_audio_fifo_size ( fifo.get() ),
//                                   output_codec_context->frame_size );
//    int data_written;

//    /** Initialize temporary storage for one output frame. */
//    std::error_code errc;

//    if ( ( errc = init_output_frame ( &output_frame, frame_size ) ) )
//    { return errc; }

//    /**
//     * Read as many samples from the FIFO buffer as required to fill the frame.
//     * The samples are stored in the frame temporarily.
//     */
//    if ( av_audio_fifo_read ( fifo.get(), ( void ** ) output_frame->data, frame_size ) < frame_size ) {
//        av_frame_free ( &output_frame );
//        return make_error_code ( AVERROR_EXIT );
//    }

//    /** Encode one frame worth of audio samples. */
//    if ( ( errc = encode_audio_frame ( output_frame, &data_written ) ) ) {
//        av_frame_free ( &output_frame );
//        return errc;
//    }

//    av_frame_free ( &output_frame );
//    return std::error_code ();
//}
//void LibavWrapper::write_output_metadata ( Metadata& metadata ) {
//    /** Write the header of the output file container. */
//    AVDictionary *d = output_format_context->metadata;

//    for ( auto _tag :metadata.tag_names() ) {
//        av_dict_set ( &d, Metadata::name ( _tag ).c_str(), metadata.get ( _tag ).c_str(), 0 );
//    }

//    av_dict_set ( &d, "duration", NULL, 0 );
//    output_format_context->metadata = d;
//}
//std::error_code LibavWrapper::write_header () {
//    AVDictionary *format_opts = nullptr;

//    //TODO opts
//    av_dict_set ( &format_opts, "id3v2_version", "3", 0 );
//    av_dict_set ( &format_opts, "write_id3v1", "1", 0 );

//    int _error;

//    if ( ( _error = avformat_write_header ( output_format_context.get(), &format_opts ) ) < 0 ) {
//        return make_error_code ( _error );
//    }

//    av_dict_free ( &format_opts );
//    return std::error_code ();
//}
//std::error_code LibavWrapper::write_trailer () {
//    int _error;

//    if ( ( _error = av_write_trailer ( output_format_context.get() ) ) < 0 )
//    { return std::error_code ( _error, av_category ); }

//    return std::error_code ();
//}
//std::error_code LibavWrapper::transcode () {
//    while ( 1 ) {

//        const int output_frame_size = output_codec_context->frame_size;
//        int finished = 0;
//        std::error_code _errc;
//        //read
//        while ( av_audio_fifo_size ( fifo.get() ) < output_frame_size ) {
//            if ( ( _errc = read_decode_convert_and_store ( &finished ) ) )
//            { return _errc; }
//            if ( finished ) { break; }
//        }
//        //write
//        while ( av_audio_fifo_size ( fifo.get() ) >= output_frame_size ||
//                ( finished && av_audio_fifo_size ( fifo.get() ) > 0 ) ) {

//            if ( ( _errc = load_encode_and_write() ) )
//            { return _errc; }
//        }
//        //flush
//        if ( finished ) {
//            int data_written;
//            do {
//                if ( ( _errc = encode_audio_frame ( NULL, &data_written ) ) )
//                { return _errc; }
//            } while ( data_written );

//            break;
//        }
//    }
//    return std::error_code ();
//}
//Metadata LibavWrapper::metadata () {
//    Metadata _metadata;
//    AVDictionaryEntry *tag = NULL;

//    tag = av_dict_get ( av_->input_format_context->metadata, "title", nullptr, AV_DICT_IGNORE_SUFFIX );

//    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

//    tag = av_dict_get ( av_->input_format_context->metadata, "album", nullptr, AV_DICT_IGNORE_SUFFIX );

//    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

//    tag = av_dict_get ( av_->input_format_context->metadata, "artist", nullptr, AV_DICT_IGNORE_SUFFIX );

//    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

//    tag = av_dict_get ( av_->input_format_context->metadata, "track", nullptr, AV_DICT_IGNORE_SUFFIX );

//    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

//    tag = av_dict_get ( av_->input_format_context->metadata, "composer", nullptr, AV_DICT_IGNORE_SUFFIX );

//    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

//    tag = av_dict_get ( av_->input_format_context->metadata, "performer", nullptr, AV_DICT_IGNORE_SUFFIX );

//    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

//    tag = av_dict_get ( av_->input_format_context->metadata, "comment", nullptr, AV_DICT_IGNORE_SUFFIX );

//    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

//    tag = av_dict_get ( av_->input_format_context->metadata, "year", nullptr, AV_DICT_IGNORE_SUFFIX );

//    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

//    tag = av_dict_get ( av_->input_format_context->metadata, "disc", nullptr, AV_DICT_IGNORE_SUFFIX );

//    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

//    tag = av_dict_get ( av_->input_format_context->metadata, "genre", nullptr, AV_DICT_IGNORE_SUFFIX );

//    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

//    tag = av_dict_get ( av_->input_format_context->metadata, "publisher", nullptr, AV_DICT_IGNORE_SUFFIX );

//    if ( tag ) { _metadata.set ( tag->key, tag->value ); }

//    return _metadata;
//}
//}//namespace libav
