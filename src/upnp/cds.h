#ifndef CDS_H
#define CDS_H

#include <unordered_map>

#include "http/request.h"
#include "http/response.h"

#include "../utils/xml.h"

#include "upnp.h"

namespace upnp {
/**
 * @brief The Content-Directory class.
 */
class Cds {
public:
    /** @brief Cds constructor */
    Cds() {}
    /**
     * @brief subscribe to content directory events.
     * @param request HTTP request
     * @param response HTTP response
     * @return HTTP status code
     */
    static http::http_status do_subscribe ( http::Request& request, http::Response& response );
    /**
     * @brief Http-Post method.
     * @param request HTTP request
     * @param response HTTP response
     * @return HTTP status code
     */
    static http::http_status do_post ( http::Request& request, http::Response& response );
    /**
     * @brief content directory feature list.
     * @param xmlWriter the XML writer
     * @param upnp_command UPNP request command
     */
    static void feature_list ( commons::xml::XMLWriter* xmlWriter, upnp::UpnpContentDirectoryRequest* upnp_command );
    /**
     * @brief browse content directory.
     * @param xmlWriter the XML writer
     * @param upnp_command UPNP request command
     */
    static void browse ( commons::xml::XMLWriter* xmlWriter, upnp::UpnpContentDirectoryRequest* upnp_command );

private:
    static const std::unordered_map< std::string, didl::DIDL_CLASS > root_nodes_;

    static bool has_only_digits( const std::string& s ) {
      return ( s.find_first_not_of( "0123456789" ) == std::string::npos);
    }
};
}//namespace upnp
#endif // CDS_H
