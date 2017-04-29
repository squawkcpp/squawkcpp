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

#include "ssdpclientconnection.h"
//#include "http.h"

namespace ssdp {
inline namespace asio_impl {

void SSDPClientConnection::send ( const std::string & request_line, const std::map< std::string, std::string > & headers ) {
	std::string message = create_header ( request_line, headers );

    asio::ip::udp::endpoint endpoint ( asio::ip::address::from_string ( multicast_address.c_str() ), multicast_port );
    asio::ip::address _listen_address = asio::ip::address::from_string ( "192.168.0.1" /*TODO*/);
    asio::ip::udp::endpoint listen_endpoint ( _listen_address, 0 );

    socket.open ( listen_endpoint.protocol() );
    socket.set_option ( asio::ip::udp::socket::reuse_address ( true ) );
    socket.bind ( listen_endpoint );

    using namespace std::placeholders;
    socket.async_receive_from ( asio::buffer ( data, max_length ), sender_endpoint,
                                strand_.wrap ( std::bind ( &SSDPClientConnection::handle_receive_from, this, _1, _2 ) ) );

    ssdp_runner = std::unique_ptr<std::thread> ( new std::thread (
                      std::bind ( static_cast<size_t ( asio::io_service::* ) () > ( &asio::io_service::run ), &io_service_ ) ) );
    socket.send_to ( asio::buffer ( message, message.length() ), endpoint );

    std::this_thread::sleep_for ( std::chrono::seconds ( 5 ) );
    socket.close();
}
void SSDPClientConnection::handle_receive_from ( const asio::error_code & error, size_t bytes_recvd ) {
    if ( !error ) {
        http::Response response;
        response.remote_ip( sender_endpoint.address().to_string() );
        http_parser.parse_response ( response, data, 0, bytes_recvd );
        _handler ( response );
        using namespace std::placeholders;
        socket.async_receive_from ( asio::buffer ( data, max_length ), sender_endpoint,
                                    strand_.wrap ( std::bind ( &SSDPClientConnection::handle_receive_from, this, _1, _2 ) ) );
	}
}
}//namespace asio_impl
}//namespace ssdp
