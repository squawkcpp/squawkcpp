//#include "audio.h"

//#include <exception>
//#include <iostream>

//#include "fmt/format.h"

///** The output bit rate in kbit/s */
//#define OUTPUT_BIT_RATE 96000
///** The number of output channels */
//#define OUTPUT_CHANNELS 2

//namespace libav {

////inline const char* get_error_text(const int error) {
////    static char error_buffer[255];
////    av_strerror(error, error_buffer, sizeof(error_buffer));
////    return error_buffer;
////}

////inline std::shared_ptr< AVFormatContext > make_format_context( const char* filename ) {
////    AVFormatContext* _fm_ctx = nullptr;
////    int err = 0;
////    if ( ( err = avformat_open_input( &_fm_ctx, filename, nullptr, nullptr ) ) != 0 )
////        throw std::runtime_error ( fmt::format( "Could not open input file '%s' (error '%s')", filename, get_error_text ( err ) ) );

////    if ( ( err = avformat_find_stream_info( _fm_ctx, nullptr ) ) < 0 ) {
////        avformat_close_input( &_fm_ctx );
////        throw std::runtime_error ( fmt::format( "Could not open find stream info (error '%s')", get_error_text ( err ) ) );
////    }
////    return std::shared_ptr< AVFormatContext >( _fm_ctx,
////        [] ( AVFormatContext* p ) { if ( p ) avformat_close_input( &p ); } );
////}

////inline std::shared_ptr< AVFormatContext > make_output_context( const char* filename ) {
////    AVIOContext *output_io_context = nullptr;
////    AVFormatContext* output_format_context = nullptr;
////    /** Open the output file to write to it. */
////    int err = 0;
////    if ( ( err = avio_open ( &output_io_context, filename, AVIO_FLAG_WRITE)) < 0) {
////        throw std::runtime_error ( fmt::format( "Could not open output file '%s' (error '%s')", filename, get_error_text ( err ) ) );
////    }
////    /** Create a new format context for the output container format. */
////    if ( !( output_format_context = avformat_alloc_context() ) ) {
////        throw std::runtime_error ( "Could not allocate output format context" );
////    }
////    /** Associate the output file (pointer) with the container format context. */
////    output_format_context->pb = output_io_context;

////    /** Guess the desired container format based on the file extension. */
////    if ( !( output_format_context->oformat = av_guess_format( NULL, filename, NULL))) {
////        throw std::runtime_error ( "Could not find output file format" );
////    }
////    av_strlcpy( output_format_context->filename, filename, sizeof(output_format_context->filename));
////    return std::shared_ptr< AVFormatContext >( output_format_context,
////        [] ( AVFormatContext* p ) { if ( p ) {
////            avio_closep( & ( p->pb ) );
////            avformat_free_context( p );
////        }});
////}

////inline std::shared_ptr< AVCodecContext > make_codec_context( AVStream* stream ) {
////}

////inline AVCodec* make_output_codec () {
////}

////inline AVStream* make_output_stream ( std::shared_ptr< AVFormatContext > output_format_context, AVCodec* output_codec ) {
////    //TODO no cleanup?
////}

////inline std::shared_ptr< AVCodecContext > make_output_codec_context( AVStream* stream ) {
////    //TODO no cleanup?
////}


//int write_callback(void* stream, uint8_t* buf, int buf_size) {
//    std::string* m_buffer = reinterpret_cast<std::string*>(stream);
//    try {
//        m_buffer->insert(m_buffer->size(),
//            reinterpret_cast<std::string::const_pointer>(buf),
//            buf_size);

//        std::cout << "Write bytes: " << buf_size << std::endl;
//    } catch(...) { // don't want c-code to handle any exceptions
//        std::cout << "Exception while writing in transcoder" << std::endl;
//        return 0;
//    }
//    return buf_size;
//}

//static int IOReadFunc(void *data, uint8_t *buf, int buf_size) {
//    IOContext *hctx = (IOContext*)data;
//    hctx->datafile->read( reinterpret_cast< char* >( buf ), buf_size );
//    return hctx->datafile->gcount();
//}

