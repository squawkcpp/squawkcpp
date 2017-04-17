///*
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.

//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.

//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//*/

//#include "upnpcontentdirectory.h"

//#include "db/dbexception.h"

//namespace squawk {

//void UpnpContentDirectory::registerContentDirectoryModule ( std::unique_ptr< ContentDirectoryModule > module ) {
//    _modules.push_back ( std::move ( module ) );
//}

////void UpnpContentDirectory::notify( std::list< int > update_ids ) {
////    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "NOTIFY:" << update_ids.size() )
////    commons::xml::XMLWriter writer;
////    commons::xml::Node root_node = writer.element( "propertyset" /*, TODO "urn:schemas-upnp-org:event-1-0" */ );
////    writer.ns( root_node, "urn:schemas-upnp-org:event-1-0", "e" );

////    commons::xml::Node property_node = writer.element( root_node, "urn:schemas-upnp-org:event-1-0", "property" );
////    std::string update_list;
////    bool first = true;
////    for( auto & id : update_ids ) {
////        if( first ) first = false;
////        else update_list += ",";
////        update_list += id;
////    }
////    writer.element( property_node, "", "SystemUpdateID", "1" );
////    writer.element( property_node, "", "ContainerUpdateIDs", update_list );

////    std::string message_body = writer.str();
////    std::istringstream message_stream( message_body );

////    for( auto & listener : _listener_list ) {

////        //make the request
////        try {
////            curlpp::Cleanup myCleanup;
////            curlpp::Easy myRequest;

////            std::list< std::string > headers;
////            headers.push_back("CONTENT-TYPE: text/xml; charset=UTF-8");
////            headers.push_back("Content-Length: " + message_body.length() );
////            headers.push_back("Accept:" );
////            headers.push_back("Expect:" );
////            headers.push_back("NT: upnp:event");
////            headers.push_back("NTS: upnp:propchange");
////            headers.push_back("SID: uuid:" + uuid );
////            headers.push_back("SEQ: 0");

////            myRequest.setOpt<curlpp::options::Url>( listener.url() );
////            myRequest.setOpt<curlpp::options::CustomRequest>("NOTIFY");
////            myRequest.setOpt(new curlpp::Options::Verbose(true));
////            myRequest.setOpt(new curlpp::Options::ReadStream( &message_stream ));
////            myRequest.setOpt(new curlpp::Options::InfileSize( message_body.length() ) );
////            myRequest.setOpt(new curlpp::Options::Upload(true));
////            myRequest.setOpt(new curlpp::Options::HttpHeader(headers));

////            if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "NOTIFY_PERFORM:" << listener.url() )
////            myRequest.perform();
////            if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "NOTIFY_DONE:" << listener.url() )

////        } catch(curlpp::RuntimeError & e) {
////            std::cerr << e.what() << std::endl;
////        } catch(curlpp::LogicError & e) {
////            std::cerr << e.what() << std::endl;
////        }
////    }
////    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "NOTIFY_PERFORM:DONE" )
////}

//http::http_status UpnpContentDirectory::do_default ( const std::string & method, http::Request & request, http::Response & /* response */ ) {
//    CLOG(DEBUG, "upnp") << method << ": " << request.str();
////    if( request.containsParameter( "Timeout" ) ) {
////        size_t pos = request.parameter( "Timeout" ).find( "-" );
////        size_t len = request.parameter( "Timeout" ).length() - pos;
////        std::string number = request.parameter( "Timeout" ).substr( pos + 1, len );
////        int timeout = std::stoi( number );

////        std::string callback = request.parameter( "Callback" ).substr( 1, request.parameter( "Callback" ).length() - 2 );
////        LOG4CXX_DEBUG(logger, "SUBSCRIBE:" << timeout << ":" + callback )
////        _listener_list.push_back( UpnpCallback( callback ) );
////        response.status( http::http_status::OK );
////        response.parameter( http::header::CONTENT_LENGTH, "0" );
////        response.parameter( "SID", "uuid:" + uuid );
////        response.parameter( "TIMEOUT", request.parameter( "Timeout" ) );

////        /* TODO
////        notify_runner = new std::thread([&,this]() {
////            notify( std::list<int>( { 0 }) );
////            //TODO delete notify_runner;
////        }); */

////        if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "NOTIFY END")

////    } else {
////        response.status( http::http_status::BAD_REQUEST );
////    }
////    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "DUBDCRIB END")
//    return http::http_status::OK;
//    }

//http::http_status UpnpContentDirectory::do_post ( http::Request & request, http::Response & response ) {

////        try {
////            upnp::UpnpContentDirectoryRequest upnp_command = upnp::parseRequest ( request.str() );
////            CLOG(DEBUG, "upnp") << "UpnpRequest:" << std::endl << upnp_command;

////            if ( upnp_command.type == upnp::UpnpContentDirectoryRequest::BROWSE ) {

////                commons::xml::XMLWriter xmlWriter;
////                browse ( &xmlWriter, &upnp_command );

////                response <<  xmlWriter.str();
////                response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
////                response.status ( http::http_status::OK );

