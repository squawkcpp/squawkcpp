/*
    <squawk multimedia server>
    Copyright (C) 2017  <etienne knecht> <squawkcpp@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "scanner.h"

#ifdef BUILD_TESTS
#include <gtest/gtest.h>
#endif

#include <chrono>
#include <thread>

#include <boost/filesystem.hpp>

#include "dao.h"
#include "database.h"
#include "upnp/upnp.h"
#include "config.h"

#include "image.h"

#include "utils/epubparser.h"
#include "utils/pdfparser.h"
#include "utils/bookresultitem.h"
#include "utils/amazonfacade.h"
#include "utils/googlebooks.h"
#include "utils/tmdb.h"

#include "http/httpclient.h" //TODO move to amazon facade
#include "http/mimetypes.h"
#include "re2/re2.h"


namespace squawk {
namespace __Scanner {
inline std::string parse_multidisc( const std::string & path ) {
    std::string path_;
    static RE2 _parse_multidisc_regex("(.*)/(?i)(cd ?\\d*|dis(c|k) \\d* ?of ?\\d*|dis(c|k) ?\\d*)" );
    if( RE2::FullMatch( path, _parse_multidisc_regex, &path_ ) ) {
        return path_;
    } else return path;
}

#ifdef BUILD_TESTS
TEST(TestScanner, parse_multidisc) {
    EXPECT_EQ( "/foo/bar", parse_multidisc( "/foo/bar" ) );
    EXPECT_EQ( "/foo/bar", parse_multidisc( "/foo/bar/CD01" ) );
    EXPECT_EQ( "/foo/bar", parse_multidisc( "/foo/bar/CD 01" ) );
    EXPECT_EQ( "/foo/bar", parse_multidisc( "/foo/bar/disk 1" ) );
    EXPECT_EQ( "/foo/bar", parse_multidisc( "/foo/bar/disk 1 of 2" ) );
    EXPECT_EQ( "/foo/bar", parse_multidisc( "/foo/bar/disc 1" ) );
    EXPECT_EQ( "/foo/bar", parse_multidisc( "/foo/bar/disC 1 of 2" ) );
    EXPECT_EQ( "/foo/bar", parse_multidisc( "/foo/bar/disk1" ) );
    EXPECT_EQ( "/foo/bar", parse_multidisc( "/foo/bar/disk 1of2" ) );
}
#endif

static bool parse_series( const std::string& filename, int* season, int* episode, std::string* name ) {
    RE2 _parse_series_regex("(?i)(.*)S(\\d*)E(\\d*).*");
    if( RE2::FullMatch( filename, _parse_series_regex, name, season, episode ) ) {
        std::replace( name->begin(), name->end(), '.', ' ');
        boost::trim(*name);
        return true;
    } else return false;
}

#ifdef BUILD_TESTS
TEST(TestScanner, parse_series ) {
    int season_, episode_;
    std::string name_;
    ASSERT_TRUE( parse_series( "The.Simpsons.S27E19.720p.HDTV.x264-AVS.[VTV]", &season_, &episode_, &name_ ) );
    ASSERT_EQ( season_, 27 );
    ASSERT_EQ( episode_, 19 );
    ASSERT_EQ( name_, "The Simpsons" );
}
#endif

static std::string clean_name ( const std::string & name ) {
    std::string s = name;

    std::string allowed = "+-()";
    s.erase ( remove_if ( s.begin(), s.end(), [&allowed] ( const char& c ) {
        return allowed.find ( c ) != std::string::npos;
    } ), s.end() );

    boost::algorithm::trim ( s );
    boost::algorithm::to_lower ( s );

    if ( s.rfind ( "the ", 0 ) == 0 )
    { s.erase ( 0, 4 ); }

    if ( s.rfind ( "die ", 0 ) == 0 )
    { s.erase ( 0, 4 ); }

    if ( s.rfind ( "das ", 0 ) == 0 )
    { s.erase ( 0, 4 ); }

    if ( s.rfind ( "der ", 0 ) == 0 )
    { s.erase ( 0, 4 ); }

    return s;
}

#ifdef BUILD_TESTS
TEST(TestScanner, clean_name ) {
    ASSERT_STREQ("clean name", clean_name("clean name").c_str());
    ASSERT_STREQ("clean name", clean_name(" clean name ").c_str());
    ASSERT_STREQ("clean name", clean_name(" Clean Name ").c_str());
    ASSERT_STREQ("clean name", clean_name("+-Clean Name ").c_str());
    ASSERT_STREQ("clean name", clean_name("The Clean Name ").c_str());
    ASSERT_STREQ("clean name", clean_name("Das Clean Name ").c_str());
}
#endif

/**
 * @brief get the mime-type from the boost path object.
 * @param extension the file extension including the dot (i.e. .txt)
 * @return the mime-type
 */
