/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SSDPASIOCONNECTION_H
#define SSDPASIOCONNECTION_H

#include <array>
#include <functional>
#include <memory>
#include <thread>

#include <asio.hpp>

#include "http/utils/httpparser.h"
#include "http/request.h"

#include "../ssdp.h"

namespace ssdp {
inline namespace asio_impl {

/**
 * ASIO implmentation of the SSDPConnection.
 */
class SSDPServerConnection {
public:
	/**
	 * Create a new SSDPAsioConnection.
	 */
    SSDPServerConnection ( const std::string & multicast_address, const int & port,
                           std::function< void ( http::Request& ) > handler );
    virtual ~SSDPServerConnection();

	/**
	 * Multicast a message to the network.
	 */
    void send ( std::string request_line, std::map< std::string, std::string > headers );
	/**
	 * Send a response to the request host.
	 */
    void send ( SsdpResponse response );

private:
	/* constuctor parameters */
	asio::io_service io_service;
	asio::io_service::strand strand_;
	asio::ip::udp::socket socket;
	std::string multicast_address;
	int multicast_port;

	asio::ip::udp::endpoint sender_endpoint;
    http::utils::HttpParser http_parser;

	/* local variables */
    std::function< void ( http::Request& ) > _handler;

    http::buffer_t data;

	/* the runner thread */
	std::unique_ptr<std::thread> ssdp_runner;

	void handle_receive_from ( const asio::error_code&, size_t bytes_recvd );
};
}//namespace asio_impl
}//namespace ssdp
#endif // SSDPASIOCONNECTION_H
