/*
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

#include <ctime>
#include <functional>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/utsname.h>

#include <boost/algorithm/string.hpp>

#include "ssdpserverimpl.h"

#include "fmt/format.h"
#include "fmt/time.h"

namespace ssdp {

inline std::string uname() {
    struct utsname uts;
    uname ( &uts );
    std::ostringstream system;
    system << uts.sysname << "/" << uts.version;
    return system.str();
};
inline std::string time_string() {
    time_t rawtime;
    time ( &rawtime );
    std::string str_time = std::string ( std::ctime ( &rawtime ) );
    boost::trim ( str_time );
    return str_time;
};

const size_t SSDPServerImpl::SSDP_THREAD_SLEEP = 5000;
const size_t SSDPServerImpl::NETWORK_COUNT = 3;
const size_t SSDPServerImpl::ANNOUNCE_INTERVAL = 1800;
const std::string SSDPServerImpl::SSDP_HEADER_SERVER = "Server";
const std::string SSDPServerImpl::SSDP_HEADER_DATE = "Date";
const std::string SSDPServerImpl::SSDP_HEADER_ST = "St";
const std::string SSDPServerImpl::SSDP_HEADER_NTS = "Nts";
const std::string SSDPServerImpl::SSDP_HEADER_USN = "Usn";
const std::string SSDPServerImpl::SSDP_HEADER_LOCATION = "Location";
const std::string SSDPServerImpl::SSDP_HEADER_NT = "Nt";
const std::string SSDPServerImpl::SSDP_HEADER_MX = "Mx";
const std::string SSDPServerImpl::SSDP_HEADER_MAN = "Man";
const std::string SSDPServerImpl::SSDP_HEADER_EXT = "Ext";
const std::string SSDPServerImpl::SSDP_OPTION_MAX_AGE = "max-age=";
const std::string SSDPServerImpl::SSDP_REQUEST_LINE_OK = "HTTP/1.1 200 OK";
const std::string SSDPServerImpl::SSDP_STATUS_DISCOVER	= "ssdp:discover";
const std::string SSDPServerImpl::SSDP_STATUS_ALIVE	= "ssdp:alive";
const std::string SSDPServerImpl::SSDP_STATUS_BYE = "ssdp:byebye";
const std::string SSDPServerImpl::SSDP_NS_ALL = "ssdp:all";
const std::string SSDPServerImpl::SSDP_MSEARCH = "M-SEARCH";
const std::string SSDPServerImpl::SSDP_NOTIFY = "NOTIFY";
const std::string SSDPServerImpl::SSDP_HEADER_REQUEST_LINE = "NOTIFY * HTTP/1.1";
const std::string SSDPServerImpl::SSDP_HEADER_SEARCH_REQUEST_LINE = "M-SEARCH * HTTP/1.1";

SSDPServerImpl::SSDPServerImpl ( const std::string & uuid, const std::string & multicast_address, const int & multicast_port,
                                 const std::string & listen_address, const std::map< std::string, std::string > & namespaces ) :
	uuid ( uuid ), multicast_address ( multicast_address ), multicast_port ( multicast_port ) {

    for ( auto & ns : namespaces ) {
        register_namespace ( ns.first, ns.second );
    }

    //start the server
    using namespace std::placeholders;
    connection = std::unique_ptr<SSDPServerConnection> ( new SSDPServerConnection (
        multicast_address, multicast_port, listen_address, std::bind ( &SSDPServerImpl::handle_receive, this, _1 ) ) );
    //start reannounce thread
    announce_thread_run = true;
    annouceThreadRunner = std::unique_ptr<std::thread> (
                              new std::thread ( &SSDPServerImpl::annouceThread, this ) );
}

/**
 * Stop the server.
 */