inline http::mime::MIME_TYPE mime_type ( const std::string & extension ) {
    if(  extension.size() == 0 || extension == "." ) return http::mime::MIME_TYPE::TEXT;
    else return http::mime::mime_type ( boost::algorithm::to_lower_copy( extension.substr( 1 ) ) );
}

#ifdef BUILD_TESTS
TEST(TestScanner, mime_type ) {
    ASSERT_EQ( http::mime::FLAC, mime_type( ".flac" ) );
    ASSERT_EQ( http::mime::GIF, mime_type( ".gif" ) );
    ASSERT_EQ( http::mime::JPEG, mime_type( ".jpg" ) );
    ASSERT_EQ( http::mime::MKV, mime_type( ".mkv" ) );
    ASSERT_EQ( http::mime::PDF, mime_type( ".pdf" ) );
}
#endif


inline std::string profile ( const http::mime::MIME_TYPE& mime_type ) {
    switch( mime_type ) {
    case http::mime::MIME_TYPE::JPEG: return "JPEG";
    case http::mime::MIME_TYPE::PNG: return "PNG";
    case http::mime::MIME_TYPE::PDF: return "PDF";
    default: std::cout << "no profile found for MimeType: " << static_cast< int >( mime_type ) << std::endl;
    }
    return "UNKOWN";
}


struct CoverList {
public:
    struct Cover {
        size_t id; int width, height;
    };

    bool add( size_t id, std::string name, int width, int height ) {
        if( covers_.find( name ) == covers_.end() ) {
            covers_[ name ] = Cover { id, width, height };
            return true;
        } else {
            if( covers_[ name ].width < width && covers_[ name ].height < height ) {
                covers_[ name ] = Cover { id, width, height };
                return true;
            } else return false;
        }
    }

    std::map< std::string, Cover > covers_;
};

#ifdef BUILD_TESTS
TEST(TestScanner, cover_item) {

    squawk::__Scanner::CoverList c;
    ASSERT_TRUE( c.add( 1U, "other", 200, 200 ) );
    ASSERT_TRUE( c.add( 1U, "front", 16, 16 ) );
    ASSERT_TRUE( c.add( 1U, "front", 200, 200 ) );
    ASSERT_FALSE( c.add( 1U, "front", 16, 16 ) );
    EXPECT_EQ( 2U, c.covers_.size() );
}
#endif
}//namespace __Scanner
}//namespace squawk


template< typename Connection >
static void sweep( Connection /*connection*/, size_t /*time*/ ) {
    std::cout << "would sweep items." << std::endl;
    std::cout << "============================================================================================" << std::endl;
//    db::exec( connection,
//            "delete from tbl_cds_object where timestamp < ?", [] (auto) {
//        // std::cout << db::get< size_t >( stmt, 0 ) << ":" << db::get< const char* >( stmt, 2 ) << " -> " << db::get< const char* >( stmt, 1 ) << std::endl;
//    }, time );

//    if( ! db::exec( connection,
//            "delete from tbl_cds_mapping where child not in ( select ROWID from tbl_cds_object )", [] (auto) {
//        // std::cout << db::get< size_t >( stmt, 0 ) << ":" << db::get< const char* >( stmt, 2 ) << " -> " << db::get< const char* >( stmt, 1 ) << std::endl;
//                  std::cout << "mappings deleted." << std::endl;
//    }) ) {
//        std::cout << "error in delete mappings." << std::endl;
//    } else std::cout << "mappings OK." << std::endl;

    std::cout << "============================================================================================" << std::endl;



}