//// whence: SEEK_SET, SEEK_CUR, SEEK_END (like fseek) and AVSEEK_SIZE
//static int64_t IOSeekFunc(void *data, int64_t pos, int whence) {
//    std::cout << "SEEK " << pos << ":" << whence << std::endl;
////    if (whence == AVSEEK_SIZE) {
////        // return the file size if you wish to
////    }

////    IOContext *hctx = (IOContext*)data;
////    hctx->datafile->seekg( pos, whence );
//////    int rs = fseek(hctx->fh, (long)pos, whence);
////    if (rs != 0) {
////        return -1;
////    }
////    long fpos = ftell(hctx->fh); // int64_t is usually long long
////    return (int64_t)fpos;
//    return -1;
//}

//IOContext::IOContext( std::istream* input ) : datafile ( input ) {
//   // allocate buffer
////    bufferSize = 4096;
//    buffer =  reinterpret_cast<unsigned char*>( av_malloc( bufferSize )); // see destructor for details
//    if ( buffer == NULL) {
//            std::cout << "Out of memory" << std::endl;
//        }

//    // allocate the AVIOContext
//    ioCtx = avio_alloc_context(
//                buffer, bufferSize, // internal buffer and its size
//                0,            // write flag (1=true,0=false)
//                reinterpret_cast<void*>( this ),  // user data, will be passed to our callback functions
//                IOReadFunc,
//                nullptr,            // no writing
//                nullptr
//    );
//    if (ioCtx == NULL) {
//            std::cout << "avio_alloc_context failed. Out of memory?" << std::endl;
//        }
//}

//IOContext::~IOContext() {
//    //TODO if (fh) fclose(fh);

//    // NOTE: ffmpeg messes up the buffer
//    // so free the buffer first then free the context
//    av_free(ioCtx->buffer);
//    ioCtx->buffer = NULL;
//    av_free(ioCtx);
//}

//void IOContext::initAVFormatContext( AVFormatContext *pCtx ) {
//    pCtx->pb = ioCtx;
//    pCtx->flags |= AVFMT_FLAG_CUSTOM_IO;

//    // you can specify a format directly
//    //pCtx->iformat = av_find_input_format("h264");

//    // or read some of the file and let ffmpeg do the guessing
//    datafile->read ( reinterpret_cast< char* >( buffer ), bufferSize );
//    //TODO size_t len = fread(buffer, 1, bufferSize, fh);
//    size_t len = datafile->tellg();
//    if ( len == 0) return;
//    datafile->seekg( 0 ); // reset to beginning of file

//    AVProbeData probeData;
//    probeData.buf = buffer;
//    probeData.buf_size = bufferSize - 1;
//    probeData.filename = "";
//    pCtx->iformat = av_probe_input_format(&probeData, 1);
//}

//Audio::Audio () {
//    /** Register all codecs and formats so that they can be used. */
//    std::cout << "LibAV Version: " << av_version_info() << ", INT:" << avutil_version() << std::endl;
//    std::call_once ( register_flag, av_register_all );
//}

//std::error_code Audio::input ( const std::string& filename ) {
//    /** open input format context */
//    AVFormatContext* _fm_ctx = nullptr;
//    int err = 0;
//    if ( ( err = avformat_open_input( &_fm_ctx, filename.c_str(), nullptr, nullptr ) ) != 0 )
//    { return std::error_code ( err, av_category ); }

//    if ( ( err = avformat_find_stream_info( _fm_ctx, nullptr ) ) < 0 ) {
//        avformat_close_input( &_fm_ctx );
//        return std::error_code ( err, av_category );
//    }
//    input_format_context_ = std::shared_ptr< AVFormatContext >( _fm_ctx,
//        [] ( AVFormatContext* p ) { if ( p ) avformat_close_input( &p ); } );

//    /** Find a decoder for the audio stream. */
//    AVCodec* codec_ = nullptr;
//    if ( !( codec_ = avcodec_find_decoder( input_format_context_->streams[0]->codec->codec_id ) ) ) {
//        throw std::runtime_error ( "Could not find input codec" ); //TODO
//    }

//    //open the codec
//    input_codec_context = std::shared_ptr< AVCodecContext >( input_format_context_->streams[0]->codec,
//        [] ( AVCodecContext* p ) { if ( p ) avcodec_close( p ); } );

