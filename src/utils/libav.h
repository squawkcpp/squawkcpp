//#ifndef LIBAV_H
//#define LIBAV_H

//#include <cstdlib>
////#include <cstdarg>
//#include <algorithm>
//#include <iostream>
//#include <map>
//#include <memory>
//#include <mutex>
//#include <vector>
//#include <string>
//#include <system_error>
////#include <thread>

////#include <boost/iostreams/categories.hpp>
////#include <boost/iostreams/stream_buffer.hpp>
////#include <boost/lexical_cast.hpp>
////#include <boost/shared_ptr.hpp>

//#include "fmt/format.h"

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
// #define __STDC_CONSTANT_MACROS
// #ifdef _STDINT_H
//  #undef _STDINT_H
// #endif
// # include <stdint.h>
//#endif
//}

//namespace libav {
//enum av_errc {
//UNKNOWN,
//AV_BSF_NOT_FOUND       = FFERRTAG(0xF8,'B','S','F'), ///< Bitstream filter not found
//AV_BUG                 = FFERRTAG( 'B','U','G','!'), ///< Internal bug, also see AVERROR_BUG2
//AV_BUFFER_TOO_SMALL    = FFERRTAG( 'B','U','F','S'), ///< Buffer too small
//AV_DECODER_NOT_FOUND   = FFERRTAG(0xF8,'D','E','C'), ///< Decoder not found
//AV_DEMUXER_NOT_FOUND   = FFERRTAG(0xF8,'D','E','M'), ///< Demuxer not found
//AV_ENCODER_NOT_FOUND   = FFERRTAG(0xF8,'E','N','C'), ///< Encoder not found
//AV_EOF                 = FFERRTAG( 'E','O','F',' '), ///< End of file
//AV_EXIT                = FFERRTAG( 'E','X','I','T'), ///< Immediate exit was requested; the called function should not be restarted
//AV_EXTERNAL            = FFERRTAG( 'E','X','T',' '), ///< Generic error in an external library
//AV_FILTER_NOT_FOUND    = FFERRTAG(0xF8,'F','I','L'), ///< Filter not found
//AV_INVALIDDATA         = FFERRTAG( 'I','N','D','A'), ///< Invalid data found when processing input
//AV_MUXER_NOT_FOUND     = FFERRTAG(0xF8,'M','U','X'), ///< Muxer not found
//AV_OPTION_NOT_FOUND    = FFERRTAG(0xF8,'O','P','T'), ///< Option not found
//AV_PATCHWELCOME        = FFERRTAG( 'P','A','W','E'), ///< Not yet implemented in FFmpeg, patches welcome
//AV_PROTOCOL_NOT_FOUND  = FFERRTAG(0xF8,'P','R','O'), ///< Protocol not found
//AV_STREAM_NOT_FOUND    = FFERRTAG(0xF8,'S','T','R'), ///< Stream not found
//AV_BUG2                = FFERRTAG( 'B','U','G',' '),
//AV_UNKNOWN             = FFERRTAG( 'U','N','K','N'), ///< Unknown error, typically from an external library
//AV_EXPERIMENTAL        = (-0x2bb2afa8), ///< Requested feature is flagged experimental. Set strict_std_compliance if you really want to use it.
//AV_INPUT_CHANGED       = (-0x636e6701), ///< Input changed between calls. Reconfiguration is required. (can be OR-ed with AVERROR_OUTPUT_CHANGED)
//AV_OUTPUT_CHANGED      = (-0x636e6702), ///< Output changed between calls. Reconfiguration is required. (can be OR-ed with AVERROR_INPUT_CHANGED)
///* HTTP & RTSP errors */
//AV_HTTP_BAD_REQUEST    = FFERRTAG(0xF8,'4','0','0'),
//AV_HTTP_UNAUTHORIZED   = FFERRTAG(0xF8,'4','0','1'),
//AV_HTTP_FORBIDDEN      = FFERRTAG(0xF8,'4','0','3'),
//AV_HTTP_NOT_FOUND      = FFERRTAG(0xF8,'4','0','4'),
//AV_HTTP_OTHER_4XX      = FFERRTAG(0xF8,'4','X','X'),
//AV_HTTP_SERVER_ERROR   = FFERRTAG(0xF8,'5','X','X')
//};
//}

//namespace std {
//template<> struct is_error_condition_enum<libav::av_errc> : public true_type {};
//}