namespace squawk {
struct ParseAlbums {
    template< typename Connection >
    static void run( Connection connection ) {
        db::exec( connection,
                "select distinct parent_id from tbl_cds_object as o where o.cls=? and o.import=0",
                [&connection] (auto stmt) {
                    const std::string clean_name_ = __Scanner::parse_multidisc( db::get<const char*>( stmt, 0 ) ) + std::string( "%" );
                    std::vector< int > track_list_, photo_list_;
                    __Scanner::CoverList _cover_list;
                    size_t year_=0;
                    std::string title_, profile_, genre_, album_;

                    db::exec( connection,
                        "select ROWID, cls, title, profile, year, genre, album, width, height from tbl_cds_object where parent_id like ?",
                        [&connection, &track_list_,&photo_list_,&year_,&title_,&profile_,&genre_,&album_,&_cover_list] (auto stmt) {
                            const upnp::didl::DIDL_CLASS cls_ = static_cast< upnp::didl::DIDL_CLASS >( db::get< int >( stmt, 1 ) );
                            switch( cls_ ) {
                            case upnp::didl::DIDL_CLASS::objectItemAudioItemMusicTrack:
                                track_list_.push_back( db::get< int >( stmt, 0 ) );
                                title_ = db::get< const char* >( stmt, 2 );
                                profile_ = db::get< const char* >( stmt, 3 );
                                year_ = db::get< int >( stmt, 4 );
                                genre_ = db::get< const char* >( stmt, 5 );
                                album_ = db::get< const char* >( stmt, 6 );

                                //update import flag
                                db::exec( connection, "update tbl_cds_object set import=? where ROWID=?", true, db::get< unsigned long >( stmt, 1U ) );
                                break;

                            case upnp::didl::DIDL_CLASS::objectItemImageItemPhoto: {

                                std::string _album_art_title = db::get< const char* >( stmt, 2 );
                                boost::to_lower( _album_art_title );
                                std::vector< std::string > _cover_names = Config::instance()->coverNames();
                                if ( std::find( _cover_names.begin(), _cover_names.end(), _album_art_title ) != _cover_names.end() ) {
                                    if( _cover_list.add( db::get< int >( stmt, 0 ),
                                                         db::get< const char* >( stmt, 2 ),
                                                         db::get< int >( stmt, 7 ),
                                                         db::get< int >( stmt, 8 ) ) ) {
                                        photo_list_.push_back( db::get< int >( stmt, 0 ) );
                                        //update class and import flag
                                        db::exec( connection, "update tbl_cds_object set cls=?, title=?, import=? where ROWID=?",
                                            upnp::didl::DIDL_CLASS::objectAlbumArt, "front", true, db::get< int >( stmt, 0 ) );

                                    } else {
                                        //delete cover thumbnail
                                        db::exec( connection, "delete from tbl_cds_object where ROWID=?", db::get< int >( stmt, 0 ) );
                                    }
                                } else {
                                    photo_list_.push_back( db::get< int >( stmt, 0 ) );
                                    //update class and import flag
                                    db::exec( connection, "update tbl_cds_object set cls=?, import=? where ROWID=?",
                                        upnp::didl::DIDL_CLASS::objectAlbumArt, true, db::get< int >( stmt, 0 ) );
                                }
                                break;
                                }
                            default:
                                //TODO CLOG(DEBUG, "cds") << "unknown type: " << http::mime::mime_type( cls_ );
                                CLOG(DEBUG, "cds") << "unknown type: " << cls_;
                            }

                        }, clean_name_
                    );

                    //save album
                    const int album_id_ = dao::store< upnp::didl::DidlContainerAlbum >::run ( connection, "album::" + clean_name_,
                        (int)upnp::didl::DIDL_CLASS::objectContainerAlbumMusicAlbum,
                                "" /*parent_id*/, album_, "album::" + clean_name_, "" /*mime_type*/, 0 /*mtime*/, 1 /*object_update_id*/, profile_,
                                0 /*rating*/, year_, 0 /*playback_count*/, genre_, true );

                    //add songs to album
                    if( track_list_.empty() )
                    { std::cout << "emtpy tracklist" << std::endl; }
                    else {
                        dao::child< upnp::didl::DidlContainerArtist >( connection, [&connection,&album_id_] (auto statement) {
                            dao::mapping( connection, album_id_, db::get< int >( statement, upnp::didl::DidlContainerArtist::id ) );
                        }, track_list_.front() );
                        for( auto track : track_list_ )
                        { dao::mapping( connection, album_id_, track ); }
                    }
                    for( auto photo : photo_list_ )
                    { dao::mapping( connection, album_id_, photo ); }
                 }, upnp::didl::DIDL_CLASS::objectItemAudioItemMusicTrack
        );
    }
};

struct ParseImage {
    template< typename Connection >
    static void run( Connection connection, const std::string& filename, const std::string path, const std::string name,
                     const http::mime::MIME_TYPE& mime_type, const int filesize, const int mtime ) {

        image::Image image_meta_ ( filename );
        size_t _id = dao::store< upnp::didl::DidlPhoto >::run( connection, filename,
            upnp::didl::DIDL_CLASS::objectItemImageItemPhoto, path, name /*title*/,
            filename /*path*/, mime_type, mtime, 1 /*object_update_id*/, __Scanner::profile ( mime_type ), filesize,
            image_meta_.width() /*width*/, image_meta_.height() /*height*/,
            image_meta_.colorDepth() /*color_depth*/, false );

        // Create Thumbnail
        image_meta_.scale ( 768, 768, fmt::format( "{0}/albumArt/med_{1}.jpg", Config::instance()->tmpDirectory(), _id ) );
        image_meta_.scale ( 160, 160, fmt::format( "{0}/albumArt/tn_{1}.jpg", Config::instance()->tmpDirectory(), _id ) );
    }
};

struct ParseBook {
    template< typename Connection >
    static void run( Connection connection, const std::string& filename, const std::string path, const std::string name, const http::mime::MIME_TYPE& mime_type, const int filesize, const int mtime ) {
        std::cout << "parse book: " << filename << std::endl;
        if( mime_type == http::mime::PDF )
            dao::store< upnp::didl::DidlEBook >::run( connection, filename,
                upnp::didl::DIDL_CLASS::objectItemEBook, path, name /*title*/,
                filename /*path*/, mime_type, mtime, 1 /*object_update_id*/, __Scanner::profile( mime_type ),
                filesize, PdfParser::parsePdf ( filename ), 0, "", false );
        else if( mime_type == http::mime::EPUB )
            dao::store< upnp::didl::DidlEBook >::run( connection, filename,
                upnp::didl::DIDL_CLASS::objectItemEBook, path, name /*title*/,
                filename /*path*/, mime_type, mtime, 1 /*object_update_id*/, __Scanner::profile( mime_type ),
                filesize, epub::EpubParser::get_isbn( filename ), 0, "", false );
        else CLOG(ERROR, "cds") << "unknown mime-type: " << filename;
    }
};

struct ImportBook {
    template< typename Connection >
    static void run( Connection connection ) {
        db::exec( connection, "select ROWID, isbn, parent_id, path, mime_type, mtime, profile, size, title from tbl_cds_object where import = 0 and cls=?",
                  [&connection](auto statement) {
            std::cout << "import book: " << db::get< size_t >( statement, 0 ) << ":" << db::get< const char * >( statement, 1 ) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds( 1000 ) ); //wait not to overload...

            std::cout << "lookup on amazon" << std::endl;
                utils::BookResultItem _res = utils::AmazonFacade::bookByIsbn( "AKIAJL7OW25HI5DRKZJQ", "0TuOhtV6gitsfeWEbkmE4NYZcszvXH2W43rnf77R",
                                                                        db::get< const char * >( statement, 1 ) );
                if( _res.count == 0 || _res.status != 200 ) {
                    _res = utils::GoogleBooks::query( db::get< const char * >( statement, 8 ), db::get< const char * >( statement, 1 ) );
                }

            if( _res.status == 200 ) {

                std::cout << "ISBN result (google) is: " << _res.isbn << std::endl;
                dao::store< upnp::didl::DidlEBook >::run( connection, db::get< const char* >( statement, 3 ) /*filename*/, upnp::didl::DIDL_CLASS::objectItemEBook,
                                                     db::get< const char * >( statement, 2 ) /*parent_id*/,
                                                     _res.title /*title*/,
                                                     db::get< const char* >( statement, 3 ) /*path*/,
                                                     db::get< const char* >( statement, 4 ) /*mime_type*/,
                                                     db::get< size_t >( statement, 5 ) /*mtime*/,
                                                     2 /*object_update_id*/,
                                                     db::get< const char* >( statement, 6 ) /*profile*/,
                                                     db::get< unsigned long >( statement, 7 ) /*size*/,
                                                     _res.isbn, _res.publicationDate /*Year*/,
                                                     _res.description /*comment*/, 1 /*import*/ );
                //save author
                if ( _res.author.length() > 0 ) {
                    std::string _clean_name = __Scanner::clean_name( _res.author );
                    const int _author_id = dao::store< upnp::didl::DidlPersonAuthor>::run( connection, _clean_name,
                        upnp::didl::DIDL_CLASS::objectPersonAuthor, "" /*path*/, _res.author /*title*/,
                        _clean_name /*path*/, 0 /*mime_type*/, 0 /*mtime*/, 1 /*object_update_id*/, "" /*profile*/
                    );
                    dao::mapping( connection, db::get< size_t >( statement, 0 ), _author_id );
                }

                //save publisher
                if ( _res.publisher.length() > 0 ) {
                    std::string _clean_name = __Scanner::clean_name( _res.publisher );
                    const int _publisher_id = dao::store< upnp::didl::DidlPersonPublisher>::run( connection, _clean_name,
                        upnp::didl::DIDL_CLASS::objectPersonPublisher, "" /*path*/, _res.publisher /*title*/,
                        _clean_name /*path*/, 0 /*mime_type*/, 0 /*mtime*/, 1 /*object_update_id*/, "" /*profile*/
                    );
                    dao::mapping( connection, db::get< unsigned long >( statement, 0 ), _publisher_id );
                }

                //dowload cover
                if( ! _res.coverUri.empty() ) {
                    try {
                        std::stringstream _ss;
                        _ss << Config::instance()->tmpDirectory() << "/albumArt/" << _res.coverUri.substr( _res.coverUri.find_last_of( "/" ) );
                        std::ofstream _ofs( _ss.str().c_str(), std::ofstream::out );
                        std::cout << "donload cover from amazon: " << _res.coverUri << std::endl;
                        http::get( _res.coverUri, _ofs );

                        image::Image image_meta_ ( _ss.str() );
                        boost::filesystem::path _path( _ss.str() );
                        size_t _id = dao::store< upnp::didl::DidlAlbumArt>::run( connection, _ss.str(),
                            upnp::didl::DIDL_CLASS::objectAlbumArt, "", "front" /*title*/,
                            _ss.str() /*path*/, http::mime::JPEG, boost::filesystem::last_write_time ( _path ),
                            1 /*object_update_id*/, __Scanner::profile( http::mime::mime_type( image_meta_.mimeType() ) ) /*profile*/,
                            boost::filesystem::file_size( _path ) /*filesize*/,
                            image_meta_.width() /*width*/, image_meta_.height() /*height*/,
                            image_meta_.colorDepth() /*color_depth*/, true );

                        dao::mapping( connection, db::get< size_t >( statement, 0 ), _id );

                        // Create Thumbnail
                        image_meta_.scale ( 160, 160, fmt::format( "{0}/albumArt/tn_{1}.jpg", Config::instance()->tmpDirectory(), _id ) );
                    } catch( ... ) { //TODO
                        CLOG(ERROR, "cds") << "canot load image:" <<  _res.coverUri;
                    }
                }

            } else {
                std::cout << "error in book result: " << db::get< const char * >( statement, 1 ) << "::" << db::get< const char * >( statement, 1 ) << " (error:" <<
                             static_cast< int >( _res.status ) << ")" << std::endl;
                db::exec( connection, "update tbl_cds_object set import = 1 where ROWID=?", db::get< size_t >( statement, 0 ) );
            }

        }, upnp::didl::DIDL_CLASS::objectItemEBook );
    }
};

