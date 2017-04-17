//#ifndef AUDIO_H
//#define AUDIO_H

//#include <algorithm>
//#include <vector>
//#include <map>
//#include <memory>
//#include <mutex>

////TODO remove
//#include <iostream>

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

//#include <stdio.h>

//enum av_errc {
//AV_ENOENT = -2,
//STREAM_ERROR,
//UNKNOWN
//};

//namespace std {
//template<> struct is_error_condition_enum<av_errc> : public true_type {};
//}

//class av_category_t : public std::error_category {
//public:
//  virtual const char* name() const noexcept { return "av"; }
//  virtual std::error_condition default_error_condition ( int ev ) const noexcept {
//    if ( ev == static_cast< int >( av_errc::AV_ENOENT ) )
//    { return std::error_condition( static_cast< av_errc >( ev ) ); }
//    else
//    { return std::error_condition( av_errc::UNKNOWN ); }
//  }
//  virtual bool equivalent ( const std::error_code& code, int condition ) const noexcept {
//    return *this==code.category() &&
//           static_cast< int >( default_error_condition( code.value()).value() ) == condition;
//  }
//  virtual std::string message(int ev) const {
//    static char error_buffer[255];
//    av_strerror( ev, error_buffer, sizeof( error_buffer ) );
//    return error_buffer;

////    return get_error_text( ev );
////    switch ( ev ) {
////    case OK:            return "Successful result.";
////    case FILE_ERROR:    return "Unable to open media file.";
////    case STREAM_ERROR:  return "Could not open find stream info.";
////    default: return "Unknown error.";
////    }
//  }
//} static av_category;

//// make_error_code overload to generate custom conditions:
//static std::error_condition make_error_condition (av_errc e) {
//    return std::error_condition(static_cast<int>(e), av_category);
//}

//namespace libav {

///**
// * @brief The Metadata struct
// *
// * <p>Media tag description and comparison table from http://age.hobba.nl/audio/tag_frame_reference.html</p>
// *
// * @htmlinclude tag_frame_reference.html
// */
//struct Metadata {
//public:
//    enum Enum {
//        TITLE, ALBUM, ARTIST, COMPOSER, PERFORMER, COMMENT, YEAR, TRACK, DISC, GENRE, PUBLISHER
//    };
//    void set( const char* name, const char* value ) {
//        if ( strcmp ( "TITLE", name ) == 0 )
//        { tags[ TITLE ] = value; }
//        else if( strcmp ( "ALBUM", name ) == 0 )
//        { tags[ ALBUM ] = value; }
//        else if( strcmp ( "ARTIST", name ) == 0 )
//        { tags[ ARTIST ] = value; }
//        else if( strcmp ( "COMPOSER", name ) == 0 )
//        { tags[ COMPOSER ] = value; }
//        else if( strcmp ( "PERFORMER", name ) == 0 )
//        { tags[ PERFORMER ] = value; }
//        else if( strcmp ( "COMMENT", name ) == 0 )
//        { tags[ COMMENT ] = value; }
//        else if( strcmp ( "YEAR", name ) == 0 )
//        { tags[ YEAR ] = value; }
//        else if( strcmp ( "track", name ) == 0 )
//        { tags[ TRACK ] = value; }
//        else if( strcmp ( "DISC", name ) == 0 )
//        { tags[ DISC ] = value; }
//        else if( strcmp ( "GENRE", name ) == 0 )
//        { tags[ GENRE ] = value; }
//        else if( strcmp ( "PUBLISHER", name ) == 0 )
//        { tags[ PUBLISHER ] = value; }

//        else if( std::find(std::begin( skip_list ), std::end( skip_list ), name ) == std::end( skip_list ) )
//        { std::cout << "\tTAG: " << name << " = " << value << std::endl; }
//    }

//    std::string& get ( const Enum& key ) {
//        return tags[ key ];
//    }

//    std::vector< Enum > tag_names () {
//        std::vector< Enum > _keys;
//        for( auto const& k: tags )
//            _keys.push_back( k.first );
//        return _keys;
//    }