////            } else if ( upnp_command.type == upnp::UpnpContentDirectoryRequest::X_FEATURE_LIST ) {

////                CLOG(DEBUG, "upnp") << "X_GetFeatureList: " << request << "\n" << upnp_command; //TODO remove
////                commons::xml::XMLWriter xmlWriter;
////                commons::xml::Node envelope_node = xmlWriter.element ( "Envelope" );
////                xmlWriter.ns ( envelope_node, upnp::XML_NS_SOAP, "s", true );
////                xmlWriter.ns ( envelope_node, upnp::XML_NS_SOAPENC, "soapenc", false );
////                xmlWriter.ns ( envelope_node, upnp::XML_NS_SCHEMA, "xsd", false );
////                xmlWriter.ns ( envelope_node, upnp::XML_NS_SCHEMA_INSTANCE, "xsi", false );
////                xmlWriter.attribute ( envelope_node, upnp::XML_NS_SOAP, "encodingStyle", XML_NS_SOAPENC );

////                commons::xml::Node body_node = xmlWriter.element ( envelope_node, upnp::XML_NS_SOAP, "Body" );
////                commons::xml::Node response_node = xmlWriter.element ( body_node, "", "X_GetFeatureListResponse", "" );
////                xmlWriter.ns ( response_node, upnp::XML_NS_UPNP_CDS, "u", true );

////                commons::xml::XMLWriter didlWriter;
////                commons::xml::Node features_node = didlWriter.element ( "Features" );
////                xmlWriter.ns ( features_node, "urn:schemas-upnp-org:av:avs", "", true );
////                xmlWriter.ns ( features_node, "http://www.w3.org/2001/XMLSchema-instance", "xsi" );
////                xmlWriter.attribute ( features_node, "http://www.w3.org/2001/XMLSchema-instance", "encodingStyle", "urn:schemas-upnp-org:av:avs http://www.upnp.org/schemas/av/avs.xsd" );

////                commons::xml::Node feature_node = didlWriter.element ( features_node, "", "Feature" );
////                xmlWriter.attribute ( feature_node, "", "name", "samsung.com_BASICVIEW" );
////                xmlWriter.attribute ( feature_node, "", "version", "1" );

////                commons::xml::Node audio_node = didlWriter.element ( feature_node, "", "container" );
////                xmlWriter.attribute ( audio_node, "", "id", "music" );
////                xmlWriter.attribute ( audio_node, "", "type", "object.item.audioItem" );

////                commons::xml::Node video_node = didlWriter.element ( feature_node, "", "container" );
////                xmlWriter.attribute ( video_node, "", "id", "video" );
////                xmlWriter.attribute ( video_node, "", "type", "object.item.videoItem" );

////                commons::xml::Node image_node = didlWriter.element ( feature_node, "", "container" );
////                xmlWriter.attribute ( image_node, "", "id", "image" );
////                xmlWriter.attribute ( image_node, "", "type", "object.item.imageItem" );

////                xmlWriter.element ( response_node, "", "FeatureList", didlWriter.str() );

////                response <<  xmlWriter.str();

////                response.parameter ( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
////                return ( http::http_status::OK );

////            } else {
////                CLOG(ERROR, "upnp") << "invoke::Unknown Method: " << upnp_command;
////                return http::http_status::BAD_REQUEST;
////            }

////        } catch ( upnp::UpnpException & ex ) {
////            CLOG(ERROR, "upnp") << "UPNP parse error: " << ex.code() << ":" << ex.what();
////            return http::http_status::INTERNAL_SERVER_ERROR;

////        } catch ( db::DbException & ex ) {
////            CLOG(ERROR, "upnp") << "DB Exception: " << ex.code() << ":" << ex.what();
////            return http::http_status::INTERNAL_SERVER_ERROR;

////        } catch ( commons::xml::XmlException & ex ) {
////            CLOG(ERROR, "upnp") << "XML parse error: " << ex.code() << ":" << ex.what();
////            return http::http_status::INTERNAL_SERVER_ERROR;

////        } catch ( ... ) {
////            CLOG(ERROR, "upnp") << "UPNP parse error.";
////            return http::http_status::INTERNAL_SERVER_ERROR;
////        }
//    return http::http_status::OK;
//}

//void UpnpContentDirectory::browse ( commons::xml::XMLWriter * xmlWriter, upnp::UpnpContentDirectoryRequest * upnp_command ) {

//    commons::xml::Node envelope_node = xmlWriter->element ( "Envelope" );
//    xmlWriter->ns ( envelope_node, upnp::XML_NS_SOAP, "s", true );
//    xmlWriter->ns ( envelope_node, upnp::XML_NS_SOAPENC, "soapenc", false );
//    xmlWriter->ns ( envelope_node, upnp::XML_NS_SCHEMA, "xsd", false );
//    xmlWriter->ns ( envelope_node, upnp::XML_NS_SCHEMA_INSTANCE, "xsi", false );
//    xmlWriter->attribute ( envelope_node, upnp::XML_NS_SOAP, "encodingStyle", XML_NS_SOAPENC );

