//#include "libav.h"

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

//#include "libaviocontext.h"
//#include "libavwrapper.h"

//namespace libav {

//inline Codec::Enum codec ( AVCodecID codec_id ) {
//    switch ( codec_id ) {
//    case AV_CODEC_ID_MP3:
//        return Codec::MP3;

//    case AV_CODEC_ID_FLAC:
//        return Codec::FLAC;

//    case AV_CODEC_ID_AAC:
//        return Codec::AAC;

//    case AV_CODEC_ID_VORBIS:
//        return Codec::VORBIS;

//    default:
//        return Codec::NONE;
//    }
//}
//inline AVCodecID codec ( Codec::Enum codec ) {
//    switch ( codec ) {
//    case Codec::MP3:
//        return AV_CODEC_ID_MP3;

//    case Codec::FLAC:
//        return AV_CODEC_ID_FLAC;

//    case Codec::AAC:
//        return AV_CODEC_ID_AAC;

//    case Codec::VORBIS:
//        return AV_CODEC_ID_VORBIS;

//    default:
//        return AV_CODEC_ID_NONE;
//    }
//}

//inline MediaType::Enum type ( AVMediaType type ) {
//    switch ( type ) {
//    case AVMEDIA_TYPE_VIDEO:
//        return MediaType::VIDEO;

//    case AVMEDIA_TYPE_AUDIO:
//        return MediaType::AUDIO;

//    case AVMEDIA_TYPE_DATA:
//        return MediaType::DATA;

//    case AVMEDIA_TYPE_SUBTITLE:
//        return MediaType::SUBTITLE;

//    case AVMEDIA_TYPE_ATTACHMENT:
//        return MediaType::ATTACHMENT;

//    case AVMEDIA_TYPE_NB:
//        return MediaType::NB;

//    default:
//        return MediaType::UNKNOWN;
//    }
//}


//LibAv::LibAv() {
//    av_ = std::make_unique< LibavWrapper >();
//}
//LibAv::~LibAv() {}

//Metadata& LibAv::metadata() {
//    return av_->metadata();
//}
//MediaInfo LibAv::stream ( const int index ) {

//    int secs;
//    if ( av_->input_format_context->duration != AV_NOPTS_VALUE ) {
//        int64_t duration = av_->input_format_context->duration + ( av_->input_format_context->duration <= INT64_MAX - 5000 ? 5000 : 0 );
//        secs  = duration / AV_TIME_BASE;
//    }

//    AVCodecContext* codec = av_->input_format_context->streams[index]->codec;
//    return (
//    MediaInfo {
//        type ( codec->codec_type ),
//        libav::codec ( codec->codec_id ),
//        codec->bit_rate,
//        codec->sample_rate,
//        codec->channels,
//        secs,
//        codec->bits_per_raw_sample,
//        codec->width,
//        codec->height,
//        _metadata
//    }
//    );
//}

//MediaInfo LibAv::media_info ( const int index ) {
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



//    AVCodecContext* codec = av_->input_format_context->streams[index]->codec;
//    return (
//    MediaInfo {
//        type ( codec->codec_type ),
//        libav::codec ( codec->codec_id ),
//        codec->bit_rate,
//        codec->sample_rate,
//        codec->channels,
//        secs,
//        codec->bits_per_raw_sample,
//        codec->width,
//        codec->height,
//        _metadata
//    }
//           );
//}

//unsigned short LibAv::nb_streams ()
//{ return av_->input_format_context->nb_streams; }

//std::error_code LibAv::input ( const std::string& filename ) {
//    return av_->make_input_format_context ( filename.c_str() );
//}
//std::error_code LibAv::input ( std::istream* is ) {
//    return av_->make_input_format_context ( is );
//}
//std::error_code LibAv::output ( const std::string& filename ) {
//    return av_->make_output_format_context ( filename.c_str() );
//}
//std::error_code LibAv::output ( MediaInfo& media_info ) {
//    return av_->make_output_format_context ( media_info.codec );
//}

//std::error_code LibAv::transcode ( int stream_number, MediaInfo& media_info ) {
//    std::error_code _errc;

//    if ( ( _errc = av_->make_input_codec_context ( stream_number ) ) ) { return _errc; }

//    if ( ( _errc = av_->make_output_codec_context ( media_info ) ) ) { return _errc; }

//    if ( ( _errc = av_->make_resampler_context () ) ) { return _errc; }

//    if ( ( _errc = av_->make_fifo () ) ) { return _errc; }

//    av_->write_output_metadata ( media_info.metadata );

//    if ( ( _errc = av_->write_header() ) ) { return _errc; }

//    if ( ( _errc = av_->transcode() ) ) { return _errc; }

//    if ( ( _errc = av_->write_trailer () ) ) { return _errc; }

//    return std::error_code ();
//}

//std::streamsize LibAv::readsome ( unsigned char* s, std::streamsize n ) {
//    //TODO
//    return -1;
//}
//std::string& LibAv::get_output() {
//    return av_->output_io_context->output;
//}
//}//namespace libav
