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

#include "ssdpserverconnection.h"

//#include <http.h>

#include <thread>
#include <future>

namespace ssdp {
inline namespace asio_impl {

SSDPServerConnection::SSDPServerConnection ( const std::string & multicast_address,
        const int & multicast_port, std::function< void ( http::Request& ) > handler ) :
    io_service(), strand_ ( io_service ), socket ( io_service ), multicast_address ( multicast_address ),
    multicast_port ( multicast_port ), _handler ( handler ) {

	asio::ip::address _multicast_address = asio::ip::address::from_string ( multicast_address );
    asio::ip::address _listen_address = asio::ip::address::from_string ( "0.0.0.0" );

	// Create the socket so that multiple may be bound to the same address.
    std::cout << "bind to: " << _listen_address << ":" <<  multicast_port << std::endl;
    asio::ip::udp::endpoint listen_endpoint ( _multicast_address /*asio::ip::address_v4::any()*/, multicast_port );
	socket.open ( listen_endpoint.protocol() );

	socket.set_option ( asio::ip::udp::socket::reuse_address ( true ) );
	socket.bind ( listen_endpoint );

	// Join the multicast group.
	socket.set_option (
		asio::ip::multicast::join_group ( _multicast_address ) );

    using namespace std::placeholders;
    socket.async_receive_from ( asio::buffer ( data, http::BUFFER_SIZE ), sender_endpoint,
                                strand_.wrap ( std::bind ( &SSDPServerConnection::handle_receive_from, this, _1,_2 ) ) );

    ssdp_runner = std::unique_ptr<std::thread> ( new std::thread (
        std::bind ( static_cast<size_t ( asio::io_service::* ) () > ( &asio::io_service::run ), &io_service ) ) );
}

SSDPServerConnection::~SSDPServerConnection() {
	io_service.stop();
	ssdp_runner->join();
}

void SSDPServerConnection::send ( std::string request_line, std::map< std::string, std::string > headers ) {

    std::string message = ssdp::create_header ( request_line, headers );

	asio::io_service io_service_;
	asio::ip::udp::endpoint endpoint ( asio::ip::address::from_string ( multicast_address.c_str() ), multicast_port );
	asio::ip::udp::socket socket ( io_service_, endpoint.protocol() );
	socket.send_to (
		asio::buffer ( message, message.length() ), endpoint );
}

void SSDPServerConnection::send ( SsdpResponse response ) {
    std::string buffer = ssdp::create_header ( response.request_line, response.headers );
	socket.send_to (
		asio::buffer ( buffer, buffer.length() ), sender_endpoint );
}

void SSDPServerConnection::handle_receive_from ( const asio::error_code & error, size_t bytes_recvd ) {
	if ( !error ) {
        http::Request request;
        request.remote_ip ( sender_endpoint.address().to_string() );
        http_parser.parse_request( request, data, 0, bytes_recvd );
        _handler ( request );
        //TODO http_parser.reset();

        using namespace std::placeholders;
        socket.async_receive_from ( asio::buffer ( data, http::BUFFER_SIZE ), sender_endpoint,
            strand_.wrap ( std::bind ( &SSDPServerConnection::handle_receive_from, this, _1, _2 ) ) );

    } else { /*TODO http_parser.reset(); */ } //on error
}
}//namespace asio_impl
}//namespace ssdp