//    return std::error_code();
//}

//std::error_code Audio::input ( std::istream* istream ) {
//    io_context = std::make_shared< IOContext >( istream );
//    AVFormatContext* _fm_ctx = avformat_alloc_context();
//    io_context->initAVFormatContext( _fm_ctx );

//    int err = 0;
//    if ( ( err = avformat_open_input( &_fm_ctx, "", nullptr, nullptr ) ) != 0 )
//    { return std::error_code ( err, av_category ); }

//    if ( ( err = avformat_find_stream_info( _fm_ctx, nullptr ) ) < 0 ) {
//        avformat_close_input( &_fm_ctx );
//        throw std::runtime_error ( fmt::format( "Could not open find stream info (error '{}')", get_error_text ( err ) ) );
//    }
//    input_format_context_ = std::shared_ptr< AVFormatContext >( _fm_ctx,
//        [] ( AVFormatContext* p ) { if ( p ) avformat_close_input( &p ); } );


//    AVCodec* codec = nullptr;
//    if ( !( codec = avcodec_find_decoder(input_format_context_->streams[0]->codec->codec_id ) ) ) {
//        throw std::runtime_error ( "Could not find input codec" );
//    }
//    if ( ( err = avcodec_open2( input_format_context_->streams[0]->codec, codec, NULL ) ) < 0 )
//        throw std::runtime_error ( fmt::format( "Could not open input codec (error '%s')", get_error_text ( err ) ) );
//    input_codec_context = std::shared_ptr< AVCodecContext >( input_format_context_->streams[0]->codec,
//        [] ( AVCodecContext* p ) { if ( p ) avcodec_close( p ); } );
//}

//MediaInfo Audio::media_info ( const int index ) {
//    Metadata _metadata;
//    AVDictionaryEntry *tag = NULL;
//    while ( ( tag = av_dict_get( input_format_context_->metadata, "", tag, AV_DICT_IGNORE_SUFFIX ) ) ) {
//        _metadata.set( tag->key, tag->value );
//    }
//    AVCodecContext* codec = input_format_context_->streams[index]->codec;
//    return (
//        MediaInfo {
//            MediaType::get( codec->codec_type ),
//            Codec::get( codec->codec_id ),
//            codec->bit_rate,
//            codec->sample_rate,
//            codec->channels,
//            input_format_context_->streams[index]->duration,
//            codec->bits_per_coded_sample,
//            codec->width,
//            codec->height,
//            _metadata
//        }
//    );
//}

//void Audio::transcode( const char* output, MediaInfo media_info ) {

//    open_output_file( output, media_info );

//    SwrContext *resample_context = NULL;
//    AVAudioFifo *fifo = NULL;

//    /** Initialize the resampler to be able to convert audio sample formats. */
//    if (init_resampler( &resample_context ) )
//        goto cleanup;
//    /** Initialize the FIFO buffer to store audio samples to be encoded. */
//    if (init_fifo(&fifo ) )
//        goto cleanup;
//    /** Write the header of the output file container. */
//    if ( write_output_file_header( ) )
//        goto cleanup;

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
//        while (av_audio_fifo_size(fifo) < output_frame_size) {
//            /**
//             * Decode one frame worth of audio samples, convert it to the
//             * output sample format and put it into the FIFO buffer.
//             */
//            if (read_decode_convert_and_store(fifo, resample_context, &finished))
//                goto cleanup;

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
//        while (av_audio_fifo_size(fifo) >= output_frame_size ||
//               (finished && av_audio_fifo_size(fifo) > 0))
//            /**
//             * Take one frame worth of audio samples from the FIFO buffer,
//             * encode it and write it to the output file.
//             */
//            if (load_encode_and_write(fifo) )
//                goto cleanup;

//        /**
//         * If we are at the end of the input file and have encoded
//         * all remaining samples, we can exit this loop and finish.
//         */
//        if (finished) {
//            int data_written;
//            /** Flush the encoder as it may have delayed frames. */
//            do {
//                if (encode_audio_frame(NULL, &data_written))
//                    goto cleanup;
//            } while (data_written);
//            break;
//        }
//    }

