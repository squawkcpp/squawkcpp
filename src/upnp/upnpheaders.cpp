#include "upnpheaders.h"

#include <map>
#include <string>

#include <boost/algorithm/string.hpp>

namespace upnp {

http::http_status UpnpHeaders::execute ( http::Request& request, http::Response& response ) {

    //set the status line
    response.version_major ( request.version_major() );
    response.version_minor ( request.version_minor() );
    response.protocol ( request.protocol() );

    //set content length, some devices need upper case header names.
//    if ( ! response.contains_parameter ( http::header::CONTENT_LENGTH ) ) {
//        std::cout << "content lenth by tellp: " << response.tellp() << std::endl;
//        response.parameter ( boost::to_upper_copy<std::string>( http::header::CONTENT_LENGTH ),  std::to_string ( response.tellp() ) );
//    } else {
//        std::cout << "content lenth crom header: " << response.parameter( http::header::CONTENT_LENGTH ) << std::endl;
//        std::string _content_length = response.parameter( http::header::CONTENT_LENGTH );
//        response.erase( http::header::CONTENT_LENGTH );
//        response.parameter ( boost::to_upper_copy<std::string>( http::header::CONTENT_LENGTH ),  _content_length );
//    }

    std::string _content_type = response.parameter( http::header::CONTENT_TYPE );
    std::cout << "test mimt_type: " << _content_type << std::endl;
    if( _content_type == "video/mp4" ) {
        _content_type = "video/mpeg";
    }
    response.erase( http::header::CONTENT_TYPE );
    response.parameter ( boost::to_upper_copy<std::string>( http::header::CONTENT_TYPE ),  _content_type );

    //add expiration date
    if ( response.expires() ) {
        time_t now = time ( nullptr );
        struct tm then_tm = *gmtime ( &now );
        then_tm.tm_sec += response.expires();
        mktime ( &then_tm );
        response.parameter ( http::header::EXPIRES, http::utils::time_to_string ( &then_tm ) );
    }

    //add now
    time_t now = time ( nullptr );
    struct tm now_tm = *gmtime ( &now );
    mktime ( &now_tm );
    response.parameter ( http::header::DATE, http::utils::time_to_string ( &now_tm ) );

    //TODO more
    //add mime-type
    // ss << header::CONTENT_TYPE << ": " << parameters_[header::CONTENT_TYPE] << "\r\n";

    if ( request.contains_parameter ( "Getcontentfeatures.dlna.org" ) &&
        request.parameter ( "Getcontentfeatures.dlna.org" ) == "1" ) {
        response.parameter ( "transferMode.dlna.org", "Streaming" );
        response.parameter ( "Accept-Ranges", "bytes" );
        response.parameter ( "contentFeatures.dlna.org", "DLNA.ORG_OP=01;DLNA.ORG_CI=0;DLNA.ORG_FLAGS=017000 00000000000000000000000000" );
        response.parameter ( "EXT", "" );
    }
    return http::http_status::OK;
}
}//namespace upnp