//namespace libav {
//class av_category_t : public std::error_category {
//public:
//  virtual const char* name() const noexcept { return "av"; }
//  virtual std::error_condition default_error_condition ( int ev ) const noexcept {
//        if ( ev == static_cast< int >( AV_BSF_NOT_FOUND ) )
//        { return std::error_condition( AV_BSF_NOT_FOUND ); }
//        else if ( ev == static_cast< int >( AV_BUG ) )
//        { return std::error_condition( AV_BUG ); }
//        else if ( ev == static_cast< int >( AV_BUFFER_TOO_SMALL ) )
//        { return std::error_condition( AV_BUFFER_TOO_SMALL ); }
//        else if ( ev == static_cast< int >( AV_DECODER_NOT_FOUND ) )
//        { return std::error_condition( AV_DECODER_NOT_FOUND ); }
//        else if ( ev == static_cast< int >( AV_DEMUXER_NOT_FOUND ) )
//        { return std::error_condition( AV_DEMUXER_NOT_FOUND ); }
//        else if ( ev == static_cast< int >( AV_ENCODER_NOT_FOUND ) )
//        { return std::error_condition( AV_ENCODER_NOT_FOUND ); }
//        else if ( ev == static_cast< int >( AV_EXIT ) )
//        { return std::error_condition( AV_EXIT ); }
//        else if ( ev == static_cast< int >( AV_EXTERNAL ) )
//        { return std::error_condition( AV_EXTERNAL ); }
//        else if ( ev == static_cast< int >( AV_FILTER_NOT_FOUND ) )
//        { return std::error_condition( AV_FILTER_NOT_FOUND ); }
//        else if ( ev == static_cast< int >( AV_INVALIDDATA ) )
//        { return std::error_condition( AV_INVALIDDATA ); }
//        else if ( ev == static_cast< int >( AV_MUXER_NOT_FOUND ) )
//        { return std::error_condition( AV_MUXER_NOT_FOUND ); }
//        else if ( ev == static_cast< int >( AV_OPTION_NOT_FOUND ) )
//        { return std::error_condition( AV_OPTION_NOT_FOUND ); }
//        else if ( ev == static_cast< int >( AV_PATCHWELCOME ) )
//        { return std::error_condition( AV_PATCHWELCOME ); }
//        else if ( ev == static_cast< int >( AV_PROTOCOL_NOT_FOUND ) )
//        { return std::error_condition( AV_PROTOCOL_NOT_FOUND ); }
//        else if ( ev == static_cast< int >( AV_STREAM_NOT_FOUND ) )
//        { return std::error_condition( AV_STREAM_NOT_FOUND ); }
//        else if ( ev == static_cast< int >( AV_UNKNOWN ) )
//        { return std::error_condition( AV_UNKNOWN ); }
//        else if ( ev == static_cast< int >( AV_EXPERIMENTAL ) )
//        { return std::error_condition( AV_EXPERIMENTAL ); }
//        else if ( ev == static_cast< int >( AV_INPUT_CHANGED ) )
//        { return std::error_condition( AV_INPUT_CHANGED ); }
//        else if ( ev == static_cast< int >( AV_OUTPUT_CHANGED ) )
//        { return std::error_condition( AV_OUTPUT_CHANGED ); }

//        else if ( ev == static_cast< int >( AV_HTTP_BAD_REQUEST ) )
//        { return std::error_condition( AV_HTTP_BAD_REQUEST ); }
//        else if ( ev == static_cast< int >( AV_HTTP_UNAUTHORIZED ) )
//        { return std::error_condition( AV_HTTP_UNAUTHORIZED ); }
//        else if ( ev == static_cast< int >( AV_HTTP_FORBIDDEN ) )
//        { return std::error_condition( AV_HTTP_FORBIDDEN ); }
//        else if ( ev == static_cast< int >( AV_HTTP_NOT_FOUND ) )
//        { return std::error_condition( AV_HTTP_NOT_FOUND ); }
//        else if ( ev == static_cast< int >( AV_HTTP_OTHER_4XX ) )
//        { return std::error_condition( AV_HTTP_OTHER_4XX ); }
//        else if ( ev == static_cast< int >( AV_HTTP_SERVER_ERROR ) )
//        { return std::error_condition( AV_HTTP_SERVER_ERROR ); }

//        else { return std::error_condition( av_errc::UNKNOWN ); }
//  }
//  virtual bool equivalent ( const std::error_code& code, int condition ) const noexcept {
//    return *this==code.category() &&
//           static_cast< int >( default_error_condition( code.value()).value() ) == condition;
//  }
//  virtual std::string message(int ev) const {
//    if ( ev == static_cast< int >( AV_HTTP_SERVER_ERROR ) )
//        return "Unkown Error.";

//    std::cout << "get error message " << ev << std::endl;
//    static char error_buffer[255];
//    av_strerror( ev, error_buffer, sizeof( error_buffer ) );
//    return error_buffer;
//  }
//} static av_category;

//    // make_error_code overload to generate custom conditions:
//static std::error_condition make_error_condition (av_errc e) {
//    return std::error_condition(static_cast<int>(e), av_category);
//}

