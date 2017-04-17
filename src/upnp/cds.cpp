#include "cds.h"

#include "easylogging++.h"

#include "http/mimetypes.h"

#include "didlwriter.h"

#include "../config.h"
#include "../dao.h"
#include "../database.h"

namespace upnp {

const std::unordered_map< std::string, didl::DIDL_CLASS > Cds::root_nodes_
    {
        { "Photos", didl::DIDL_CLASS::objectItemImageItemPhoto },
        { "Series", didl::DIDL_CLASS::objectContainerSeries },
        { "Videos", didl::DIDL_CLASS::objectItemVideoItemMovie },
        { "Artists", didl::DIDL_CLASS::objectContainerPersonMusicArtist },
        { "Albums", didl::DIDL_CLASS::objectContainerAlbumMusicAlbum  },
    };

http::http_status Cds::do_subscribe ( http::Request& request, http::Response& ) {
    CLOG(DEBUG, "upnp") << "subscribe:" << request.str();
    return http::http_status::OK;
}

http::http_status Cds::do_post ( http::Request& request, http::Response& response ) {
    try {
        upnp::UpnpContentDirectoryRequest upnp_command = upnp::parseRequest ( request.str() );
        CLOG(DEBUG, "upnp") << "UpnpRequest:" << std::endl << upnp_command;
        commons::xml::XMLWriter xmlWriter;

        if ( upnp_command.type == upnp::UpnpContentDirectoryRequest::BROWSE ) {
            browse ( &xmlWriter, &upnp_command );
            response << xmlWriter.str();

        } else if ( upnp_command.type == upnp::UpnpContentDirectoryRequest::X_FEATURE_LIST ) {
            browse ( &xmlWriter, &upnp_command );
            response <<  xmlWriter.str();

        } else {
            CLOG(ERROR, "upnp") << "invoke::Unknown Method: " << upnp_command;
            return http::http_status::BAD_REQUEST;
        }

    } catch ( upnp::UpnpException & ex ) {
        CLOG(ERROR, "upnp") << "UPNP parse error: " << ex.code() << ":" << ex.what();
        return http::http_status::INTERNAL_SERVER_ERROR;

    } catch ( db::DbException & ex ) {
        CLOG(ERROR, "upnp") << "DB Exception: " << ex.code() << ":" << ex.what();
        return http::http_status::INTERNAL_SERVER_ERROR;

    } catch ( commons::xml::XmlException & ex ) {
        CLOG(ERROR, "upnp") << "XML parse error: " << ex.code() << ":" << ex.what();
        return http::http_status::INTERNAL_SERVER_ERROR;

    } catch ( std::invalid_argument & e ) {
        CLOG(ERROR, "upnp") << "UPNP parse error:" << e.what();
        return http::http_status::INTERNAL_SERVER_ERROR;

    } catch ( ... ) {
        CLOG(ERROR, "upnp") << "UPNP parse error.";
        return http::http_status::INTERNAL_SERVER_ERROR;
    }
    response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
    return http::http_status::OK;
}

void Cds::feature_list ( commons::xml::XMLWriter* xmlWriter, upnp::UpnpContentDirectoryRequest* /*upnp_command*/ ) {

    commons::xml::Node envelope_node = soap::make_soap_envelope ( xmlWriter );
    commons::xml::Node body_node = xmlWriter->element ( envelope_node, soap::XML_NS_SOAP, soap::XML_RESULT_BODY );
    commons::xml::Node response_node = xmlWriter->element ( body_node, "", "X_GetFeatureListResponse", "" );
    xmlWriter->ns ( response_node, upnp::XML_NS_UPNP_CDS, "u", true );

    commons::xml::XMLWriter didlWriter;
    commons::xml::Node features_node = didlWriter.element ( "Features" );
    xmlWriter->ns ( features_node, "urn:schemas-upnp-org:av:avs", "", true );
    xmlWriter->ns ( features_node, "http://www.w3.org/2001/XMLSchema-instance", "xsi" );
    xmlWriter->attribute ( features_node, "http://www.w3.org/2001/XMLSchema-instance", "encodingStyle", "urn:schemas-upnp-org:av:avs http://www.upnp.org/schemas/av/avs.xsd" );

    commons::xml::Node feature_node = didlWriter.element ( features_node, "", "Feature" );
    xmlWriter->attribute ( feature_node, "", "name", "samsung.com_BASICVIEW" );
    xmlWriter->attribute ( feature_node, "", "version", "1" );

    commons::xml::Node audio_node = didlWriter.element ( feature_node, "", didl::DIDL_ELEMENT_CONTAINER );
    xmlWriter->attribute ( audio_node, "", didl::DIDL_ATTRIBUTE_ID, "music" );
    xmlWriter->attribute ( audio_node, "", "type", "object.item.audioItem" );

    commons::xml::Node video_node = didlWriter.element ( feature_node, "", didl::DIDL_ELEMENT_CONTAINER );
    xmlWriter->attribute ( video_node, "", didl::DIDL_ATTRIBUTE_ID, "video" );
    xmlWriter->attribute ( video_node, "", "type", "object.item.videoItem" );

    commons::xml::Node image_node = didlWriter.element ( feature_node, "", didl::DIDL_ELEMENT_CONTAINER );
    xmlWriter->attribute ( image_node, "", didl::DIDL_ATTRIBUTE_ID, "image" );
    xmlWriter->attribute ( image_node, "", "type", "object.item.imageItem" );

    xmlWriter->element ( response_node, "", "FeatureList", didlWriter.str() );
}

void Cds::browse ( commons::xml::XMLWriter* xmlWriter, upnp::UpnpContentDirectoryRequest* upnp_command ) {

    auto db_ = db::make_connection( squawk::Config::instance()->databaseFile() );
    commons::xml::Node envelope_node = soap::make_soap_envelope ( xmlWriter );

    commons::xml::Node body_node = xmlWriter->element ( envelope_node, soap::XML_NS_SOAP, soap::XML_RESULT_BODY );
    commons::xml::Node response_node = xmlWriter->element ( body_node, "", BROWSE_RESPONSE, "" );
    xmlWriter->ns ( response_node, upnp::XML_NS_UPNP_CDS, "u", true );

    commons::xml::XMLWriter didlWriter;
    std::tuple< size_t, size_t > res_;

    // browse metadata
    if ( upnp_command->contains ( BROWSE_FLAG ) &&
         upnp_command->getValue ( BROWSE_FLAG ) == BROWSE_METADATA ) {

        //TODO also get for item by id
        commons::xml::Node didl_element = didlWriter.element ( upnp::didl::DIDL_ROOT_NODE );
        didlWriter.ns ( didl_element, upnp::XML_NS_DIDL );
        didlWriter.ns ( didl_element, upnp::XML_NS_PURL, "dc" );
        didlWriter.ns ( didl_element, upnp::XML_NS_DLNA, "dlna" );
        didlWriter.ns ( didl_element, upnp::XML_NS_UPNP, "upnp" );
        didlWriter.ns ( didl_element, upnp::XML_NS_PV, "pv" );

        commons::xml::Node container_element = didlWriter.element ( didl_element, "", didl::DIDL_ELEMENT_CONTAINER, "" );
        didlWriter.attribute ( container_element, didl::DIDL_ATTRIBUTE_ID, "0" );
        didlWriter.attribute ( container_element, "parentID", "-1" );
        didlWriter.attribute ( container_element, didl::DIDL_ATTRIBUTE_RESTRICTED, "1" );
        didlWriter.attribute ( container_element, didl::DIDL_ATTRIBUTE_CHILD_COUNT, "3" );
        didlWriter.attribute ( container_element, "searchable", "1" );

        commons::xml::Node audio_element = didlWriter.element ( container_element, upnp::XML_NS_UPNP, "searchClass", "object.item.audioItem" );
        didlWriter.attribute ( audio_element, "includeDerived", "1" );
        commons::xml::Node image_element = didlWriter.element ( container_element, upnp::XML_NS_UPNP, "searchClass", "object.item.imageItem" );
        didlWriter.attribute ( image_element, "includeDerived", "1" );
        commons::xml::Node video_element = didlWriter.element ( container_element, upnp::XML_NS_UPNP, "searchClass", "object.item.videoItem" );
        didlWriter.attribute ( video_element, "includeDerived", "1" );
        didlWriter.element ( container_element, upnp::XML_NS_PURL, "title", "root" );
        didlWriter.element ( container_element, upnp::XML_NS_UPNP, "class", "object.container.storageFolder" );
        didlWriter.element ( container_element, upnp::XML_NS_UPNP, "storageUsed", "-1" );

        res_ = std::tuple< size_t, size_t > ( 3, 3 );

    } else if ( upnp_command->contains ( BROWSE_FLAG ) &&
                upnp_command->getValue ( BROWSE_FLAG ) == BROWSE_DIRECT_CHILDREN &&
                upnp_command->getValue ( OBJECT_ID ) == "0" ) {

        upnp::DidlWriter writer_( &didlWriter, db_, upnp_command->getValue( OBJECT_ID ) );
        writer_.node( root_nodes_ );
        res_ = std::tuple< size_t, size_t > ( root_nodes_.size(), root_nodes_.size() );

    } else if ( upnp_command->contains ( BROWSE_FLAG ) &&
                upnp_command->getValue ( BROWSE_FLAG ) == BROWSE_DIRECT_CHILDREN ) {

        upnp::DidlWriter writer_( &didlWriter, db_, upnp_command->getValue( OBJECT_ID ) );

        //root node
        if( root_nodes_.find ( upnp_command->getValue( OBJECT_ID ) ) != root_nodes_.end() ) {

            size_t _start_index = 0, _count = 128U;
            if( upnp_command->contains( START_INDEX ) && Cds::has_only_digits ( upnp_command->getValue( START_INDEX ) ) )
            { _start_index = std::atol( upnp_command->getValue( START_INDEX ).c_str() ); }
            if( upnp_command->contains( REQUESTED_COUNT ) && Cds::has_only_digits ( upnp_command->getValue( REQUESTED_COUNT ) ) )
            { _count = std::atol( upnp_command->getValue( REQUESTED_COUNT ).c_str() ); }

            dao::query<didl::DidlObject>::run ( db_, std::vector< didl::DIDL_CLASS >( { root_nodes_.at( upnp_command->getValue( OBJECT_ID ) ) } ),
                                  didl::DidlObject::title, true, std::bind( &upnp::DidlWriter::write, &writer_, std::placeholders::_1 ), _start_index, _count );
            res_ = std::tuple< size_t, size_t > ( writer_.count(), dao::count( db_, root_nodes_.at( upnp_command->getValue( OBJECT_ID ) ) ) );

        //album node
        } else if ( ! strncmp(upnp_command->getValue( OBJECT_ID ).c_str(), DidlWriter::PREFIX_ALBUM_ID, strlen( DidlWriter::PREFIX_ALBUM_ID ) ) ) {
            const std::string _album_id = upnp_command->getValue( OBJECT_ID ).substr( strlen( DidlWriter::PREFIX_ALBUM_ID ), upnp_command->getValue( OBJECT_ID ).size() );
            if( Cds::has_only_digits( _album_id ) ) {
                dao::child< didl::DidlMusicTrack >( db_, std::bind( &upnp::DidlWriter::write, &writer_, std::placeholders::_1 ), std::atol( _album_id.c_str() ) );
                res_ = std::tuple< size_t, size_t > ( writer_.count(), dao::count< didl::DidlMusicTrack >( db_, std::atol( _album_id.c_str() ) ) );
            } else throw upnp::UpnpException( static_cast< int >( http::http_status::BAD_REQUEST ), std::string( "malformed album id: " ) +  upnp_command->getValue( OBJECT_ID ) );

        //artist node
        } else if ( ! strncmp(upnp_command->getValue( OBJECT_ID ).c_str(), DidlWriter::PREFIX_ARTIST_ID, strlen( DidlWriter::PREFIX_ARTIST_ID ) ) ) {
            const std::string _artist_id = upnp_command->getValue( OBJECT_ID ).substr( strlen( DidlWriter::PREFIX_ARTIST_ID ), upnp_command->getValue( OBJECT_ID ).size() );
            if( Cds::has_only_digits( _artist_id ) ) {
                dao::child_artist ( db_, "", std::bind( &upnp::DidlWriter::write, &writer_, std::placeholders::_1 ), std::atol( _artist_id.c_str() ) );
                res_ = std::tuple< size_t, size_t > ( writer_.count(), dao::count_artist_childs ( db_, std::atol( _artist_id.c_str() ) ) );
            } else throw upnp::UpnpException( static_cast< int >( http::http_status::BAD_REQUEST ), std::string( "malformed artist id: " ) +  upnp_command->getValue( OBJECT_ID ) );

        //series node
        } else if ( ! strncmp(upnp_command->getValue( OBJECT_ID ).c_str(), DidlWriter::PREFIX_SERIE_ID, strlen( DidlWriter::PREFIX_SERIE_ID ) ) ) {
            const std::string _serie_id = upnp_command->getValue( OBJECT_ID ).substr( strlen( DidlWriter::PREFIX_SERIE_ID ), upnp_command->getValue( OBJECT_ID ).size() );
            if( Cds::has_only_digits( _serie_id ) ) {
                std::cout << "get series for id: " << _serie_id << std::endl;
                dao::child< didl::DidlEpisode > ( db_, std::bind( &upnp::DidlWriter::write, &writer_, std::placeholders::_1 ), std::atol( _serie_id.c_str() ) );
                res_ = std::tuple< size_t, size_t > ( writer_.count(), dao::count< didl::DidlEpisode > ( db_, std::atol( _serie_id.c_str() ) ) );
            } else throw upnp::UpnpException( static_cast< int >( http::http_status::BAD_REQUEST ), std::string( "malformed serie id: " ) +  upnp_command->getValue( OBJECT_ID ) );

        } else {
            CLOG(ERROR, "upnp") << "not implmemented: " << upnp_command->getValue( OBJECT_ID );
        }

        std::cout << "\n------------------------------------------------------------------\n" << didlWriter.str() <<
                     "\n------------------------------------------------------------------" << std::endl;

    } else {
        CLOG(ERROR, "upnp") << "Browse unknown command (" << upnp_command->getValue ( BROWSE_FLAG ) << ")";
    }

    commons::xml::Node result_node = xmlWriter->element ( response_node, "", soap::XML_RESULT, didlWriter.str() );
    xmlWriter->attribute ( result_node, soap::XML_NS_SCHEMA_INSTANCE, soap::XML_TYPE, soap::XML_TYPE_STRING );

    commons::xml::Node nr_node = xmlWriter->element ( response_node, "", NUMBER_RESULT, std::to_string ( std::get< 0 > ( res_ ) ) );
    xmlWriter->attribute ( nr_node, soap::XML_NS_SCHEMA_INSTANCE, soap::XML_TYPE, soap::XML_TYPE_INT );
    commons::xml::Node tm_node = xmlWriter->element ( response_node, "", TOTAL_MATCHES, std::to_string ( std::get< 1 > ( res_ ) ) );
    xmlWriter->attribute ( tm_node, soap::XML_NS_SCHEMA_INSTANCE, soap::XML_TYPE, soap::XML_TYPE_INT );
    commons::xml::Node uid_node = xmlWriter->element ( response_node, "", UPDATE_ID, "1" ); //TODO
    xmlWriter->attribute ( uid_node, soap::XML_NS_SCHEMA_INSTANCE, soap::XML_TYPE, soap::XML_TYPE_INT );
}
}//namespace upnp