struct ImportSeries {
    template< typename Connection >
    static void run( Connection connection ) {

        db::exec( connection, "select ROWID, title, parent_id, path, mime_type, mtime, profile, size from tbl_cds_object where import = 0 and cls=? limit 0,1",
                  [&connection](auto statement) {
            CLOG(TRACE, "cds") << "import series." << db::get< size_t >( statement, 0 ) << ":" << db::get< const char * >( statement, 1 ) << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds( 500 ) );
            std::string _series_result = tmdb::Tmdb::series( Config::instance()->tmdb_api_key(), db::get< const char * >( statement, 1 ) );

            tmdb::Series _series;
            std::vector< tmdb::Series > _series_list = tmdb::Tmdb::parse_series( _series_result );
            if( _series_list.empty() ) {
                CLOG(WARNING, "cds") << "WARN: no tmdb entry found for: " << db::get< size_t >( statement, 0 ) << ":" << db::get< const char * >( statement, 1 ) << std::endl;
                return;
            } else if ( _series_list.size() == 1 ) {
                _series = _series_list.front();
            } else {
                for ( auto& _s : _series_list ) {
                    if( __Scanner::clean_name( _s.title ) == __Scanner::clean_name( db::get< const char * >( statement, 1 ) ) ) {
                        _series = _s;
                        break;
                    }
                    CLOG(WARNING, "cds") << "WARN: no exact match found for: " << db::get< size_t >( statement, 0 ) << ":" << db::get< const char * >( statement, 1 ) << std::endl;
                }
            }
            dao::store< upnp::didl::DidlContainerSeries >::run( connection, db::get< const char* >( statement, 3 ) /*filename*/, upnp::didl::DIDL_CLASS::objectContainerSeries,
                                                 db::get< const char * >( statement, 2 ) /*parent_id*/,
                                                 _series.title /*title*/,
                                                 db::get< const char* >( statement, 3 ) /*path*/,
                                                 db::get< const char* >( statement, 4 ) /*mime_type*/,
                                                 db::get< size_t >( statement, 5 ) /*mtime*/,
                                                 2 /*object_update_id*/,
                                                 db::get< const char* >( statement, 6 ) /*profile*/,
                                                 0 /*rating*/, _series.first_air_date /*year*/,
                                                 0 /*playback_count*/, _series.comment /*comment*/,
                                                 _series.tmdb_id, 1 );


            //dowload poster
            if ( ! _series.poster.empty() ) {
                std::stringstream _ss_poster;
                _ss_poster << Config::instance()->tmpDirectory() << "/albumArt/" << _series.poster.substr( _series.poster.find_last_of( "/" ) );
                tmdb::Tmdb::get( _series.poster, _ss_poster.str() );
                image::Image image_meta_poster_ ( _ss_poster.str() );
                boost::filesystem::path _path_poster( _ss_poster.str() );
                size_t _id_poster = dao::store< upnp::didl::DidlAlbumArt>::run( connection, _ss_poster.str(),
                    upnp::didl::DIDL_CLASS::objectAlbumArt, "", "poster" /*title*/,
                    _ss_poster.str() /*path*/, http::mime::JPEG, boost::filesystem::last_write_time ( _path_poster ),
                    1 /*object_update_id*/, __Scanner::profile( http::mime::mime_type( image_meta_poster_.mimeType() ) ) /*profile*/,
                    boost::filesystem::file_size( _path_poster ) /*filesize*/,
                    image_meta_poster_.width() /*width*/, image_meta_poster_.height() /*height*/,
                    image_meta_poster_.colorDepth() /*color_depth*/, true );
                // Create mapping
                dao::mapping( connection, db::get< size_t >( statement, 0 ), _id_poster );
                // Create Thumbnail
                image_meta_poster_.scale ( 160, 160, fmt::format( "{0}/albumArt/tn_{1}.jpg", Config::instance()->tmpDirectory(), _id_poster ) );
            }

            //dowload backdrop
            if ( ! _series.backdrop.empty() ) {
                std::stringstream _ss_backdrop;
                _ss_backdrop << Config::instance()->tmpDirectory() << "/albumArt/" << _series.backdrop.substr( _series.backdrop.find_last_of( "/" ) );
                tmdb::Tmdb::get( _series.backdrop, _ss_backdrop.str() );
                image::Image image_meta_backdrop_ ( _ss_backdrop.str() );
                boost::filesystem::path _path_backdrop( _ss_backdrop.str() );
                size_t _id_backdrop = dao::store< upnp::didl::DidlAlbumArt>::run( connection, _ss_backdrop.str(),
                    upnp::didl::DIDL_CLASS::objectAlbumArt, "", "backdrop" /*title*/,
                    _ss_backdrop.str() /*path*/, http::mime::JPEG, boost::filesystem::last_write_time ( _path_backdrop ),
                    1 /*object_update_id*/, __Scanner::profile( http::mime::mime_type( image_meta_backdrop_.mimeType() ) ) /*profile*/,
                    boost::filesystem::file_size( _path_backdrop ) /*filesize*/,
                    image_meta_backdrop_.width() /*width*/, image_meta_backdrop_.height() /*height*/,
                    image_meta_backdrop_.colorDepth() /*color_depth*/, true );
                // Create mapping
                dao::mapping( connection, db::get< size_t >( statement, 0 ), _id_backdrop );
                // Create Thumbnail
                image_meta_backdrop_.scale ( 160, 160, fmt::format( "{0}/albumArt/tn_{1}.jpg", Config::instance()->tmpDirectory(), _id_backdrop ) );
            }
        }, upnp::didl::DIDL_CLASS::objectContainerSeries );
    }
};

