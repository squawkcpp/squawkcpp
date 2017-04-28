#ifndef API_H
#define API_H

#include <iostream>

#include "http/request.h"
#include "http/response.h"

#include "fmt/format.h"
#include "fmt/time.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include <boost/lexical_cast.hpp>

#include "config.h"
#include "dao.h"

namespace squawk {
namespace __Api {
template< typename Request >
inline size_t parse_id( Request& request ) {
    return ( request.contains_attribute( "id" ) ? std::stoi( request.attribute ( "id" ) ) : 0 );
}
template< typename Request >
inline size_t parse_page( Request& request ) {
    int page = 0;
    if ( request.contains_attribute ( "index" ) )
    { page = boost::lexical_cast<int> ( request.attribute ( "index" ) ); }
    return ( page );
}
template< typename Request >
inline size_t parse_limit( Request& request ) {
    int limit = 0;
    if ( request.contains_attribute ( "count" ) )
    { limit = boost::lexical_cast<int> ( request.attribute ( "count" ) ); }
    return ( limit == 0 || limit > 128 ? 128 : limit );
}
template< typename W, typename Connection >
static void artists( W& writer, Connection con, const int id ) {
    writer.String( "artists" );
    writer.StartArray();
    dao::child< upnp::didl::DidlContainerArtist > ( con, [&writer] (auto statement) {

        writer.StartObject();
        writer.String( upnp::didl::DidlContainerArtist::attribute( upnp::didl::DidlContainerArtist::title ).c_str() );
        writer.String( db::get< const char* >( statement, upnp::didl::DidlMusicTrack::title ) );
        writer.EndObject();

    }, id );
    writer.EndArray();
}
template< typename W, typename Connection >
static void albumCoverUri( W& writer, Connection con, const int id ) {
    dao::child< upnp::didl::DidlAlbumArt > ( con, "(title='front' or title='poster')", [&writer] (auto statement) {
        writer.String( "albumArtUri" );
        writer.String( fmt::format( "http://{0}:{1}/albumArt/tn_{2}.jpg",
                       squawk::Config::instance()->httpHost(),
                       squawk::Config::instance()->httpPort(),
                       db::get< size_t >( statement, upnp::didl::DidlAlbumArt::id ) ).c_str() );
    }, id );
}
template< typename W, typename Connection >
static void albumArtUri( W& writer, Connection con, const int id ) {
    writer.String( "albumArt" );
    writer.StartArray();
    dao::child< upnp::didl::DidlAlbumArt > ( con, [&writer] (auto statement) {
        writer.StartObject();
        writer.String( upnp::didl::DidlContainerArtist::attribute( upnp::didl::DidlAlbumArt::title ).c_str() );
        writer.String( db::get< const char * >( statement, upnp::didl::DidlAlbumArt::title ) );
        writer.String( "uri" );
        writer.String( fmt::format( "http://{0}:{1}/res/{2}.{3}",
                       squawk::Config::instance()->httpHost(),
                       squawk::Config::instance()->httpPort(),
                       db::get< size_t >( statement, upnp::didl::DidlAlbumArt::id ),
                       http::mime::extension( db::get< http::mime::MIME_TYPE >( statement, upnp::didl::DidlAlbumArt::mime_type ) ) ).c_str() );
        writer.String( "TN" );
        writer.String( fmt::format( "http://{0}:{1}/albumArt/tn_{2}.jpg",
                       squawk::Config::instance()->httpHost(),
                       squawk::Config::instance()->httpPort(),
                       db::get< size_t >( statement, upnp::didl::DidlAlbumArt::id ) ).c_str() );
        writer.EndObject();
    }, id );
    writer.EndArray();
}

inline std::vector< upnp::didl::DIDL_CLASS > classes( const std::string& query ) {
    std::vector< upnp::didl::DIDL_CLASS > _results;
    std::istringstream ss;
    ss.str( query );
    std::string _item;
    while ( std::getline( ss, _item, ',' ) ) {
        if( _item == "album" ) {
            _results.push_back( upnp::didl::DIDL_CLASS::objectContainerAlbumMusicAlbum );
        } else if( _item == "serie" ) {
            _results.push_back( upnp::didl::DIDL_CLASS::objectContainerSeries );
        } else if( _item == "movie" ) {
            _results.push_back( upnp::didl::DIDL_CLASS::objectItemVideoItemMovie);
        } else if( _item == "ebook" ) {
            _results.push_back( upnp::didl::DIDL_CLASS::objectItemEBook );
        } else if( _item == "image" ) {
            _results.push_back( upnp::didl::DIDL_CLASS::objectItemImageItemPhoto );
        }
    }
    return _results;
}
}//namespace __Api
}//namespace squawk

