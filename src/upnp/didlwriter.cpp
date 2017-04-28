#include "didlwriter.h"

#include "../dao.h"
#include "../config.h"

#include "http/mimetypes.h"

#include "fmt/format.h"
#include "fmt/time.h"

namespace upnp {

const char* DidlWriter::PREFIX_ALBUM_ID = "Albums$";
const char* DidlWriter::PREFIX_ARTIST_ID = "Artists$";
const char* DidlWriter::PREFIX_SERIE_ID = "Series$";

DidlWriter::DidlWriter( commons::xml::XMLWriter * xml_writer, db::sqlite3_ptr db, const std::string& parent ) : xml_writer_( xml_writer ), db_( db ), parent_( parent ),
    didl_element_ ( xml_writer_->element ( didl::DIDL_ROOT_NODE ) ) {
    xml_writer_->ns ( didl_element_, upnp::XML_NS_DIDL );
    xml_writer_->ns ( didl_element_, upnp::XML_NS_PURL, "dc" );
    xml_writer_->ns ( didl_element_, upnp::XML_NS_DLNA, "dlna" );
    xml_writer_->ns ( didl_element_, upnp::XML_NS_UPNP, "upnp" );
    xml_writer_->ns ( didl_element_, upnp::XML_NS_PV, "pv" );
}

void DidlWriter::node( const std::unordered_map< std::string, didl::DIDL_CLASS >& root_nodes ) {
    for( auto _node : root_nodes ) {
        commons::xml::Node container_element = xml_writer_->element ( didl_element_, "", didl::DIDL_ELEMENT_CONTAINER, "" );
        xml_writer_->attribute ( container_element, didl::DIDL_ATTRIBUTE_ID, _node.first );
        xml_writer_->attribute ( container_element, didl::DIDL_ATTRIBUTE_PARENT_ID, "/" );
        xml_writer_->attribute ( container_element, didl::DIDL_ATTRIBUTE_RESTRICTED, "1" );
        xml_writer_->attribute ( container_element, didl::DIDL_ATTRIBUTE_CHILD_COUNT, std::to_string ( dao::count( db_, _node.second ) ) );
        xml_writer_->element ( container_element, upnp::XML_NS_PURL, didl::DIDL_ELEMENT_TITLE, _node.first );
        xml_writer_->element ( container_element, upnp::XML_NS_UPNP, didl::DIDL_ELEMENT_CLASS, didl::className( didl::DIDL_CLASS::objectContainerStorageFolder ) );
        count_+=1;
    }
}

void DidlWriter::write(db::sqlite3_stmt_ptr statement) {
    if( db::get< didl::DIDL_CLASS >( statement, didl::DidlObject::cls ) == didl::DIDL_CLASS::objectContainer ) {
        std::cout << "objectContainer" << std::endl;

    } else if( db::get< didl::DIDL_CLASS >( statement, didl::DidlObject::cls ) == didl::DIDL_CLASS::objectContainerAlbumMusicAlbum ) {
        commons::xml::Node container_element = xml_writer_->element ( didl_element_, "", didl::DIDL_ELEMENT_CONTAINER, "" );
        xml_writer_->attribute ( container_element, didl::DIDL_ATTRIBUTE_ID, fmt::format( "{0}{1}", DidlWriter::PREFIX_ALBUM_ID, db::get< size_t >( statement, didl::DidlContainerAlbum::id ) ) );
        xml_writer_->attribute ( container_element, PARENT_ID, parent_ );
        xml_writer_->attribute ( container_element, didl::DIDL_ATTRIBUTE_RESTRICTED, "1" );
        xml_writer_->attribute ( container_element, didl::DIDL_ATTRIBUTE_CHILD_COUNT,
                                 std::to_string( dao::count< didl::DidlMusicTrack >( db_, db::get< size_t >( statement, didl::DidlContainerAlbum::id ) ) ) );

        xml_writer_->element ( container_element, upnp::XML_NS_PURL, didl::DIDL_ELEMENT_TITLE, db::get< const char* >( statement, didl::DidlContainerAlbum::title ) );
        xml_writer_->element ( container_element, upnp::XML_NS_UPNP, didl::DIDL_ELEMENT_CLASS, className ( db::get< didl::DIDL_CLASS >( statement, didl::DidlContainerAlbum::cls ) ) );

        //get the artists
        dao::child< didl::DidlContainerArtist >( db_, [this, &container_element](auto statement) {
            xml_writer_->element ( container_element, upnp::XML_NS_UPNP, "artist", db::get< const char * >( statement, didl::DidlContainerArtist::title ) );
        }, db::get< size_t >( statement, didl::DidlContainerAlbum::id ) );

        std::time_t time_ = db::get< long >( statement, didl::DidlContainerAlbum::year );
        xml_writer_->element ( container_element, upnp::XML_NS_PURL, "date", fmt::format("{:%Y-%m-%d}", *std::localtime( &time_ ) ) );

        //get the cover
        dao::child< didl::DidlAlbumArt >( db_, "title='front'", [this, &container_element](auto statement) {
            commons::xml::Node dlna_album_art_node =
                xml_writer_->element ( container_element, upnp::XML_NS_UPNP, didl::DIDL_ALBUM_ART_URI,
                                       fmt::format( "http://{0}:{1}/albumArt/tn_{2}.jpg",
                                                    squawk::Config::instance()->httpHost(),
                                                    squawk::Config::instance()->httpPort(),
                                                    db::get< size_t >( statement, didl::DidlContainerAlbum::id ) ) );
            xml_writer_->ns ( dlna_album_art_node, upnp::XML_NS_DLNA_METADATA, "dlna", false );
            xml_writer_->attribute ( dlna_album_art_node, upnp::XML_NS_DLNA_METADATA, dlna::DLNA_PROFILE_ID, dlna::DLNA_PROFILE_JPEG_TN );
        }, db::get< int >( statement, didl::DidlContainerAlbum::id ) );
        count_+=1;

    } else if( db::get< didl::DIDL_CLASS >( statement, didl::DidlObject::cls ) == didl::DIDL_CLASS::objectAlbumArt ) {
        std::cout << "objectAlbumArt" << std::endl; //TODO should not be

    } else if( db::get< didl::DIDL_CLASS >( statement, didl::DidlObject::cls ) == didl::DIDL_CLASS::objectContainerAlbumPhotoAlbum ) {
        std::cout << "objectContainerAlbumPhotoAlbum" << std::endl; //TODO does photo album exist.





    } else if( db::get< didl::DIDL_CLASS >( statement, didl::DidlObject::cls ) == didl::DIDL_CLASS::objectContainerPersonMusicArtist ) {
        commons::xml::Node container_element = xml_writer_->element ( didl_element_, "", didl::DIDL_ELEMENT_CONTAINER, "" );
        xml_writer_->attribute ( container_element, didl::DIDL_ATTRIBUTE_ID, fmt::format( "Artists${}", db::get< int >( statement, didl::DidlContainerAlbum::id ) ) );
        xml_writer_->attribute ( container_element, PARENT_ID, parent_ );
        xml_writer_->attribute ( container_element, didl::DIDL_ATTRIBUTE_RESTRICTED, "1" );
        xml_writer_->attribute ( container_element, didl::DIDL_ATTRIBUTE_CHILD_COUNT,
                                 std::to_string( dao::count_artist_childs( db_, db::get< size_t >( statement, didl::DidlContainerAlbum::id ) ) ) );

        xml_writer_->element ( container_element, upnp::XML_NS_PURL, "title", db::get< const char* >( statement, didl::DidlContainerAlbum::title ) );
        xml_writer_->element ( container_element, upnp::XML_NS_UPNP, "class", className ( db::get< didl::DIDL_CLASS >( statement, didl::DidlContainerAlbum::cls ) ) );
        count_+=1;

    } else if( db::get< didl::DIDL_CLASS >( statement, didl::DidlObject::cls ) == didl::DIDL_CLASS::objectItemAudioItemMusicTrack ) {
        commons::xml::Node item_element_ = xml_writer_->element ( didl_element_, "", "item", "" );
        xml_writer_->attribute ( item_element_, didl::DIDL_ATTRIBUTE_ID, fmt::format( "track${}", db::get< size_t >( statement, didl::DidlMusicTrack::id ) ) );
        xml_writer_->attribute ( item_element_, PARENT_ID, parent_ );
        xml_writer_->attribute ( item_element_, didl::DIDL_ATTRIBUTE_RESTRICTED, "1" );
        xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "class", className ( db::get< didl::DIDL_CLASS >( statement, didl::DidlMusicTrack::cls ) ) );
        xml_writer_->element ( item_element_, upnp::XML_NS_PURL, "title", db::get< const char * >( statement, didl::DidlMusicTrack::title ) );

        xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "originalTrackNumber", std::to_string( db::get< int >( statement, didl::DidlMusicTrack::track ) ) );
        xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "album", db::get< const char * >( statement, didl::DidlMusicTrack::album ) );
        xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "description", db::get< const char * >( statement, didl::DidlMusicTrack::comment ) );

        std::time_t time_ = db::get< long >( statement, didl::DidlMusicTrack::year );
        xml_writer_->element ( item_element_, upnp::XML_NS_PURL, "date", fmt::format("{:%Y-%m-%d}", *std::localtime( &time_ ) ) );
        xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "genre", db::get< const char * >( statement, didl::DidlMusicTrack::genre ) );

        //get the artists
        dao::child< didl::DidlContainerArtist >( db_, [this, &item_element_](auto statement) {
            xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "artist", db::get< const char * >( statement, didl::DidlContainerArtist::title ) );
            //TODO            xml_writer_->element ( container_element, upnp::XML_NS_PURL, "creator", container.contributor() );
        }, db::get< int >( statement, didl::DidlMusicTrack::id ) );