//private:
//    std::map< Enum, std::string > tags;
//    std::vector < std::string > skip_list { { "LC", "ITUNESCOMPILATION", "TAGGING_TIME" } };
//};

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
//    static Enum get( AVMediaType type ) {
//        switch( type ) {
//        case AVMEDIA_TYPE_VIDEO:    return VIDEO;
//        case AVMEDIA_TYPE_AUDIO:    return AUDIO;
//        case AVMEDIA_TYPE_DATA:     return DATA;
//        case AVMEDIA_TYPE_SUBTITLE: return SUBTITLE;
//        case AVMEDIA_TYPE_ATTACHMENT: return ATTACHMENT;
//        case AVMEDIA_TYPE_NB:       return NB;
//        default: return UNKNOWN;
//        }
//    }
//};

//struct Codec {
//    enum Enum {
//        NONE = -1, MP3, FLAC, AAC, VORBIS
//    };
//    static Enum get( AVCodecID codec_id ) {
//        switch( codec_id ) {
//        case AV_CODEC_ID_MP3:  return MP3;
//        case AV_CODEC_ID_FLAC:  return FLAC;
//        case AV_CODEC_ID_AAC:  return AAC;
//        case AV_CODEC_ID_VORBIS:  return VORBIS;
//        default: return NONE;
//        }
//    }
//    static AVCodecID get( Enum codec ) {
//        switch( codec ) {
//        case MP3: return AV_CODEC_ID_MP3;
//        case FLAC: return AV_CODEC_ID_FLAC;
//        case AAC: return AV_CODEC_ID_AAC;
//        case VORBIS: return AV_CODEC_ID_VORBIS;
//        static_assert( true, "wrong codec selected" );
//        }
//    }
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

//class IOContext {
//public:
//    IOContext( std::istream* input );
//    IOContext & operator=(const IOContext&) = delete;
//    IOContext(const IOContext&) = delete;
//    ~IOContext();

//    void initAVFormatContext(AVFormatContext *);

//    std::istream* datafile;
//    AVIOContext *ioCtx;

//    const int bufferSize = 32 * 1024;
//    unsigned char* buffer;

////    uint8_t *buffer; // internal buffer for ffmpeg
////    int bufferSize;
//};

///**
// * @brief The Audio class
// */
//class Audio {
//public:
//    Audio();
//    ~Audio() {}

//    /**
//     * @brief set the input file
//     * @param filename filename
//     * @return av error code
//     */
//    std::error_code input ( const std::string& filename );
//    /**
//     * @brief set the input stream
//     * @param istream input stream
//     * @return av error code
//     */
//    std::error_code input ( std::istream* istream );




//    void transcode( const char* output, MediaInfo media_info );
//    void transcode( MediaInfo media_info );
//    size_t read( char* buffer, size_t size );


//    /** @brief number of streams. */
//    unsigned short nb_streams ()
//    { return input_format_context_->nb_streams; }

//    /** get the media information */
//    MediaInfo media_info ( const int index );

//private:
//    std::once_flag register_flag;
//    std::once_flag register_avfilter_flag;

//    int open_input_file(const char *filename);
//    int open_output_file( const char *filename, MediaInfo media_info );
//    int write_output_file_trailer();
//    int load_encode_and_write( AVAudioFifo *fifo );
//    int encode_audio_frame( AVFrame *frame, int *data_present );
//    int init_output_frame(AVFrame **frame, int frame_size);
//    int read_decode_convert_and_store(AVAudioFifo *fifo,
//                                             SwrContext *resampler_context,
//                                             int *finished);
//    const char* get_error_text(const int error); //TODO remove
//    void init_packet(AVPacket *packet);
//    int init_input_frame(AVFrame **frame);
//    int init_resampler(SwrContext **resample_context);
//    int init_fifo(AVAudioFifo **fifo );
//    int write_output_file_header();
//    int decode_audio_frame(AVFrame *frame, int *data_present, int *finished);
//    int init_converted_samples(uint8_t ***converted_input_samples, int frame_size);
//    int convert_samples(const uint8_t **input_data,
//                               uint8_t **converted_data, const int frame_size,
//                               SwrContext *resample_context);
//    int add_samples_to_fifo(AVAudioFifo *fifo,
//                                   uint8_t **converted_input_samples,
//                                   const int frame_size);

//    std::shared_ptr< AVFormatContext > input_format_context_;
//    std::shared_ptr< AVCodecContext > input_codec_context;
//    std::shared_ptr< AVFormatContext > output_format_context;
//    std::shared_ptr< AVCodecContext > output_codec_context;
//    std::shared_ptr< IOContext > io_context;
//};
//}//namespace libav
//#endif // AUDIO_H