SSDPServerImpl::~SSDPServerImpl() {
    //stop reannounce thread
    announce_thread_run = false;
    annouceThreadRunner->join();
    suppress();
}
void SSDPServerImpl::handle_response ( http::Response & response ) {
    if ( response.status() == http::http_status::OK ) {
        if ( response.parameter ( SSDP_HEADER_USN ).find ( uuid ) == std::string::npos ) {
//            fireEvent ( SSDP_EVENT_TYPE::ANNOUNCE, response.remote_ip(), parseResponse ( response ) );
            signal_detected.emit(  SSDP_EVENT_TYPE::ANNOUNCE, response.remote_ip(), parseResponse ( response ) );

        }//fi no self announcement
	}
}
void SSDPServerImpl::handle_receive ( http::Request & request ) {
    // do not process own messages received over other interface
    if ( request.parameter ( SSDP_HEADER_USN ).find ( uuid ) == std::string::npos ) {

        if ( request.method() == SSDP_MSEARCH ) { //search request
            //search all devices
            if ( request.parameter ( SSDP_HEADER_ST ) == NS_ROOT_DEVICE || request.parameter ( SSDP_HEADER_ST ) == SSDP_NS_ALL ) {
                for ( auto & iter : namespaces ) {
                    SsdpResponse response ( http::http_status::OK, SSDP_REQUEST_LINE_OK, create_response ( iter.first, iter.second ) );
                    connection->send ( response );
                }

            //search specific devices
            } else if ( namespaces.find ( request.parameter ( SSDP_HEADER_ST ) ) != namespaces.end() ) {
                connection->send ( SsdpResponse ( http::http_status::OK, SSDP_REQUEST_LINE_OK,
                                              create_response ( request.parameter ( SSDP_HEADER_ST ),
                                                      namespaces[request.parameter ( SSDP_HEADER_ST ) ] ) ) );
            }

        //notify status
        } else if ( request.method() == SSDP_NOTIFY ) {

            if ( request.parameter ( SSDP_HEADER_NTS ) == SSDP_STATUS_ALIVE ) {
//                fireEvent ( SSDP_EVENT_TYPE::ANNOUNCE, request.remote_ip(), parseRequest ( request ) );
                signal_detected.emit(  SSDP_EVENT_TYPE::ANNOUNCE, request.remote_ip(), parseRequest ( request ) );


            } else {
//                fireEvent ( SSDP_EVENT_TYPE::BYE, request.remote_ip(), parseRequest ( request ) );
                signal_detected.emit(  SSDP_EVENT_TYPE::BYE, request.remote_ip(), parseRequest ( request ) );
            }

        } else { assert ( false ); } //other response
    }//fi no self announcement
}