struct ParseVideo {
    template< typename Connection >
    static std::error_code run( Connection connection, const std::string& filename, const std::string path, const std::string name, const http::mime::MIME_TYPE& mime_type, const int filesize, const int mtime ) {
        CLOG(TRACE, "cds") << "import movie:" << filename;

        av::Format _format( filename );
        if( !!_format )
        { return _format.errc(); }
//        commons::media::MediaFile media_file = commons::media::MediaParser::parseFile ( filename );
        int width_ = 0, height_ = 0, bitrate_ = 0, color_depth_ = 0, duration_ = 0;
        for ( auto stream_ : _format ) {
            width_ = stream_->width();
            height_ = stream_->height();
            bitrate_ = stream_->bitrate();
            //TODO color_depth_ = stream_.
            //TODO duration_ = stream_.
        }

        int season_, episode_;
        std::string _series_name;
        bool _is_series = __Scanner::parse_series ( name, &season_, &episode_, &_series_name );


        if( _is_series ) {
            size_t _movie_id = dao::store< upnp::didl::DidlMovie >::run( connection, filename,
                upnp::didl::DIDL_CLASS::objectItemVideoItemEpisode, path, name /*title*/,
                filename /*path*/, mime_type, mtime, 1 /*object_update_id*/, "VIDEO" /*profile*/, filesize,
                season_, episode_, width_, height_, bitrate_, color_depth_, duration_, false );

            std::string _clean_name = __Scanner::clean_name( _series_name );
            const int _series_id = dao::store< upnp::didl::DidlContainerSeries>::run( connection, _clean_name,
                upnp::didl::DIDL_CLASS::objectContainerSeries, "" /*parent_id*/, _series_name /*title*/,
                _clean_name /*path*/, "" /*mime_type*/, 0 /*mtime*/, 1 /*object_update_id*/, "" /*profile*/,
                0 /*rating*/, 0 /*year*/, 0 /*playback_count*/, "" /*comment*/, 0 /*tmdb_id*/, false
            );
            dao::mapping( connection, _series_id, _movie_id );
        } else {
            /*TODO size_t _movie_id =*/ dao::store< upnp::didl::DidlMovie >::run( connection, filename,
                upnp::didl::DIDL_CLASS::objectItemVideoItemMovie, path, name /*title*/,
                filename /*path*/, mime_type, mtime, 1 /*object_update_id*/, "VIDEO" /*profile*/, filesize,
                season_, episode_, width_, height_, bitrate_, color_depth_, duration_, false );
        }
        return std::error_code ();
    }
};

