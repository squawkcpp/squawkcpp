#ifndef UPNPHEADERS_H
#define UPNPHEADERS_H

#include <boost/algorithm/string.hpp>
#include "http/request.h"
#include "http/response.h"
#include "http/constant.h"

namespace upnp {

class UpnpHeaders {
public:
    UpnpHeaders() {}
    UpnpHeaders( const UpnpHeaders& ) = default; //TODO
    UpnpHeaders( UpnpHeaders&& ) = default;
    UpnpHeaders& operator= ( const UpnpHeaders& ) = default; //TODO
    UpnpHeaders& operator= ( UpnpHeaders&& ) = default;
    ~UpnpHeaders() {}

    static http::http_status execute ( http::Request& request, http::Response& response );
};
}//namespace upnp
#endif // UPNPHEADERS_H