SsdpEvent SSDPServerImpl::parseRequest ( http::Request & request ) {
	time_t cache_control = 0;

    if ( request.contains_parameter ( http::header::CACHE_CONTROL ) ) {
        cache_control = parse_keep_alive ( request.parameter ( http::header::CACHE_CONTROL ) );
    }

    return SsdpEvent { request.parameter ( http::header::HOST ), request.parameter ( SSDP_HEADER_LOCATION ),
                       request.parameter ( SSDP_HEADER_NT ), request.parameter ( SSDP_HEADER_NTS ),
                       request.parameter ( SSDP_HEADER_SERVER ), request.parameter ( SSDP_HEADER_USN ),
                       std::time ( 0 ), cache_control };
}
SsdpEvent SSDPServerImpl::parseResponse ( http::Response & response ) {
	time_t cache_control = 0;

    if ( response.contains_parameter ( http::header::CACHE_CONTROL ) ) {
        cache_control = parse_keep_alive ( response.parameter ( http::header::CACHE_CONTROL ) );

    } else { assert ( false ); } //no cache control in response

    return SsdpEvent { response.parameter ( http::header::HOST ), response.parameter ( SSDP_HEADER_LOCATION ),
                       response.parameter ( SSDP_HEADER_ST ), response.parameter ( SSDP_HEADER_NTS ),
                       response.parameter ( SSDP_HEADER_SERVER ), response.parameter ( SSDP_HEADER_USN ),
                       std::time ( 0 ), cache_control };
}
void SSDPServerImpl::announce() {
    suppress();

	for ( size_t i = 0; i < NETWORK_COUNT; i++ ) {
		for ( auto & iter : namespaces ) {
			send_anounce ( iter.first, iter.second );
		}
	}
}
void SSDPServerImpl::suppress() {
	for ( size_t i = 0; i < NETWORK_COUNT; i++ ) {
		for ( auto & iter : namespaces ) {
			send_suppress ( iter.first );
		}
	}
}
void SSDPServerImpl::search ( const std::string & service ) {

	std::async ( std::launch::async, [this, &service]() {

		std::map< std::string, std::string > map;
        map[boost::to_upper_copy ( http::header::HOST )] = fmt::format (  "{}:{}", multicast_address, multicast_port );
        map[boost::to_upper_copy ( SSDP_HEADER_ST )] = service;
        map[boost::to_upper_copy ( SSDP_HEADER_MX )] = "2";
        map[boost::to_upper_copy ( SSDP_HEADER_MAN )] = SSDP_STATUS_DISCOVER;
        map[boost::to_upper_copy ( http::header::CONTENT_LENGTH )] = "0";

        using namespace std::placeholders;
        SSDPClientConnection connection ( multicast_address, multicast_port, std::bind( &SSDPServerImpl::handle_response, this, _1 ) );
        connection.send ( SSDP_HEADER_SEARCH_REQUEST_LINE, map );
	} );
}
std::map< std::string, std::string > SSDPServerImpl::create_response ( const std::string & nt, const std::string & location ) {

	std::map< std::string, std::string > map;
    map[boost::to_upper_copy ( http::header::CACHE_CONTROL )] = fmt::format ( "max-age={}", ANNOUNCE_INTERVAL );
    map[boost::to_upper_copy ( SSDP_HEADER_LOCATION )] = location;
    map[boost::to_upper_copy ( SSDP_HEADER_SERVER )] = fmt::format ( "{} DLNADOC/1.50 UPnP/1.0 SSDP/1.0.0", uname() );
    map[boost::to_upper_copy ( SSDP_HEADER_ST )] = nt;
    map[boost::to_upper_copy ( SSDP_HEADER_USN )] = fmt::format ( "uuid:{}::{}", uuid, nt );
    map[boost::to_upper_copy ( SSDP_HEADER_EXT )] = "";
    map[boost::to_upper_copy ( http::header::DATE )] = time_string();
    map[boost::to_upper_copy ( http::header::CONTENT_LENGTH )] = "0";

	return map;
}
void SSDPServerImpl::send_anounce ( const std::string & nt, const std::string & location ) {

	std::map< std::string, std::string > map;
    map[boost::to_upper_copy ( http::header::HOST )] = fmt::format (  "{}:{}", multicast_address, multicast_port );
    map[boost::to_upper_copy ( http::header::CACHE_CONTROL )] = fmt::format ( "max-age={}", ANNOUNCE_INTERVAL );
    map[boost::to_upper_copy ( SSDP_HEADER_LOCATION )] = location;
    map[boost::to_upper_copy ( SSDP_HEADER_SERVER )] = fmt::format ( "{} DLNADOC/1.50 UPnP/1.0 SSDP/1.0.0", uname() );
    map[boost::to_upper_copy ( SSDP_HEADER_NT )] = nt;
    map[boost::to_upper_copy ( SSDP_HEADER_USN )] = fmt::format ( "uuid:{}::{}", uuid, nt );
    map[boost::to_upper_copy ( SSDP_HEADER_NTS )] = SSDP_STATUS_ALIVE;
    map[boost::to_upper_copy ( SSDP_HEADER_EXT )] = "";
    map[boost::to_upper_copy ( SSDP_HEADER_DATE )] = time_string();
    map[boost::to_upper_copy ( http::header::CONTENT_LENGTH )] = "0";

	connection->send ( SSDP_HEADER_REQUEST_LINE, map );
}
void SSDPServerImpl::send_suppress ( const std::string & nt ) {

	std::map< std::string, std::string > map;
    map[boost::to_upper_copy ( http::header::HOST )] = fmt::format (  "{}:{}", multicast_address, multicast_port );
    map[boost::to_upper_copy ( SSDP_HEADER_NT )] = nt;
    map[boost::to_upper_copy ( SSDP_HEADER_USN )] = fmt::format ( "uuid:{}::{}", uuid, nt );
    map[boost::to_upper_copy ( SSDP_HEADER_NTS )] = SSDP_STATUS_BYE;
    map[boost::to_upper_copy ( SSDP_HEADER_SERVER )] = fmt::format ( "{} DLNADOC/1.50 UPnP/1.0 SSDP/1.0.0", uname() );
    map[boost::to_upper_copy ( SSDP_HEADER_EXT )] = "";
    map[boost::to_upper_copy ( SSDP_HEADER_DATE )] = time_string();
    map[boost::to_upper_copy ( http::header::CONTENT_LENGTH )] = "0";

	connection->send ( SSDP_HEADER_REQUEST_LINE, map );
}
void SSDPServerImpl::annouceThread() {
    _announce_time = std::chrono::high_resolution_clock::now();

	while ( announce_thread_run ) {

        //check reanounce timer
		auto end_time = std::chrono::high_resolution_clock::now();
        auto dur = end_time - _announce_time;
		auto f_secs = std::chrono::duration_cast<std::chrono::duration<unsigned int>> ( dur );

		if ( f_secs.count() >= ( ANNOUNCE_INTERVAL / 3 ) ) {
			for ( size_t i = 0; i < NETWORK_COUNT; i++ ) {
				for ( auto & iter : namespaces ) {
					send_anounce ( iter.first, iter.second );
				}
			}

            _announce_time = std::chrono::high_resolution_clock::now();
		}

        std::this_thread::sleep_for ( std::chrono::milliseconds ( SSDP_THREAD_SLEEP ) );
	}
}
//void SSDPServerImpl::fireEvent ( SSDP_EVENT_TYPE type, std::string  client_ip, SsdpEvent device ) const {
//	for ( auto & listener : listeners ) {
//        listener ( type, client_ip, device );
//	}
//}
}//namespace ssdp