//TODO        std::time_t last_playback_time_ = db::get< const char * >( statement, didl::_DidlMusicTrack::playback_time );
//        xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "lastPlaybackTime", fmt::format("{:%Y-%m-%d %h:%m:%s}", *std::localtime( &last_playback_time_ ) ) );
        xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "playbackCount", std::to_string( db::get< int >( statement, didl::DidlMusicTrack::playback_count ) ) );
        xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "rating", std::to_string( db::get< int >( statement, didl::DidlMusicTrack::rating ) ) );

        //write track resources
        commons::xml::Node dlna_res_node = xml_writer_->element ( item_element_, "", "res",
            fmt::format( "http://{0}:{1}/res/{2}.{3}",
                squawk::Config::instance()->httpHost(),
                squawk::Config::instance()->httpPort(),
                db::get< const char* >( statement, didl::DidlMusicTrack::id ),
                http::mime::extension( db::get< http::mime::MIME_TYPE >( statement, didl::DidlMusicTrack::mime_type ) ) ).c_str() );
        xml_writer_->attribute ( dlna_res_node, "", "protocolInfo",
                                fmt::format( "http-get:*:{}:DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000",
                                             http::mime::mime_type( db::get< http::mime::MIME_TYPE >( statement, didl::DidlMusicTrack::mime_type ) ) ) ); //TODO fix mime type

        xml_writer_->attribute ( dlna_res_node, "", "size", std::to_string( db::get< int >( statement, didl::DidlMusicTrack::size ) ) );
        xml_writer_->attribute ( dlna_res_node, "", "dlnaProfile", db::get< const char * >( statement, didl::DidlMusicTrack::profile ) );
        xml_writer_->attribute ( dlna_res_node, "", "mime-type", http::mime::mime_type( db::get< http::mime::MIME_TYPE >( statement, didl::DidlMusicTrack::mime_type ) ) );
        xml_writer_->attribute ( dlna_res_node, "", "duration", std::to_string( db::get< int >( statement, didl::DidlMusicTrack::duration ) ) );
        if( size_t _bitrate = db::get< int >( statement, didl::DidlMusicTrack::bitrate ) )
        { xml_writer_->attribute ( dlna_res_node, "", "bitrate", std::to_string( _bitrate ) ); }
        if( size_t _bits_per_sample = db::get< int >( statement, didl::DidlMusicTrack::bits_per_sample ) )
        { xml_writer_->attribute ( dlna_res_node, "", "bitsPerSample", std::to_string( _bits_per_sample ) ); }
        if( size_t _sample_frequency = db::get< int >( statement, didl::DidlMusicTrack::sample_frequency ) )
        { xml_writer_->attribute ( dlna_res_node, "", "sampleFrequency", std::to_string( _sample_frequency ) ); }
        if( size_t _nr_audio_channels = db::get< int >( statement, didl::DidlMusicTrack::nr_audio_channels ) )
        { xml_writer_->attribute ( dlna_res_node, "", "nrAudioChannels", std::to_string( _nr_audio_channels ) ); }
