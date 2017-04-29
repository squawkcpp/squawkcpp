/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

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

#include "squawkserver.h"

#include <string>
#include <iostream>
#include <thread>
#include <exception>
#include <signal.h>

#include "fmt/format.h"
#include "fmt/time.h"

#include "http/mod/exec.h"
#include "http/mod/file.h"
#include "http/mod/match.h"
#include "http/mod/method.h"

#include "ssdp/ssdp.h"

#include "api.h"
#include "database.h"
#include "scanner.h"

#include "upnp/cds.h"
#include "upnp/cms.h"
#include "upnp/description.h"
#include "upnp/upnp.h"

#include "config.h"

#include <sigc++/sigc++.h>

namespace squawk {

using namespace std::placeholders;

void SquawkServer::start() {

    /** Create the Server **/
    create_db();

    /** Setup and start the HTTP Server **/
    web_server = std::shared_ptr< http::Server< http::HttpServer > >( new http::Server< http::HttpServer >(
        Config::instance()->localListenAddress(),
        std::to_string( Config::instance()->httpPort() )
    ));


    web_server->bind( http::mod::Match<>( "/ctl/ContentDir" ),
                      http::mod::Method (
                        http::mod::method::POST( http::mod::Exec( std::bind( &upnp::Cds::do_post, _1, _2 ) ) ),
                        http::mod::method::GET( http::mod::Exec( [] ( http::Request&, http::Response& response) {
                            response.parameter( http::header::CONTENT_LENGTH, "0" );
                            return http::http_status::METHOD_NOT_ALLOWED;
                        })),
                        http::mod::method::S( "SUBSCRIBE", http::mod::Exec( std::bind( &upnp::Cds::do_subscribe, _1, _2 ) ) )
                      ), http::mod::Http() );

    web_server->bind( http::mod::Match<>( "/ctl/ConnectionMgr" ),
                      http::mod::Exec( std::bind( &upnp::Cms::do_post, std::make_shared< upnp::Cms >(), _1, _2 ) ),
                      http::mod::Http() );

    web_server->bind( http::mod::Match<>( "/rootDesc.xml" ),
                      http::mod::Exec( std::bind( &upnp::Description::do_get , _1, _2 ) ),
                      http::mod::Http() );

    web_server->bind( http::mod::Match<>( "/query/?" ),
                      http::mod::Exec( std::bind( &squawk::Api::query, _1, _2 ) ),
                      http::mod::Http() );

    web_server->bind( http::mod::Match< std::string >( "/album/(\\d*)", "id" ),
                      http::mod::Exec( std::bind( &squawk::Api::album, _1, _2 ) ),
                      http::mod::Http() );

    web_server->bind( http::mod::Match< std::string >( "/book/(\\d*)", "id" ),
                      http::mod::Exec( std::bind( &squawk::Api::book, _1, _2 ) ),
                      http::mod::Http() );

    web_server->bind( http::mod::Match< std::string >( "/movie/(\\d*)", "id" ),
                      http::mod::Exec( std::bind( &squawk::Api::movie, _1, _2 ) ),
                      http::mod::Http() );

    web_server->bind( http::mod::Match< std::string >( "/series/(\\d*)", "id" ),
                      http::mod::Exec( std::bind( &squawk::Api::series, _1, _2 ) ),
                      http::mod::Http() );

    web_server->bind( http::mod::Match< std::string, std::string >( "/res/(\\d*).(flac|mp3|avi|mp4|mkv|mpeg|mov|wmv|jpg|png|pdf|epub)", "id", "ext" ),
                      http::mod::Exec( [](http::Request& request, http::Response& ) -> http::http_status {
                          //TODO there is a problem with filename "xyz.....flac"
                          http::http_status _status = http::http_status::NOT_FOUND;
                          dao::item< upnp::didl::DidlObject >(  db::make_connection( Config::instance()->databaseFile() ),
                          [&request,&_status](auto stmt) {
                              request.uri( db::get< const char * >( stmt, upnp::didl::DidlObject::path ) );
                              _status = http::http_status::OK;
                          }, std::atol( request.attribute( "id" ).c_str() ) );
                          return _status;
                      }),
                      http::mod::File( "/" ),
                      http::mod::Http() /* TODO UPNP headers */ );

//    web_server->bind( http::mod::Match< std::string >( "/api/(upnp/device|upnp/event|album|artist|track|browse|statistic)", "command" ),
//                      http::mod::Exec( std::bind( &squawk::UpnpContentDirectoryApi::do_get, std::make_shared< squawk::UpnpContentDirectoryApi >(), _1, _2 ) ),
//                      http::mod::Http() );

    web_server->bind( http::mod::Match<>( "/bower_components/.*" ),
                      http::mod::File( Config::instance()->bowerRoot() ),
                      http::mod::Http() );

    web_server->bind( http::mod::Match<>( "/albumArt/.*" ),
                      http::mod::File( Config::instance()->tmpDirectory() ),
                      http::mod::Http() );

    web_server->bind( http::mod::Match<>( "*" ),
                      http::mod::File( Config::instance()->docRoot() ),
                      http::mod::Http() );

    //    web_server->register_servlet( std::unique_ptr< http::HttpServlet >(
//        new squawk::UpnpContentDirectoryApi( "/api/(upnp/device|upnp/event|album|artist|track|browse|statistic)/?(\\d*)?") ) );

    /** Setup and start the SSDP Server **/
    const std::string device_uri_ = fmt::format( "http://{}:{}/rootDesc.xml", Config::instance()->httpHost(), Config::instance()->httpPort() );
    _ssdp_server = std::shared_ptr< ssdp::SSDPServerImpl >( new ssdp::SSDPServerImpl(
        Config::instance()->uuid(),
        Config::instance()->multicastAddress(),
        Config::instance()->multicastPort(),
        std::map< std::string, std::string >( {
            { Config::instance()->uuid(), device_uri_ },
            { ssdp::NS_ROOT_DEVICE, device_uri_ },
            { ssdp::NS_MEDIASERVER, device_uri_ },
            { ssdp::NS_CONTENT_DIRECTORY, device_uri_ }
        } )
    ) );
    //add the event listener
    _ssdp_server->signal_detected.connect( sigc::mem_fun( this, &SquawkServer::ssdp_event ) );

    //clean the devices
    _ssdp_devices_thread = std::unique_ptr<std::thread> ( new std::thread( &SquawkServer::cleanup_upnp_devices, this ) );

    /** Rescan the Media Directory **/
    if(Config::instance()->rescan) {
        import_media_directory();
    }

    _ssdp_server->announce();
    _ssdp_server->search();
}
void SquawkServer::stop() {
    _ssdp_devices_thread_run = false;
    _ssdp_devices_thread->join();
    //TODO _ssdp_server->stop();
    //web_server->stop();
}

bool SquawkServer::import_media_directory() {
    static std::thread _file_parser_thread;
    if ( ! _file_parser_thread.joinable() ) {
        //static std::shared_ptr< squawk::Scanner > _file_parser;
        static std::shared_ptr< squawk::Scanner > _file_parser;
        _file_parser_thread = std::thread( &Scanner::parse, _file_parser, Config::instance()->mediaDirectories() );
        return true;
    } else return false;
}

void SquawkServer::cleanup_upnp_devices() {
    while ( _ssdp_devices_thread_run ) { {
            std::lock_guard<std::mutex> _ssdp_devices_guard( _ssdp_devices_mutex );

            auto iter_ = _ssdp_devices.begin();
            auto end_iter_ = _ssdp_devices.end();
            while( iter_ != end_iter_ ) {
                if ( iter_->second.timeout() ) {
                    CLOG(INFO, "upnp") << "(Device Timeout) :" << iter_->second.friendlyName();
                    _ssdp_devices.erase( iter_++ );
                } else {
                    ++iter_;
                }
            }//while iter ssdp devices
        }//context for the mutex
        std::this_thread::sleep_for ( std::chrono::milliseconds ( 5000 ) );
    }//while forever
}

void SquawkServer::ssdp_event( ssdp::SSDP_EVENT_TYPE type, std::string, ssdp::SsdpEvent device ) {

    std::cout << "event " << device << std::endl;

    //nt            device type
    //nts           event type
    //usn           device endpoint uuid
    //cache_control cache timer
    std::string _rootdevice_usn;
    size_t uuid_position_end_ = device.usn.find( "::" );
    if( uuid_position_end_ != std::string::npos ) {
        _rootdevice_usn = device.usn.substr(0, uuid_position_end_ );
    } else if( device.nt == ssdp::NS_ROOT_DEVICE ) {
        _rootdevice_usn = device.usn;
    } else if( device.nt == device.usn ) {
        _rootdevice_usn = device.usn;
    } else {
        CLOG(ERROR, "upnp") << "can not parse usn: " << device.nt << "=" << device.usn;
    }

    if( device.nt == ssdp::NS_ROOT_DEVICE ) {
        if( type == ssdp::SSDP_EVENT_TYPE::BYE ) {
            if( _ssdp_devices.find( _rootdevice_usn ) != _ssdp_devices.end() ) {
                CLOG(INFO, "upnp") << "rootdevice (bye) " << _ssdp_devices[ _rootdevice_usn ].friendlyName();
                std::lock_guard<std::mutex> _ssdp_devices_guard( _ssdp_devices_mutex );
                _ssdp_devices.erase( _ssdp_devices.find( _rootdevice_usn ) );
            }
        } else if( _ssdp_devices.find( _rootdevice_usn ) != _ssdp_devices.end() ) {
            //update timestamp
            CLOG(DEBUG, "upnp") << "rootdevice (reanc) " << _ssdp_devices[ _rootdevice_usn ].friendlyName();
            _ssdp_devices[ _rootdevice_usn ].touch();

        } else {
            //create new rootdevice
            try {
                upnp::UpnpDevice device_ = upnp::deviceDescription( device );
                device_.touch();
                device_.timeout( device.cache_control );

                CLOG(INFO, "upnp") << "rootdevice (anc) " << device_.friendlyName();

                std::lock_guard<std::mutex> _ssdp_devices_guard( _ssdp_devices_mutex );
                _ssdp_devices[ _rootdevice_usn ] = device_;

            } catch( commons::xml::XmlException & ex ) {
                CLOG(ERROR, "upnp") << "XML Parse Exception (" << ex.code() << ") " << ex.what();
            } catch( ... ) {
                CLOG(ERROR, "upnp") << device;
            }
        }//fi listener type
    }//fi root device
}//lambda event

void SquawkServer::create_db() {
    auto database_ = db::make_connection( Config::instance()->databaseFile() );
    //create tables if they dont exist
    for ( auto & stmt : CREATE_STATEMENTS ) {
        LOG ( TRACE ) << "create table:" << stmt;

        try {
            db::exec ( database_, stmt );

        } catch ( db::DbException & e ) {
            LOG ( FATAL ) << "Create table, Exception:" << e.code() << "-> " << e.what();
            throw upnp::UpnpException ( e.code(), "DbException: " +  std::string ( e.what() ) );
        }
    }
}

}//namespace squawk