//    commons::xml::Node body_node = xmlWriter->element ( envelope_node, upnp::XML_NS_SOAP, "Body" );
//    commons::xml::Node response_node = xmlWriter->element ( body_node, "", "BrowseResponse", "" );
//    xmlWriter->ns ( response_node, upnp::XML_NS_UPNP_CDS, "u", true );

//    commons::xml::XMLWriter didlWriter;
//    std::tuple< size_t, size_t > res_;

//    // browse metadata
//    if ( upnp_command->contains ( "BrowseFlag" ) &&
//            upnp_command->getValue ( "BrowseFlag" ) == "BrowseMetadata" ) {

//        commons::xml::Node didl_element = didlWriter.element ( "DIDL-Lite" );
//        didlWriter.ns ( didl_element, upnp::XML_NS_DIDL );
//        didlWriter.ns ( didl_element, upnp::XML_NS_PURL, "dc" );
//        didlWriter.ns ( didl_element, upnp::XML_NS_DLNA, "dlna" );
//        didlWriter.ns ( didl_element, upnp::XML_NS_UPNP, "upnp" );
//        didlWriter.ns ( didl_element, upnp::XML_NS_PV, "pv" );

//        commons::xml::Node container_element = didlWriter.element ( didl_element, "", "container", "" );
//        didlWriter.attribute ( container_element, "id", "0" );
//        didlWriter.attribute ( container_element, "parentID", "-1" );
//        didlWriter.attribute ( container_element, "restricted", "1" );
//        didlWriter.attribute ( container_element, "childCount", "2" );

//        commons::xml::Node audio_element = didlWriter.element ( container_element, upnp::XML_NS_UPNP, "searchClass", "object.item.audioItem" );
//        didlWriter.attribute ( audio_element, "includeDerived", "1" );
//        commons::xml::Node image_element = didlWriter.element ( container_element, upnp::XML_NS_UPNP, "searchClass", "object.item.imageItem" );
//        didlWriter.attribute ( image_element, "includeDerived", "1" );
//        commons::xml::Node video_element = didlWriter.element ( container_element, upnp::XML_NS_UPNP, "searchClass", "object.item.videoItem" );
//        didlWriter.attribute ( video_element, "includeDerived", "1" );
//        didlWriter.element ( container_element, upnp::XML_NS_PURL, "title", "root" );
//        didlWriter.element ( container_element, upnp::XML_NS_UPNP, "class", "object.container.storageFolder" );
//        didlWriter.element ( container_element, upnp::XML_NS_UPNP, "storageUsed", "-1" );

//        res_ = std::tuple< size_t, size_t > ( 3, 3 );

//    } else if ( upnp_command->contains ( "BrowseFlag" ) &&
//                upnp_command->getValue ( "BrowseFlag" ) == "BrowseDirectChildren" &&
//                upnp_command->getValue ( "ObjectID" ) == "0" ) {

//        didl::DidlXmlWriter didlDocument ( &didlWriter );

//        size_t result_count_ = 0;

//        for ( auto & module_ : _modules ) {
//            result_count_ += module_->getRootNode ( &didlDocument );
//        }

//        res_ = std::tuple< size_t, size_t > ( result_count_, result_count_ );

//    } else if ( upnp_command->contains ( "BrowseFlag" ) &&
//                upnp_command->getValue ( "BrowseFlag" ) == "BrowseDirectChildren" ) {

//        didl::DidlXmlWriter didl_element ( &didlWriter );

//        //search all callback handler
//        for ( auto & module : _modules ) {
//            if ( module->match ( upnp_command ) ) {
//                res_ = module->parseNode ( &didl_element, upnp_command );
//            }
//        }

//    } else {
//        CLOG(ERROR, "upnp") << "Browse unknown command (" << upnp_command->getValue ( "BrowseFlag" ) << ")";
//    }

//    commons::xml::Node result_node = xmlWriter->element ( response_node, "", "Result", didlWriter.str() );
//    xmlWriter->attribute ( result_node, upnp::XML_NS_SCHEMA_INSTANCE, "type", "xsd:string" );

//    commons::xml::Node nr_node = xmlWriter->element ( response_node, "", "NumberReturned", std::to_string ( std::get< 0 > ( res_ ) ) );
//    xmlWriter->attribute ( nr_node, upnp::XML_NS_SCHEMA_INSTANCE, "type", "xsd:int" );
//    commons::xml::Node tm_node = xmlWriter->element ( response_node, "", "TotalMatches", std::to_string ( std::get< 1 > ( res_ ) ) );
//    xmlWriter->attribute ( tm_node, upnp::XML_NS_SCHEMA_INSTANCE, "type", "xsd:int" );
//    commons::xml::Node uid_node = xmlWriter->element ( response_node, "", "UpdateID", "1" ); //TODO
//    xmlWriter->attribute ( uid_node, upnp::XML_NS_SCHEMA_INSTANCE, "type", "xsd:int" );
//}
//}//namespace squawk