struct ParseAudio {
    template< typename Connection >
    static std::error_code run( Connection connection, const std::string& filename, const std::string& path, const std::string& /*name*/, const http::mime::MIME_TYPE& mime_type, const int filesize, const int mtime ) {
        CLOG(TRACE, "cds") << "import audio:" << filename;

        //Get the track information
        av::Format _format( filename );
        if( !!_format )
        { return _format.errc(); }

        av::Metadata _metadata = _format.metadata();
        size_t track_number_ = 0;
        std::string str_track_ = _metadata.get ( av::Metadata::TRACK );
        if ( !str_track_.empty() && std::all_of ( str_track_.begin(), str_track_.end(), ::isdigit ) ) { //when is number
            track_number_ = std::stoi ( str_track_ );
        }

        unsigned long year_ = 0;
        std::string str_year_ = _metadata.get ( av::Metadata::YEAR );
        if ( !str_year_.empty() && std::all_of ( str_year_.begin(), str_year_.end(), ::isdigit ) ) { //when str_year is number
            year_ = upnp::didl::epoch_time ( std::stoi ( str_year_ ) );
        }

        size_t disc_ = 0;
        std::string str_disc_ = _metadata.get ( av::Metadata::DISC );
        if ( !str_disc_.empty() && std::all_of ( str_disc_.begin(), str_disc_.end(), ::isdigit ) ) { //when is number
            disc_ = std::stoi ( str_disc_ );
        }

        int bitrate_=0, bits_per_sample_=0, nr_audio_channels_=0, sample_frequency_=0;
        for ( auto stream_ : _format ) {
            bitrate_ = stream_->bitrate();
            bits_per_sample_ = stream_->bits_per_sample();
            nr_audio_channels_ = stream_->channels();
            sample_frequency_ = stream_->samplerate();
        }

        //save track
        const int track_ = dao::store< upnp::didl::DidlMusicTrack >::run( connection, filename,
            (int)upnp::didl::DIDL_CLASS::objectItemAudioItemMusicTrack, path, _metadata.get ( av::Metadata::TITLE ) /*title*/,
            filename /*path*/, (int)mime_type /*mime_type*/, mtime, 1 /*object_update_id*/, upnp::profile( _format ),

            0 /*rating*/, year_ /*year*/, 0 /*playback_count*/,
            _metadata.get ( av::Metadata::GENRE ) /*genre*/, false /*import*/,

            filesize, track_number_ /*track*/, disc_ /*disc*/,
            bitrate_, bits_per_sample_, nr_audio_channels_, _format.playtime(), sample_frequency_,
            _metadata.get ( av::Metadata::COMMENT ) /*comment*/,
            _metadata.get ( av::Metadata::ALBUM ) /*album*/
        );

        //save artist
        if ( ! _metadata.get ( av::Metadata::ARTIST ).empty() ) {
            std::string artist_ = _metadata.get ( av::Metadata::ARTIST );
            std::string clean_name_ = __Scanner::clean_name( artist_ );
            const int artist_id_ = dao::store< upnp::didl::DidlContainerArtist >::run( connection, clean_name_,
                (int)upnp::didl::DIDL_CLASS::objectContainerPersonMusicArtist, path, artist_ /*title*/,
                clean_name_ /*path*/, 0 /*mime_type*/, mtime, 1 /*object_update_id*/, ""
            );
            dao::mapping( connection, track_, artist_id_ );
        }

        //save composer
        if ( ! _metadata.get ( av::Metadata::COMPOSER ).empty() ) {
            std::string artist_ = _metadata.get ( av::Metadata::COMPOSER );
            std::string clean_name_ = __Scanner::clean_name( artist_ );
            const int artist_id_ = dao::store< upnp::didl::DidlContainerArtist >::run( connection, clean_name_,
                (int)upnp::didl::DIDL_CLASS::objectContainerPersonMusicArtist, path, artist_ /*title*/,
                clean_name_ /*path*/, 0 /*mime_type*/, mtime, 1 /*object_update_id*/, ""
            );
            dao::mapping( connection, track_, artist_id_ );
        }
        return std::error_code ();
    }
};