//    /** Write the trailer of the output file container. */
//    if (write_output_file_trailer() )
//        goto cleanup;

//cleanup:
//    if (fifo)
//        av_audio_fifo_free(fifo);
//    swr_free(&resample_context);
//}


///**
// * Open an output file and the required encoder.
// * Also set some basic encoder parameters.
// * Some of these parameters are based on the input file's parameters.
// */
//int Audio::open_output_file( const char *filename, MediaInfo media_info ) {

//    AVIOContext *output_io_context = nullptr;
//    AVFormatContext* _output_format_context = nullptr;
//    /** Open the output file to write to it. */
//    int err = 0;
//    if ( ( err = avio_open ( &output_io_context, filename, AVIO_FLAG_WRITE)) < 0) {
//        throw std::runtime_error ( fmt::format( "Could not open output file '%s' (error '%s')", filename, get_error_text ( err ) ) );
//    }
//    /** Create a new format context for the output container format. */
//    if ( !( _output_format_context = avformat_alloc_context() ) ) {
//        throw std::runtime_error ( "Could not allocate output format context" );
//    }
//    /** Associate the output file (pointer) with the container format context. */
//    _output_format_context->pb = output_io_context;

//    /** Guess the desired container format based on the file extension. */
//    if ( !( _output_format_context->oformat = av_guess_format( NULL, filename, NULL))) {
//        throw std::runtime_error ( "Could not find output file format" );
//    }
//    av_strlcpy( _output_format_context->filename, filename, sizeof(_output_format_context->filename));

//    output_format_context = std::shared_ptr< AVFormatContext >( _output_format_context,
//        [] ( AVFormatContext* p ) { if ( p ) {
//            avio_closep( & ( p->pb ) );
//            avformat_free_context( p );
//        }});

//    //find encoder
//    AVCodec *output_codec          = nullptr;
//    /** Find the encoder to be used by its name. */
//    if ( !( output_codec = avcodec_find_encoder( Codec::get( media_info.codec ) ) ) ) {
//        throw std::runtime_error ( "Could not find an AAC encoder." );
//    }

//    //make output stream
//    AVStream *stream = nullptr;
//    /** Create a new audio stream in the output file container. */
//    if (!(stream = avformat_new_stream( output_format_context.get(), output_codec ) ) ) {
//        throw std::runtime_error ( "Could not create new stream." );
//    }

////    AVCodecContext *avctx          = NULL;
////    avctx = avcodec_alloc_context3(output_codec);
////       if (!avctx) {
////           fprintf(stderr, "Could not allocate an encoding context\n");
//////           error = AVERROR(ENOMEM);
//////           goto cleanup;
////       }

//    /** Save the encoder context for easier access later. */
//     AVCodecContext *avctx = avcodec_alloc_context3(output_codec);
//        if (!avctx) {
//            fprintf(stderr, "Could not allocate an encoding context\n");
//            //error = AVERROR(ENOMEM);
//            //goto cleanup;
//    }
//    output_codec_context = std::shared_ptr< AVCodecContext >( /*avctx*/ stream->codec,
//        [] ( AVCodecContext* p ) { if ( p ) {std::cout << "clode output codec " << std::endl; avcodec_close( p );  } } );

//    /**
//     * Set the basic encoder parameters.
//     * The input file's sample rate is used to avoid a sample rate conversion.
//     */
//    output_codec_context->channels       = media_info.channels; //OUTPUT_CHANNELS;
//    output_codec_context->channel_layout = av_get_default_channel_layout( media_info.channels ); //OUTPUT_CHANNELS);
//    output_codec_context->sample_rate    = input_codec_context->sample_rate; //TODO stream number
//    output_codec_context->sample_fmt     = output_codec->sample_fmts[0];
//    output_codec_context->bit_rate       = media_info.bitrate; //OUTPUT_BIT_RATE;
//    /** Set the sample rate for the container. */
//    stream->time_base.den = input_codec_context->sample_rate;
//    stream->time_base.num = 1;
//    /** Allow the use of the experimental AAC encoder */
//    output_codec_context->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
//    /* Some container formats (like MP4) require global headers to be present
//       Mark the encoder so that it behaves accordingly. */
//    if ( output_format_context->oformat->flags & AVFMT_GLOBALHEADER)
//        output_codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