namespace squawk {
/**
 * @brief The Api struct
 */
struct Api {
    /**
     * @brief album
     * @param request
     * @param response
     * @return
     */
    static http::http_status album ( http::Request& request, http::Response& response ) {
        using namespace rapidjson;
        StringBuffer sb;
        PrettyWriter<StringBuffer> writer(sb);

        dao::item< upnp::didl::DidlContainerAlbum > ( db::make_connection( Config::instance()->databaseFile() ), [&writer] (auto statement) {

                writer.StartObject();
                writer.String( "path" );
                writer.String( fmt::format("/{0}/{1}",
                    upnp::didl::className( db::get< upnp::didl::DIDL_CLASS >( statement, upnp::didl::DidlContainerAlbum::cls ) ),
                    db::get< int >( statement, upnp::didl::DidlObject::id )
                ).c_str() );

                writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlContainerAlbum::title ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlContainerAlbum::title ) );
                writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlContainerAlbum::profile ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlContainerAlbum::profile ));
                writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlContainerAlbum::rating ).c_str() );
                writer.Uint( db::get< int >( statement, upnp::didl::DidlContainerAlbum::rating ));
                writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlContainerAlbum::year).c_str() );
                std::time_t time_ = db::get< int >( statement, upnp::didl::DidlContainerAlbum::year );
                writer.String( fmt::format("{:%Y}", *std::localtime( &time_ ) ).c_str() );
                writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlContainerAlbum::playback_count ).c_str() );
                writer.Uint( db::get< int >( statement, upnp::didl::DidlContainerAlbum::playback_count ));
                writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlContainerAlbum::genre ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlContainerAlbum::genre ) );

                writer.String( "tracks" );
                writer.StartArray();
                dao::child< upnp::didl::DidlMusicTrack > ( db::make_connection( Config::instance()->databaseFile() ), [&writer] (auto statement) {

                    writer.StartObject();
                    writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlMusicTrack::track ).c_str() );
                    writer.Uint( db::get< int >( statement, upnp::didl::DidlMusicTrack::track ) );
                    writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlMusicTrack::title ).c_str() );
                    writer.String( db::get< const char* >( statement, upnp::didl::DidlMusicTrack::title ) );
                    writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlMusicTrack::profile ).c_str() );
                    writer.String( db::get< const char* >( statement, upnp::didl::DidlMusicTrack::profile ));
                    writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlMusicTrack::size ).c_str() );
                    writer.Uint( db::get< int >( statement, upnp::didl::DidlMusicTrack::size ));
                    writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlMusicTrack::disc ).c_str() );
                    writer.Uint( db::get< int >( statement, upnp::didl::DidlMusicTrack::disc ));
                    writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlMusicTrack::bitrate ).c_str() );
                    writer.Uint( db::get< int >( statement, upnp::didl::DidlMusicTrack::bitrate ));
                    writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlMusicTrack::bits_per_sample ).c_str() );
                    writer.Uint( db::get< int >( statement, upnp::didl::DidlMusicTrack::bits_per_sample ) );
                    writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlMusicTrack::nr_audio_channels ).c_str() );
                    writer.Uint( db::get< int >( statement, upnp::didl::DidlMusicTrack::nr_audio_channels ) );
                    writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlMusicTrack::duration ).c_str() );
                    std::time_t time_ = db::get< int >( statement, upnp::didl::DidlMusicTrack::duration );
                    writer.String( fmt::format("{:%H:%M:%S}", *std::localtime( &time_ ) ).c_str() );
                    writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlMusicTrack::sample_frequency ).c_str() );
                    writer.Uint( db::get< int >( statement, upnp::didl::DidlMusicTrack::sample_frequency ) );
                    writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlMusicTrack::comment ).c_str() );
                    writer.String( db::get< const char* >( statement, upnp::didl::DidlMusicTrack::comment ) );
                    writer.String( "uri" );
                    writer.String( fmt::format( "http://{0}:{1}/res/{2}.{3}",
                                   squawk::Config::instance()->httpHost(),
                                   squawk::Config::instance()->httpPort(),
                                   db::get< const char* >( statement, upnp::didl::DidlMusicTrack::id ),
                                   http::mime::extension( db::get< http::mime::MIME_TYPE >( statement, upnp::didl::DidlMusicTrack::mime_type ) ) ).c_str() );
                    writer.EndObject();

                }, db::get< int >( statement, upnp::didl::DidlObject::id ) );
                writer.EndArray();

                __Api::albumCoverUri( writer, db::make_connection( Config::instance()->databaseFile() ), db::get< int >( statement, upnp::didl::DidlObject::id ) );
                __Api::albumArtUri( writer, db::make_connection( Config::instance()->databaseFile() ), db::get< int >( statement, upnp::didl::DidlObject::id ) );
                __Api::artists( writer, db::make_connection( Config::instance()->databaseFile() ), db::get< int >( statement, upnp::didl::DidlObject::id ) );
                writer.EndObject();

            }, std::atoi( request.attribute( "id" ).c_str() )
        );

        response << sb.GetString();
        response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::JSON ) );
        return ( http::http_status::OK );
    }

    /**
     * @brief book
     * @param request
     * @param response
     * @return
     */
    static http::http_status book ( http::Request& request, http::Response& response ) {
        using namespace rapidjson;
        StringBuffer sb;
        PrettyWriter<StringBuffer> writer(sb);

        dao::item< upnp::didl::DidlEBook > ( db::make_connection( Config::instance()->databaseFile() ), [&writer] (auto statement) {

                writer.StartObject();
                writer.String( "path" );
                writer.String( fmt::format("/{0}/{1}",
                    upnp::didl::className( db::get< upnp::didl::DIDL_CLASS >( statement, upnp::didl::DidlEBook::cls ) ),
                    db::get< int >( statement, upnp::didl::DidlObject::id )
                ).c_str() );

                writer.String( upnp::didl::DidlEBook::attribute( upnp::didl::DidlEBook::title ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlEBook::title ) );
                writer.String( upnp::didl::DidlEBook::attribute( upnp::didl::DidlEBook::profile ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlEBook::profile ));
                writer.String( upnp::didl::DidlEBook::attribute( upnp::didl::DidlEBook::year).c_str() );
                std::time_t time_ = db::get< int >( statement, upnp::didl::DidlEBook::year );
                writer.String( fmt::format("{:%Y}", *std::localtime( &time_ ) ).c_str() );
                writer.String( upnp::didl::DidlEBook::attribute( upnp::didl::DidlEBook::comment ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlEBook::comment ) );
                writer.String( upnp::didl::DidlEBook::attribute( upnp::didl::DidlEBook::isbn ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlEBook::isbn) );
                writer.String( "uri" );
                writer.String( fmt::format( "http://{0}:{1}/res/{2}.{3}",
                               squawk::Config::instance()->httpHost(),
                               squawk::Config::instance()->httpPort(),
                               db::get< const char* >( statement, upnp::didl::DidlEBook::id ),
                               http::mime::extension( db::get< http::mime::MIME_TYPE >( statement, upnp::didl::DidlEBook::mime_type ) ) ).c_str() );

                //autor
                dao::child< upnp::didl::DidlPersonAuthor > ( db::make_connection( Config::instance()->databaseFile() ), [&writer] (auto statement) {
                    writer.String( "author" );
                    writer.String( db::get< const char* >( statement, upnp::didl::DidlPersonAuthor::title ) );
                }, db::get< int >( statement, upnp::didl::DidlEBook::id ) );

                //publisher
                dao::child< upnp::didl::DidlPersonPublisher > ( db::make_connection( Config::instance()->databaseFile() ), [&writer] (auto statement) {
                    writer.String( "publisher" );
                    writer.String( db::get< const char* >( statement, upnp::didl::DidlPersonPublisher::title ) );
                }, db::get< int >( statement, upnp::didl::DidlEBook::id ) );

                __Api::albumArtUri( writer, db::make_connection( Config::instance()->databaseFile() ), db::get< int >( statement, upnp::didl::DidlObject::id ) );

                writer.EndObject();

            }, std::atoi( request.attribute( "id" ).c_str() )
        );

        response << sb.GetString();
        response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::JSON ) );
        return ( http::http_status::OK );
    }

    /**
     * @brief movie
     * @param request
     * @param response
     * @return
     */
    static http::http_status movie ( http::Request& request, http::Response& response ) {
        using namespace rapidjson;
        StringBuffer sb;
        PrettyWriter<StringBuffer> writer(sb);

        dao::item< upnp::didl::DidlMovie > ( db::make_connection( Config::instance()->databaseFile() ), [&writer] (auto statement) {

                writer.StartObject();
                writer.String( "path" );
                writer.String( fmt::format("/{0}/{1}",
                    upnp::didl::className( db::get< upnp::didl::DIDL_CLASS >( statement, upnp::didl::DidlMovie::cls ) ),
                    db::get< int >( statement, upnp::didl::DidlObject::id )
                ).c_str() );

                writer.String( upnp::didl::DidlMovie::attribute( upnp::didl::DidlMovie::title ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlMovie::title ) );
                writer.String( upnp::didl::DidlMovie::attribute( upnp::didl::DidlMovie::profile ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlMovie::profile ));
//TODO                writer.String( upnp::didl::_DidlMovie::attribute( upnp::didl::_DidlMovie::year).c_str() );
//                std::time_t time_ = db::get< int >( statement, upnp::didl::_DidlMovie::year );
//                writer.String( fmt::format("{:%Y}", *std::localtime( &time_ ) ).c_str() );
//                writer.String( upnp::didl::_DidlMovie::attribute( upnp::didl::_DidlMovie::comment ).c_str() );
//                writer.String( db::get< const char* >( statement, upnp::didl::_DidlMovie::comment ) );
//                writer.String( upnp::didl::_DidlMovie::attribute( upnp::didl::_DidlMovie::isbn ).c_str() );
//                writer.String( db::get< const char* >( statement, upnp::didl::_DidlMovie::isbn) );
                writer.String( "uri" );
                writer.String( fmt::format( "http://{0}:{1}/res/{2}.{3}",
                               squawk::Config::instance()->httpHost(),
                               squawk::Config::instance()->httpPort(),
                               db::get< const char* >( statement, upnp::didl::DidlMovie::id ),
                               http::mime::extension( db::get< http::mime::MIME_TYPE >( statement, upnp::didl::DidlMovie::mime_type ) ) ).c_str() );

                //autor
                dao::child< upnp::didl::DidlPersonAuthor > ( db::make_connection( Config::instance()->databaseFile() ), [&writer] (auto statement) {
                    writer.String( "author" );
                    writer.String( db::get< const char* >( statement, upnp::didl::DidlPersonAuthor::title ) );
                }, db::get< int >( statement, upnp::didl::DidlMovie::id ) );

                //publisher
                dao::child< upnp::didl::DidlPersonPublisher > ( db::make_connection( Config::instance()->databaseFile() ), [&writer] (auto statement) {
                    writer.String( "publisher" );
                    writer.String( db::get< const char* >( statement, upnp::didl::DidlPersonPublisher::title ) );
                }, db::get< int >( statement, upnp::didl::DidlMovie::id ) );

                __Api::albumArtUri( writer, db::make_connection( Config::instance()->databaseFile() ), db::get< int >( statement, upnp::didl::DidlObject::id ) );

                writer.EndObject();

            }, std::atoi( request.attribute( "id" ).c_str() )
        );

        response << sb.GetString();
        response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::JSON ) );
        return ( http::http_status::OK );
    }

    /**
     * @brief season
     * @param request
     * @param response
     * @return
     */
    static http::http_status series ( http::Request& request, http::Response& response ) {
        using namespace rapidjson;
        StringBuffer sb;
        PrettyWriter<StringBuffer> writer(sb);

        writer.StartObject();
        dao::item< upnp::didl::DidlContainerSeries > ( db::make_connection( Config::instance()->databaseFile() ), [&writer] (auto statement) {

                writer.String( "path" );
                writer.String( fmt::format("/{0}/{1}",
                    upnp::didl::className( db::get< upnp::didl::DIDL_CLASS >( statement, upnp::didl::DidlContainerSeries::cls ) ),
                    db::get< int >( statement, upnp::didl::DidlObject::id )
                ).c_str() );

                writer.String( upnp::didl::DidlContainerSeries::attribute( upnp::didl::DidlContainerSeries::title ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlContainerSeries::title ) );
                writer.String( upnp::didl::DidlContainerSeries::attribute( upnp::didl::DidlContainerSeries::profile ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlContainerSeries::profile ));
                writer.String( upnp::didl::DidlContainerSeries::attribute( upnp::didl::DidlContainerSeries::year).c_str() );
                std::time_t time_ = db::get< int >( statement, upnp::didl::DidlContainerSeries::year );
                writer.String( fmt::format("{:%Y}", *std::localtime( &time_ ) ).c_str() );
                writer.String( upnp::didl::DidlContainerSeries::attribute( upnp::didl::DidlContainerSeries::comment ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlContainerSeries::comment ) );
                writer.String( "uri" );
                writer.String( fmt::format( "http://{0}:{1}/res/{2}.{3}",
                               squawk::Config::instance()->httpHost(),
                               squawk::Config::instance()->httpPort(),
                               db::get< const char* >( statement, upnp::didl::DidlContainerSeries::id ),
                               http::mime::extension( db::get< http::mime::MIME_TYPE >( statement, upnp::didl::DidlContainerSeries::mime_type ) ) ).c_str() );

                __Api::albumArtUri( writer, db::make_connection( Config::instance()->databaseFile() ), db::get< int >( statement, upnp::didl::DidlObject::id ) );

            }, std::atoi( request.attribute( "id" ).c_str() )
        );

        db::exec( db::make_connection( Config::instance()->databaseFile() ),
                  "select distinct o.season from tbl_cds_object as o JOIN tbl_cds_mapping as m where m.parent=? and o.cls=? and m.child = o.ROWID",
                [&request,&writer](auto stmt) {

                    writer.String( fmt::format( "Season: {}", db::get< int >( stmt, 0 ) ).c_str() );
                    writer.StartArray();
                    std::cout << "get season " << db::get< size_t >( stmt, 0 ) << std::endl;
                    dao::child< upnp::didl::DidlEpisode > ( db::make_connection( Config::instance()->databaseFile() ),
                                                            fmt::format( "(season={})", db::get< size_t >( stmt, 0 ) ),
                                                            [&request,&writer] (auto statement) {
//                        writer.String( "albumArtUri" );
//                        writer.String( fmt::format( "http://{0}:{1}/albumArt/tn_{2}.jpg",
//                                       squawk::Config::instance()->httpAddress(),
//                                       squawk::Config::instance()->httpPort(),
//                                       db::get< size_t >( statement, upnp::didl::DidlAlbumArt::id ) ).c_str() );

                        std::cout << "found episode " << db::get< const char* >( statement, upnp::didl::DidlEpisode::title ) << std::endl;

                        writer.StartObject();
                        writer.String( upnp::didl::DidlEpisode::attribute( upnp::didl::DidlEpisode::season ).c_str() );
                        writer.Int( db::get< int >( statement, upnp::didl::DidlEpisode::season ) );
                        writer.String( upnp::didl::DidlEpisode::attribute( upnp::didl::DidlEpisode::episode ).c_str() );
                        writer.Int( db::get< int >( statement, upnp::didl::DidlEpisode::episode ) );
                        writer.String( upnp::didl::DidlEpisode::attribute( upnp::didl::DidlEpisode::title ).c_str() );
                        writer.String( db::get< const char* >( statement, upnp::didl::DidlEpisode::title ) );

                        writer.EndObject();

                    },  std::atoi( request.attribute( "id" ).c_str() ) );
                    writer.EndArray();

                }, std::atoi( request.attribute( "id" ).c_str() ), upnp::didl::DIDL_CLASS::objectItemVideoItemEpisode );

        writer.EndObject();
        response << sb.GetString();
        response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::JSON ) );
        return ( http::http_status::OK );
    }

    /**
     * @brief query
     * @param request
     * @param response
     * @return
     */
    static http::http_status query ( http::Request& request, http::Response& response ) {
        CLOG(DEBUG, "cds") << request;

        const unsigned long page_ = __Api::parse_page( request );
        const unsigned long limit_ = __Api::parse_limit( request );
        const auto classes_ = __Api::classes( request.attribute( "classes" ) );

        using namespace rapidjson;
        StringBuffer sb;
        PrettyWriter<StringBuffer> writer(sb);

        writer.StartArray();

        auto connection_ = db::make_connection( Config::instance()->databaseFile() );
        dao::query< upnp::didl::DidlObject >::run( connection_, classes_, upnp::didl::DidlObject::title, true,
            [&writer,&connection_,&page_, &limit_, &classes_](auto statement) {
                writer.StartObject();
                writer.String( "uri" );
                writer.String( fmt::format("/{0}/{1}",
                    upnp::didl::className( db::get< upnp::didl::DIDL_CLASS >( statement, upnp::didl::DidlObject::cls ) ),
                    db::get< unsigned long >( statement, upnp::didl::DidlObject::id )
                ).c_str() );

                writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlObject::title ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlObject::title ) );
                writer.String( upnp::didl::DidlMusicTrack::attribute( upnp::didl::DidlObject::profile ).c_str() );
                writer.String( db::get< const char* >( statement, upnp::didl::DidlObject::profile ));
                __Api::albumCoverUri( writer, db::make_connection( Config::instance()->databaseFile() ), db::get< unsigned long >( statement, upnp::didl::DidlObject::id ) );
                __Api::artists( writer, db::make_connection( Config::instance()->databaseFile() ), db::get< unsigned long >( statement, upnp::didl::DidlObject::id ) );
                writer.EndObject();

        }, page_, limit_ );

        writer.EndArray();

        response << sb.GetString();
        //TODO must be done in http::HEADER
        response.expires( 0 );
        response.parameter ( http::header::CACHE_CONTROL, "no-cache" );
        response.parameter ( http::header::PRAGMA, "no-cache" );
        response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::JSON ) );
        return ( http::http_status::OK );
    }
};
}//namespace squawk
#endif // API_H
