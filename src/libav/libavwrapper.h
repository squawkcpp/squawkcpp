//#ifndef LIBAVWRAPPER_H
//#define LIBAVWRAPPER_H

//#include <memory>
//#include <mutex>
//#include <iostream> //TODO remove cout

//extern "C" {
//#include "libavformat/avformat.h"
//#include "libavformat/avio.h"

//#include "libavcodec/avcodec.h"

//#include "libavutil/audio_fifo.h"
//#include "libavutil/avassert.h"
//#include "libavutil/avstring.h"
//#include "libavutil/frame.h"
//#include "libavutil/opt.h"
//#include "libswresample/swresample.h"

//#ifdef __cplusplus
//#define __STDC_CONSTANT_MACROS
//#ifdef _STDINT_H
//#undef _STDINT_H
//#endif
//# include <stdint.h>
//#endif
//}

//#include "averrc.h"
//#include "codec.h"
//#include "mediainfo.h"
//#include "libaviocontext.h"

//namespace libav {
//class LibavWrapper {
//public:
//    LibavWrapper();
//    ~LibavWrapper() {}

//    /**
//     * @brief  input format context from filename.
//     * @param filename the filename
//     * @return error code
//     */
//    std::error_code make_input_format_context ( const char* filename );
//    /**
//     * @brief make input format context from stream.
//     * @param istream the input stream
//     * @return error code
//     */
//    std::error_code make_input_format_context ( std::istream* istream );
//    std::error_code make_output_format_context ( const char* filename );
//    std::error_code make_output_format_context ( Codec::Enum codec );
//    std::error_code make_input_codec_context ( const int stream_number );
//    std::error_code make_output_codec_context ( const MediaInfo& media_info );

//    AVCodecID av_codec_id ( Codec::Enum codec ) {
//        switch ( codec ) {
//        case Codec::AAC:
//            return AV_CODEC_ID_AAC;

//        case Codec::FLAC:
//            return AV_CODEC_ID_FLAC;

//        case Codec::MP3:
//            return AV_CODEC_ID_MP3;

//        case Codec::VORBIS:
//            return AV_CODEC_ID_VORBIS;

//        default:
//            return AV_CODEC_ID_NONE;
//        }
//    }

//    /**
//     * \brief Initialize the audio resampler based on the input and output codec settings.
//     * If the input and output sample formats differ, a conversion is required
//     * libswresample takes care of this, but requires initialization.
//     */
//    std::error_code make_resampler_context();
//    std::error_code make_fifo();
//    /**
//     * @brief Decode one audio frame from the input file.
//     * @param frame The AVFrame pointer
//     * @param data_present pointer for data present.
//     * @param finished pointer
//     * @return error code
//     */
//    std::error_code decode_audio_frame ( AVFrame *frame, int *data_present, int *finished );
//    /**
//     * @brief Add converted input audio samples to the FIFO buffer for later processing.
//     * @param fifo AVAudioFifo pointer
//     * @param converted_input_samples pointer to converted samples.
//     * @param frame_size frame size
//     * @return  error code
//     */
//    std::error_code add_samples_to_fifo ( AVAudioFifo *fifo,
//                                          uint8_t **converted_input_samples,
//                                          const int frame_size );
//    /**
//     * @brief Read one audio frame from the input file, decodes, converts and stores it in the FIFO buffer.
//     * @param finished finished result pointer
//     * @return error code
//     */
//    std::error_code read_decode_convert_and_store ( int *finished );
//    /**
//     * @brief Initialize one input frame for writing to the output file.
//     * The frame will be exactly frame_size samples large.
//     * @param frame AVFrame pointer
//     * @param frame_size frame size
//     * @return error code
//     */
//    std::error_code init_output_frame ( AVFrame **frame, int frame_size );
//    /**
//     * @brief Encode one frame worth of audio to the output file.
//     * @param frame AVFrame pointer
//     * @param data_present result pointer for data present.
//     * @return
//     */
//    std::error_code encode_audio_frame ( AVFrame *frame, int *data_present );
//    /**
//     * @brief Load one audio frame from the FIFO buffer, encode and write it to the output file.
//     * @return error code
//     */
//    std::error_code load_encode_and_write ();
//    /**
//     * @brief Write output metadata.
//     * @param metadata the metadata.
//     */
//    void write_output_metadata ( Metadata& metadata );
//    /**
//     * @brief write output header
//     * @return error code
//     */
//    std::error_code write_header ();
//    /**
//     * @brief Write the trailer of the output file container.
//     * @return error code;
//     */
//    std::error_code write_trailer ();
//    /**
//     * @brief copy the streams.
//     * @return error code
//     */
//    std::error_code transcode ();
//    Metadata metadata ();

//    struct sample_t {
//        sample_t ( auto source_codec_context, auto dest_codec_context, int sample_size ) {
//            dst_nb_samples =
//                av_rescale_rnd ( sample_size, dest_codec_context->sample_rate, source_codec_context->sample_rate, AV_ROUND_UP );

//            /* Allocate memory for the samples of all channels */
//            int _errc;

//            if ( ( _errc  = av_samples_alloc_array_and_samples ( &data, nullptr, dest_codec_context->channels,
//                            dst_nb_samples, dest_codec_context->sample_fmt, 0 ) ) ) {
//                error = make_error_code ( _errc );
//            }

//            error = std::error_code();
//        }
//        ~sample_t() {
//            if ( data )
//            { av_freep ( &data[0] ); }

//            av_freep ( &data );
//        }
//        int dst_nb_samples;
//        std::error_code error;
//        uint8_t** data;
//    };

//    std::shared_ptr< LibavIoContext > output_io_context;

//private:
//    int64_t pts = 0;
//    static std::once_flag _register_flag;
//    std::shared_ptr< LibavIoContext > input_io_context;
//    std::shared_ptr < AVCodecContext > input_codec_context;
//    std::shared_ptr < SwrContext > resample_context;
//    std::shared_ptr< sample_t > sample_buffer;

//    std::shared_ptr < AVFormatContext > input_format_context;
//    std::shared_ptr < AVFormatContext > output_format_context;
//    AVCodecContext* output_codec_context;
//    std::shared_ptr < AVAudioFifo > fifo;
//};
//}//namespace libav
//#endif // LIBAVWRAPPER_H