//    /** Open the encoder for the audio stream to use it later. */
//    int error;
//    if ((error = avcodec_open2( output_codec_context.get(), output_codec, NULL)) < 0) {
//        fprintf(stderr, "Could not open output codec (error '%s')\n",
//                get_error_text(error));
//        goto cleanup;
//    }
////    error = avcodec_parameters_from_context(stream->codec, output_codec_context);
////       if (error < 0) {
////           fprintf(stderr, "Could not initialize stream parameters\n");
////           goto cleanup;
////       }
//    return 0;

//cleanup:
//    avio_closep( &output_format_context->pb);
////    avformat_free_context( output_format_context);
//    output_format_context = NULL;
//    return error < 0 ? error : AVERROR_EXIT;
//}

///**
// * Convert an error code into a text message.
// * @param error Error code to be converted
// * @return Corresponding error text (not thread-safe)
// */
//const char* Audio::get_error_text(const int error)
//{
//    static char error_buffer[255];
//    av_strerror(error, error_buffer, sizeof(error_buffer));
//    return error_buffer;
//}


///** Initialize one data packet for reading or writing. */
//void Audio::init_packet(AVPacket *packet)
//{
//    av_init_packet(packet);
//    /** Set the packet data and size so that it is recognized as being empty. */
//    packet->data = NULL;
//    packet->size = 0;
//}

///** Initialize one audio frame for reading from the input file */
//int Audio::init_input_frame(AVFrame **frame)
//{
//    if (!(*frame = av_frame_alloc())) {
//        fprintf(stderr, "Could not allocate input frame\n");
//        return AVERROR(ENOMEM);
//    }
//    return 0;
//}

///**
// * Initialize the audio resampler based on the input and output codec settings.
// * If the input and output sample formats differ, a conversion is required
// * libswresample takes care of this, but requires initialization.
// */
//int Audio::init_resampler(SwrContext **resample_context)
//{
//    return 0;
//}

///** Initialize a FIFO buffer for the audio samples to be encoded. */
//int Audio::init_fifo(AVAudioFifo **fifo )
//{
//    /** Create the FIFO buffer based on the specified output sample format. */
//    if (!(*fifo = av_audio_fifo_alloc(output_codec_context->sample_fmt,
//                                      output_codec_context->channels, 1))) {
//        fprintf(stderr, "Could not allocate FIFO\n");
//        return AVERROR(ENOMEM);
//    }
//    return 0;
//}

///** Write the header of the output file container. */
//int Audio::write_output_file_header() {
//    int error;
//    if ((error = avformat_write_header(output_format_context.get(), NULL)) < 0) {
//        fprintf(stderr, "Could not write output file header (error '%s')\n", get_error_text(error));
//        return error;
//    }
//    return 0;
//}

///** Decode one audio frame from the input file. */
//int Audio::decode_audio_frame(AVFrame *frame,
//                              int *data_present, int *finished)
//{
//    /** Packet used for temporary storage. */
//    AVPacket input_packet;
//    int error;
//    init_packet(&input_packet);

//    /** Read one audio frame from the input file into a temporary packet. */
//    if ((error = av_read_frame(input_format_context_.get(), &input_packet)) < 0) {
//        /** If we are at the end of the file, flush the decoder below. */
//        if (error == AVERROR_EOF)
//            *finished = 1;
//        else {
//            fprintf(stderr, "Could not read frame (error '%s')\n",
//                    get_error_text(error));
//            return error;
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
//        fprintf(stderr, "Could not decode frame (error '%s')\n",
//                get_error_text(error));
//        av_free_packet(&input_packet);
//        return error;
//    }

//    /**
//     * If the decoder has not been flushed completely, we are not finished,
//     * so that this function has to be called again.
//     */
//    if (*finished && *data_present)
//        *finished = 0;
//    av_free_packet(&input_packet);
//    return 0;
//}

