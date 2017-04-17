#include "cms.h"

#include "upnp.h"

namespace upnp {

using namespace upnp; //TODO remove

http::http_status Cms::do_post ( http::Request & request, http::Response & response ) {

    try {
        upnp::UpnpContentDirectoryRequest upnp_command = upnp::parseRequest ( request.str() );
        CLOG ( TRACE, "upnp" ) << "Default:" << upnp_command;

        if ( upnp_command.type == upnp::UpnpContentDirectoryRequest::PROTOCOL_INFO ) {

            commons::xml::XMLWriter xmlWriter;
            commons::xml::Node envelope_node = upnp::soap::make_soap_envelope ( &xmlWriter );

            commons::xml::Node body_node = xmlWriter.element ( envelope_node, upnp::soap::XML_NS_SOAP, "Body" );
            commons::xml::Node response_node = xmlWriter.element ( body_node, "", "GetProtocolInfoResponse", "" );
            xmlWriter.ns ( response_node, upnp::XML_NS_UPNP_CMS, "u", true );

            commons::xml::Node source_node = xmlWriter.element ( response_node, "", "Source", "http-get:*:audio/mpeg:*,http-get:*:text/plain:*,http-get:*:video/mp4:*,http-get:*:video/mpeg:*,http-get:*:video/quicktime:*" /*SOURCE_TYPES*/ );
            xmlWriter.attribute ( source_node, upnp::soap::XML_NS_SCHEMA_INSTANCE, "type", "xsd:string" );
            commons::xml::Node sink_node = xmlWriter.element ( response_node, "", "Sink", "" );
            xmlWriter.attribute ( sink_node, upnp::soap::XML_NS_SCHEMA_INSTANCE, "type", "xsd:string" );

            response << xmlWriter.str();

            response.parameter( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
            response.status ( http::http_status::OK );
            return http::http_status::OK;

        } else {
            CLOG ( ERROR, "upnp" ) << "invoke::Unknown Method: " << upnp_command;
            return http::http_status::METHOD_NOT_ALLOWED;
        }

    } catch ( upnp::UpnpException & ex ) {
        CLOG ( ERROR, "upnp" ) << "UPNP parse error: " << ex.code() << ":" << ex.what();
        return http::http_status::INTERNAL_SERVER_ERROR;

    } catch ( commons::xml::XmlException & ex ) {
        CLOG ( ERROR, "upnp" ) << "XML parse error: " << ex.code() << ":" << ex.what();
        return http::http_status::INTERNAL_SERVER_ERROR;

    } catch ( ... ) {
        CLOG ( ERROR, "upnp" ) << "UPNP parse error.";
        return http::http_status::INTERNAL_SERVER_ERROR;
    }

    return http::http_status::BAD_REQUEST;
}
http::http_status Cms::do_default ( const std::string& /*method*/, http::Request & request, http::Response & response ) {
    CLOG ( TRACE, "upnp" ) << "Default:" << request;
    response.parameter ( http::header::CONTENT_LENGTH, "0" );
    response.parameter ( "SID", "uid:b419974c-9784-11e5-9793-9ae5d810837d" ); // TODO real UID
    response.parameter ( "TIMEOUT", "Second-1800" ); // TODO parameter
    return http::http_status::OK;
}
}//namespace upnp