//static std::error_code make_error_code ( int error ) {
//    switch( error ) {
//    case UNKNOWN:
//    case AV_BSF_NOT_FOUND:
//    case AV_BUG:
//    case AV_BUFFER_TOO_SMALL:
//    case AV_DECODER_NOT_FOUND:
//    case AV_DEMUXER_NOT_FOUND:
//    case AV_ENCODER_NOT_FOUND:
//    case AV_EOF:
//    case AV_EXIT:
//    case AV_EXTERNAL:
//    case AV_FILTER_NOT_FOUND:
//    case AV_INVALIDDATA:
//    case AV_MUXER_NOT_FOUND:
//    case AV_OPTION_NOT_FOUND:
//    case AV_PATCHWELCOME:
//    case AV_PROTOCOL_NOT_FOUND:
//    case AV_STREAM_NOT_FOUND:
//    case AV_BUG2:
//    case AV_UNKNOWN:
//    case AV_EXPERIMENTAL:
//    case AV_INPUT_CHANGED:
//    case AV_OUTPUT_CHANGED:
//    /* HTTP & RTSP errors */
//    case AV_HTTP_BAD_REQUEST:
//    case AV_HTTP_UNAUTHORIZED:
//    case AV_HTTP_FORBIDDEN:
//    case AV_HTTP_NOT_FOUND:
//    case AV_HTTP_OTHER_4XX:
//    case AV_HTTP_SERVER_ERROR:
//        return std::error_code( error, av_category );
//    }
//    return std::error_code ( abs( error ), std::generic_category() );
//}
//}//namespace libav

//namespace libav {



///**
// * @brief The MediaType struct
// */
//struct MediaType {
//    enum Enum {
//        UNKNOWN = -1,  ///< Usually treated as AVMEDIA_TYPE_DATA
//        VIDEO,
//        AUDIO,
//        DATA,          ///< Opaque data information usually continuous
//        SUBTITLE,
//        ATTACHMENT,    ///< Opaque data information usually sparse
//        NB
//    };
//    static Enum get( AVMediaType type );
//};

///**
// * @brief The Codec struct
// */
//struct Codec {
//    enum Enum {
//        NONE = -1, MP3, FLAC, AAC, VORBIS
//    };
//    static Enum get( AVCodecID codec_id );
//    static AVCodecID get( Enum codec );
//};

///**
// * @brief set/get the media information and metada
// */
//struct MediaInfo {
//    MediaType::Enum type;
//    Codec::Enum codec;
//    int bitrate;
//    int samplerate;
//    int channels;
//    int64_t playtime;
//    int bits_per_sample;
//    int width;
//    int height;
//    Metadata metadata;
//};

///**
// * @brief The Libav class
// */
//class Libav {
//public:
//    /** @brief Libav */
//    Libav ();
//    ~Libav ();

//    /**
//     * @brief input from file by name.
//     * @param filename the filename
//     * @return error
//     */
//    std::error_code input ( const std::string& filename );
//    /**
//     * @brief output to file by name
//     * @param filename the filename
//     * @return error
//     */
//    std::error_code output ( const std::string& filename );

//    std::error_code transcode ( MediaInfo& media_info );

//    /** @brief number of streams. */
//    unsigned short nb_streams ()
//    { return input_format_context->nb_streams; }

//    /** get the media information */
//    MediaInfo media_info ( const int index );

//private:
//    std::once_flag av_register_flag;
//    int64_t pts = 0;

//    std::shared_ptr < AVFormatContext > input_format_context;
//    std::shared_ptr < AVCodecContext > input_codec_context;
//    std::shared_ptr < AVFormatContext > output_format_context;
//    std::shared_ptr < AVCodecContext > output_codec_context;
//    std::shared_ptr < AVAudioFifo > fifo;
//    std::shared_ptr < SwrContext > resample_context;

//    std::error_code init_resampler();
//    std::error_code init_fifo();

//    std::error_code read_decode_convert_and_store( int *finished);
//    std::error_code decode_audio_frame( AVFrame *frame, int *data_present, int *finished);
//    std::error_code init_converted_samples(uint8_t ***converted_input_samples, int frame_size);
//    std::error_code convert_samples(const uint8_t **input_data,
//                               uint8_t **converted_data, const int frame_size );
//    std::error_code add_samples_to_fifo(AVAudioFifo *fifo,
//                                   uint8_t **converted_input_samples,
//                                   const int frame_size);
//    std::error_code load_encode_and_write ();
//    std::error_code init_output_frame(AVFrame **frame, int frame_size);
//    std::error_code encode_audio_frame(AVFrame *frame, int *data_present);
//};
//}//namespace libav
//#endif // LIBAV_H