///**
// * Initialize a temporary storage for the specified number of audio samples.
// * The conversion requires temporary storage due to the different format.
// * The number of audio samples to be allocated is specified in frame_size.
// */
//int Audio::init_converted_samples(uint8_t ***converted_input_samples, int frame_size)
//{
//    int error;

//    /**
//     * Allocate as many pointers as there are audio channels.
//     * Each pointer will later point to the audio samples of the corresponding
//     * channels (although it may be NULL for interleaved formats).
//     */
//    if (!(*converted_input_samples = (uint8_t**)calloc(output_codec_context->channels,
//                                            sizeof(**converted_input_samples)))) {
//        fprintf(stderr, "Could not allocate converted input sample pointers\n");
//        return AVERROR(ENOMEM);
//    }

//    /**
//     * Allocate memory for the samples of all channels in one consecutive
//     * block for convenience.
//     */
//    if ((error = av_samples_alloc(*converted_input_samples, NULL,
//                                  output_codec_context->channels,
//                                  frame_size,
//                                  output_codec_context->sample_fmt, 0)) < 0) {
//        fprintf(stderr,
//                "Could not allocate converted input samples (error '%s')\n",
//                get_error_text(error));
//        av_freep(&(*converted_input_samples)[0]);
//        free(*converted_input_samples);
//        return error;
//    }
//    return 0;
//}

///**
// * Convert the input audio samples into the output sample format.
// * The conversion happens on a per-frame basis, the size of which is specified
// * by frame_size.
// */
//int Audio::convert_samples(const uint8_t **input_data,
//                           uint8_t **converted_data, const int frame_size,
//                           SwrContext *resample_context)
//{
//    int error;

//    /** Convert the samples using the resampler. */
//    if ((error = swr_convert(resample_context,
//                             converted_data, frame_size,
//                             input_data    , frame_size)) < 0) {
//        fprintf(stderr, "Could not convert input samples (error '%s')\n",
//                get_error_text(error));
//        return error;
//    }

//    return 0;
//}

///** Add converted input audio samples to the FIFO buffer for later processing. */
//int Audio::add_samples_to_fifo(AVAudioFifo *fifo,
//                               uint8_t **converted_input_samples,
//                               const int frame_size)
//{
//    int error;

//    /**
//     * Make the FIFO as large as it needs to be to hold both,
//     * the old and the new samples.
//     */
//    if ((error = av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + frame_size)) < 0) {
//        fprintf(stderr, "Could not reallocate FIFO\n");
//        return error;
//    }

//    /** Store the new samples in the FIFO buffer. */
//    if (av_audio_fifo_write(fifo, (void **)converted_input_samples,
//                            frame_size) < frame_size) {
//        fprintf(stderr, "Could not write data to FIFO\n");
//        return AVERROR_EXIT;
//    }
//    return 0;
//}

///**
// * Read one audio frame from the input file, decodes, converts and stores
// * it in the FIFO buffer.
// */
//int Audio::read_decode_convert_and_store(AVAudioFifo *fifo,
//                                         SwrContext *resampler_context,
//                                         int *finished)
//{
//    /** Temporary storage of the input samples of the frame read from the file. */
//    AVFrame *input_frame = NULL;
//    /** Temporary storage for the converted input samples. */
//    uint8_t **converted_input_samples = NULL;
//    int data_present;
//    int ret = AVERROR_EXIT;

//    /** Initialize temporary storage for one input frame. */
//    if (init_input_frame(&input_frame))
//        goto cleanup;
//    /** Decode one frame worth of audio samples. */
//    if (decode_audio_frame(input_frame, &data_present, finished))
//        goto cleanup;
//    /**
//     * If we are at the end of the file and there are no more samples
//     * in the decoder which are delayed, we are actually finished.
//     * This must not be treated as an error.
//     */
//    if (*finished && !data_present) {
//        ret = 0;
//        goto cleanup;
//    }
//    /** If there is decoded data, convert and store it */
//    if (data_present) {
//        /** Initialize the temporary storage for the converted input samples. */
//        if (init_converted_samples(&converted_input_samples, input_frame->nb_samples))
//            goto cleanup;