struct ParseFileRunner {
    template< typename Connection >
    static void run(Connection connection, auto* statistic, const std::string& directory ) {
        boost::filesystem::file_status directory_s_ = boost::filesystem::status( directory );
        if( ! boost::filesystem::exists( directory_s_ ) ) {
            std::stringstream _ss;
            _ss << "file does not exist: " << directory;
            throw std::invalid_argument(  _ss.str() );
        } else if ( ! boost::filesystem::is_directory ( directory_s_ ) ) {
            std::stringstream _ss;
            _ss << "not a directory: " << directory;
            throw std::invalid_argument(  _ss.str() );
        } else {

            boost::filesystem::path _fs_path ( directory );
            boost::filesystem::directory_iterator end_itr;
            for ( boost::filesystem::directory_iterator itr ( _fs_path ); itr != end_itr; ++itr ) {

                const std::string item_filepath_ = directory + "/" + itr->path().filename().string();
                const std::string item_stem_ = itr->path().stem().string();

                if ( boost::filesystem::is_regular_file ( itr->status() ) ) {

                    //Update the statistic
                    http::mime::MIME_TYPE mime_type_ = __Scanner::mime_type ( itr->path().extension().string() );
                    (*statistic)[mime_type_] = ( statistic->find ( mime_type_ ) != statistic->end() ?
                                                 statistic->find ( mime_type_ )->second + 1 : 1 );


                    if ( dao::touch ( connection, item_filepath_, boost::filesystem::last_write_time ( itr->path() ) ) == 0 ) {
                        std::error_code _errc;
                        //insert file in database.
                        switch( mime_type_ ) {
                        case http::mime::FLAC:
                        case http::mime::MPEG:
                            _errc = ParseAudio::run( connection, item_filepath_, directory, item_stem_, mime_type_,
                                                              boost::filesystem::file_size( itr->path() ),
                                                              boost::filesystem::last_write_time ( itr->path() ) );
                            break;
                        case http::mime::AVI:
                        case http::mime::MP4:
                        case http::mime::MKV:
                            _errc = ParseVideo::run( connection, item_filepath_, directory, item_stem_, mime_type_,
                                                              boost::filesystem::file_size( itr->path() ),
                                                              boost::filesystem::last_write_time ( itr->path() ) );
                            break;
                        case http::mime::PNG:
                        case http::mime::JPEG:
                            ParseImage::run( connection, item_filepath_, directory, item_stem_, mime_type_,
                                                              boost::filesystem::file_size( itr->path() ),
                                                              boost::filesystem::last_write_time ( itr->path() ) );
                            break;
                        case http::mime::PDF:
                        case http::mime::EPUB:
                            ParseBook::run( connection, item_filepath_, directory, item_stem_, mime_type_,
                                                             boost::filesystem::file_size( itr->path() ),
                                                             boost::filesystem::last_write_time ( itr->path() ) );
                            break;
                        default:
                            CLOG(DEBUG, "cds") << "unknown filetype." << item_filepath_;
                            break;
                        }
                    if ( _errc )
                    { CLOG(DEBUG, "cds") << "error parse file: " << item_filepath_  << "(" << _errc.message() << ")"; }

                    } //else file not updated.

                } else if ( boost::filesystem::is_directory ( itr->status() ) ) {
                    ParseFileRunner::run ( connection, statistic, item_filepath_ );
                }
            }
        }
    }
};

void Scanner::parse ( const std::vector< std::string >& directories ) throw(std::invalid_argument) {

    auto db_ = db::make_connection( Config::instance()->databaseFile() );
    long _start_time = std::time ( 0 );
    auto start_ = std::chrono::system_clock::now();
    std::map<http::mime::MIME_TYPE, int> statistic;

    db::transaction_start( db_ );
    for( auto directory : directories )
    { ParseFileRunner::run( db_, &statistic, directory ); }
    sweep( db_, _start_time );

    ParseAlbums::run( db_ );
    ImportBook::run( db_ );
    ImportSeries::run( db_ );


    db::transaction_end( db_ );

    //output statistic
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start_;

    std::stringstream ss;
    int sum = 0;
    ss << std::endl << "***************************************************" << std::endl;

    for ( auto iter : statistic ) {
        ss << http::mime::mime_type( iter.first ) << "\t" << iter.second << std::endl;
        sum += iter.second;
    }

    ss << "Total:\t" << sum << std::endl;
    ss << "Time:\t" << elapsed_seconds.count() << std::endl;
    CLOG(INFO, "cds") << ss.str();
    //TODO signal_detected.emit();
}

}//namespace squawk