//TODO Use for Video and Images        xml_writer_->attribute ( dlna_res_node, "", "resolution", attr.second ); break;
//        xml_writer_->attribute ( dlna_res_node, "", "colorDepth", attr.second ); break;
//        xml_writer_->attribute ( dlna_res_node, "", "framerate", attr.second ); break;

        //get the cover //TODO larget cover
        dao::child< didl::DidlAlbumArt >( db_, "title='front'", [this, &item_element_](auto statement) {
            commons::xml::Node dlna_album_art_node =
                xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, didl::DIDL_ALBUM_ART_URI,
                                       fmt::format( "http://{0}:{1}/albumArt/med_{2}.jpg",
                                                    squawk::Config::instance()->httpHost(),
                                                    squawk::Config::instance()->httpPort(),
                                                    db::get< size_t >( statement, didl::DidlAlbumArt::id ) ) );
            xml_writer_->ns ( dlna_album_art_node, upnp::XML_NS_DLNA_METADATA, "dlna", false );
            xml_writer_->attribute ( dlna_album_art_node, upnp::XML_NS_DLNA_METADATA, dlna::DLNA_PROFILE_ID, dlna::DLNA_PROFILE_JPEG_TN );
        }, dao::parent( db_, db::get< int >( statement, didl::DidlMusicTrack::id ) ) );
        count_+=1;

    } else if( db::get< didl::DIDL_CLASS >( statement, didl::DidlObject::cls ) == didl::DIDL_CLASS::objectItemImageItemPhoto ) {
        commons::xml::Node item_element_ = xml_writer_->element ( didl_element_, "", "item", "" );
        xml_writer_->attribute ( item_element_, didl::DIDL_ATTRIBUTE_ID, fmt::format( "photo${}", db::get< size_t >( statement, didl::DidlPhoto::id ) ) );
        xml_writer_->attribute ( item_element_, PARENT_ID, parent_ );
        xml_writer_->attribute ( item_element_, didl::DIDL_ATTRIBUTE_RESTRICTED, "1" );
        xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "class", className ( db::get< didl::DIDL_CLASS >( statement, didl::DidlPhoto::cls ) ) );
        xml_writer_->element ( item_element_, upnp::XML_NS_PURL, "title", db::get< const char* >( statement, didl::DidlPhoto::title ) );

        const std::string _mime_type = http::mime::mime_type ( db::get< http::mime::MIME_TYPE >( statement, didl::DidlPhoto::mime_type ) );
        commons::xml::Node dlna_res_node = xml_writer_->element ( item_element_, "", "res",
            fmt::format( "http://{0}:{1}/res/{2}.{3}",
                squawk::Config::instance()->httpHost(),
                squawk::Config::instance()->httpPort(),
                db::get< const char* >( statement, didl::DidlPhoto::id ),
                http::mime::extension( db::get< http::mime::MIME_TYPE >( statement, didl::DidlPhoto::mime_type ) ) ).c_str() );
        xml_writer_->attribute ( dlna_res_node, "", "protocolInfo",
                                fmt::format( "http-get:*:{}:DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000", _mime_type ) ); //TODO fix mime type

        xml_writer_->attribute ( dlna_res_node, "", "size", std::to_string( db::get< size_t >( statement, didl::DidlPhoto::size ) ) );
        //TODO xml_writer_->attribute ( dlna_res_node, "", "dlnaProfile", item.dlnaProfile() );
        xml_writer_->attribute ( dlna_res_node, "", "mime-type", _mime_type );

        if( size_t _color_depth = db::get< int >( statement, didl::DidlPhoto::color_depth ) )
        { xml_writer_->attribute ( dlna_res_node, "", "colorDepth", std::to_string( _color_depth ) ); }
        xml_writer_->attribute ( dlna_res_node, "", "resolution", fmt::format( "{0}x{1}",
                db::get< int >( statement, didl::DidlPhoto::width ),  db::get< int >( statement, didl::DidlPhoto::height ) ) );
        count_+=1;

    } else if( db::get< didl::DIDL_CLASS >( statement, didl::DidlObject::cls ) == didl::DIDL_CLASS::objectContainerSeries ) {
        std::cout << "didl::DIDL_CLASS::objectContainerSeries" << std::endl;
        commons::xml::Node container_element = xml_writer_->element ( didl_element_, "", didl::DIDL_ELEMENT_CONTAINER, "" );
        xml_writer_->attribute ( container_element, didl::DIDL_ATTRIBUTE_ID, fmt::format( "Series${}", db::get< int >( statement, didl::DidlContainerSeries::id ) ) );
        xml_writer_->attribute ( container_element, PARENT_ID, parent_ );
        xml_writer_->attribute ( container_element, didl::DIDL_ATTRIBUTE_RESTRICTED, "1" );
        //TODO xml_writer_->attribute ( container_element, didl::DIDL_ATTRIBUTE_CHILD_COUNT, std::to_string ( container.childCount() ) );

        xml_writer_->element ( container_element, upnp::XML_NS_PURL, "title", db::get< const char* >( statement, didl::DidlContainerSeries::title ) );
        xml_writer_->element ( container_element, upnp::XML_NS_UPNP, "class", className ( db::get< didl::DIDL_CLASS >( statement, didl::DidlContainerSeries::cls ) ) );
        count_+=1;

    } else if( db::get< didl::DIDL_CLASS >( statement, didl::DidlObject::cls ) == didl::DIDL_CLASS::objectItemVideoItemMovie ||
               db::get< didl::DIDL_CLASS >( statement, didl::DidlObject::cls ) == didl::DIDL_CLASS::objectItemVideoItemEpisode ) {
        std::cout << "didl::DIDL_CLASS::objectItemVideoItemMovie" << std::endl;
        commons::xml::Node item_element_ = xml_writer_->element ( didl_element_, "", "item", "" );
        xml_writer_->attribute ( item_element_, didl::DIDL_ATTRIBUTE_ID, fmt::format( "Movie${}", db::get< int >( statement, didl::DidlMovie::id ) ) );
        xml_writer_->attribute ( item_element_, PARENT_ID, parent_ );
        xml_writer_->attribute ( item_element_, didl::DIDL_ATTRIBUTE_RESTRICTED, "1" );
//        xml_writer_->attribute ( item_element_, didl::DIDL_ATTRIBUTE_CHILD_COUNT,
//                                 std::to_string( dao::count_artist_childs( db_, db::get< size_t >( statement, didl::_DidlContainerAlbum::id ) ) ) );

        xml_writer_->element ( item_element_, upnp::XML_NS_PURL, "title", db::get< const char* >( statement, didl::DidlMovie::title ) );
        xml_writer_->element ( item_element_, upnp::XML_NS_UPNP, "class", className ( db::get< didl::DIDL_CLASS >( statement, didl::DidlMovie::cls ) ) );

//        std::time_t time_ = db::get< unsigned long >( statement, didl::_DidlMovie::year );
//        xml_writer_->element ( item_element_, upnp::XML_NS_PURL, "date", fmt::format("{:%Y-%m-%d}", *std::localtime( &time_ ) ) );

        commons::xml::Node dlna_res_node = xml_writer_->element ( item_element_, "", "res",
            fmt::format( "http://{0}:{1}/res/{2}.{3}",
                squawk::Config::instance()->httpHost(),
                squawk::Config::instance()->httpPort(),
                db::get< const char* >( statement, didl::DidlMovie::id ),
                http::mime::extension( db::get< http::mime::MIME_TYPE >( statement, didl::DidlMovie::mime_type ) ) ).c_str() );
        xml_writer_->attribute ( dlna_res_node, "", "protocolInfo",
                                fmt::format( "http-get:*:{}:DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000",
                                             http::mime::mime_type ( db::get< http::mime::MIME_TYPE >( statement, didl::DidlMovie::mime_type ) ) ) ); //TODO fix mime type

        xml_writer_->attribute ( dlna_res_node, "", "size", std::to_string( db::get< size_t >( statement, didl::DidlMovie::size ) ) );
        //TODO xml_writer_->attribute ( dlna_res_node, "", "dlnaProfile", item.dlnaProfile() );
        xml_writer_->attribute ( dlna_res_node, "", "mime-type", http::mime::mime_type ( db::get< http::mime::MIME_TYPE >( statement, didl::DidlMovie::mime_type ) ) );

        xml_writer_->attribute ( dlna_res_node, "", "duration", std::to_string( db::get< int >( statement, didl::DidlMovie::duration ) ) );
        if( size_t _bitrate = db::get< int >( statement, didl::DidlMovie::bitrate ) )
        { xml_writer_->attribute ( dlna_res_node, "", "bitrate", std::to_string( _bitrate ) ); }
//TODO        if( size_t _bits_per_sample = db::get< int >( statement, didl::_DidlMovie::bits_per_sample ) )
//        { xml_writer_->attribute ( dlna_res_node, "", "bitsPerSample", std::to_string( _bits_per_sample ) ); }
//        if( size_t _sample_frequency = db::get< int >( statement, didl::_DidlMovie::sample_frequency ) )
//        { xml_writer_->attribute ( dlna_res_node, "", "sampleFrequency", std::to_string( _sample_frequency ) ); }
//        if( size_t _nr_audio_channels = db::get< int >( statement, didl::_DidlMovie::nr_audio_channels ) )
//        { xml_writer_->attribute ( dlna_res_node, "", "nrAudioChannels", std::to_string( _nr_audio_channels ) ); }
        if( size_t _color_depth = db::get< int >( statement, didl::DidlMovie::color_depth ) )
        { xml_writer_->attribute ( dlna_res_node, "", "colorDepth", std::to_string( _color_depth ) ); }
//TODO        if( size_t _nr_audio_channels = db::get< int >( statement, didl::_DidlMovie::framerate ) )
//        { xml_writer_->attribute ( dlna_res_node, "", "framerate", attr.second ); }

        xml_writer_->attribute ( dlna_res_node, "", "resolution", fmt::format( "{0}x{1}",
                db::get< int >( statement, didl::DidlMovie::width ),  db::get< int >( statement, didl::DidlMovie::height ) ) );
        count_+=1;
    }
}
std::string DidlWriter::http_uri ( const std::string& suffix ) {
    return fmt::format( "http://{0}:{1}/{2}", squawk::Config::instance()->httpHost(), squawk::Config::instance()->httpPort(), suffix );
}
}//namespace upnp