//        /**
//         * Convert the input samples to the desired output sample format.
//         * This requires a temporary storage provided by converted_input_samples.
//         */
//        if (convert_samples((const uint8_t**)input_frame->extended_data, converted_input_samples,
//                            input_frame->nb_samples, resampler_context))
//            goto cleanup;

//        /** Add the converted input samples to the FIFO buffer for later processing. */
//        if (add_samples_to_fifo(fifo, converted_input_samples,
//                                input_frame->nb_samples))
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

//    return ret;
//}

///**
// * Initialize one input frame for writing to the output file.
// * The frame will be exactly frame_size samples large.
// */
//int Audio::init_output_frame(AVFrame **frame,
//                             int frame_size)
//{
//    int error;

//    /** Create a new frame to store the audio samples. */
//    if (!(*frame = av_frame_alloc())) {
//        fprintf(stderr, "Could not allocate output frame\n");
//        return AVERROR_EXIT;
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
//        fprintf(stderr, "Could allocate output frame samples (error '%s')\n",
//                get_error_text(error));
//        av_frame_free(frame);
//        return error;
//    }

//    return 0;
//}

///** Global timestamp for the audio frames */
//static int64_t pts = 0;

///** Encode one frame worth of audio to the output file. */
//int Audio::encode_audio_frame(AVFrame *frame, int *data_present)
//{
//    /** Packet used for temporary storage. */
//    AVPacket output_packet;
//    int error;
//    init_packet(&output_packet);

//    /** Set a timestamp based on the sample rate for the container. */
//    if (frame) {
//        frame->pts = pts;
//        pts += frame->nb_samples;
//    }

//    /**
//     * Encode the audio frame and store it in the temporary packet.
//     * The output audio stream encoder is used to do this.
//     */
//    if ((error = avcodec_encode_audio2( output_codec_context.get(), &output_packet,
//                                       frame, data_present)) < 0) {
//        fprintf(stderr, "Could not encode frame (error '%s')\n",
//                get_error_text(error));
//        av_free_packet(&output_packet);
//        return error;
//    }

//    /** Write one audio frame from the temporary packet to the output file. */
//    if (*data_present) {
//        if ((error = av_write_frame(output_format_context.get(), &output_packet)) < 0) {
//            fprintf(stderr, "Could not write frame (error '%s')\n",
//                    get_error_text(error));
//            av_free_packet(&output_packet);
//            return error;
//        }

//        av_free_packet(&output_packet);
//    }

//    return 0;
//}

///**
// * Load one audio frame from the FIFO buffer, encode and write it to the
// * output file.
// */
//int Audio::load_encode_and_write( AVAudioFifo *fifo )
//{
//    /** Temporary storage of the output samples of the frame written to the file. */
//    AVFrame *output_frame;
//    /**
//     * Use the maximum number of possible samples per frame.
//     * If there is less than the maximum possible frame size in the FIFO
//     * buffer use this number. Otherwise, use the maximum possible frame size
//     */
//    const int frame_size = FFMIN(av_audio_fifo_size(fifo),
//                                 output_codec_context->frame_size);
//    int data_written;

//    /** Initialize temporary storage for one output frame. */
//    if (init_output_frame(&output_frame, frame_size))
//        return AVERROR_EXIT;

//    /**
//     * Read as many samples from the FIFO buffer as required to fill the frame.
//     * The samples are stored in the frame temporarily.
//     */
//    if (av_audio_fifo_read(fifo, (void **)output_frame->data, frame_size) < frame_size) {
//        fprintf(stderr, "Could not read data from FIFO\n");
//        av_frame_free(&output_frame);
//        return AVERROR_EXIT;
//    }

//    /** Encode one frame worth of audio samples. */
//    if (encode_audio_frame(output_frame, &data_written)) {
//        av_frame_free(&output_frame);
//        return AVERROR_EXIT;
//    }
//    av_frame_free(&output_frame);
//    return 0;
//}

///** Write the trailer of the output file container. */
//int Audio::write_output_file_trailer() {
//    int error;
//    if ((error = av_write_trailer(output_format_context.get() ) ) < 0 ) {
//        fprintf(stderr, "Could not write output file trailer (error '%s')\n",
//                get_error_text(error));
//        return error;
//    }
//    return 0;
//}
//}//namespace libav
