/*
    <squawk multimedia server>
    Copyright (C) 2017  <etienne knecht> <squawkcpp@gmail.com>

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

#ifndef SQUAWKSERVER_H
#define SQUAWKSERVER_H

#include "http/server.h"
#include "http/httpserver.h"

#include "config.h"
#include "ssdp/ssdpserverimpl.h"
#include "upnp/upnp.h"

namespace squawk {

//forward declaration of class - make it a friend in DidlObject.
class UpnpContentDirectoryDao;
//forward declaration of class - make it a friend in DidlObject.
class UpnpContentDirectoryParser;

/**
 * @brief The SquawkServer Class
 */
class SquawkServer {
  public:
    SquawkServer(SquawkServer const&) = delete;
    SquawkServer & operator=(SquawkServer const&) = delete;
    virtual ~SquawkServer() {}

    static std::shared_ptr< SquawkServer > instance() {
        static auto _server = std::shared_ptr< SquawkServer >( new SquawkServer() );
        return _server;
    }

    /**
     * @brief Start the Server.
     */
    void start ();

    /**
     * @brief Stop the Server.
     */
    void stop();

    void create_db ();

    std::map< std::string, upnp::UpnpDevice > upnp_devices () {
        return _ssdp_devices;
    }

    bool import_media_directory();

  private:
    SquawkServer() {}

    /* CREATE TABLES */
    const std::list< std::string > CREATE_STATEMENTS {
        "CREATE TABLE IF NOT EXISTS tbl_cds_object( cls, parent_id, size, title, path, mime_type, mtime, rating, year, track, width, height, color_depth, bitrate, bits_per_sample, nr_audio_channels, duration, sample_frequency, disc, isbn, playback_count, object_update_id, contributor, artist, author, publisher, genre, album, comment, series_title, season, episode, profile, last_playback_time, import, timestamp, tmdb_id );",
        "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexObjectPath ON tbl_cds_object(path);",
        "CREATE TABLE IF NOT EXISTS tbl_cds_mapping( parent, child );",

        "CREATE TABLE IF NOT EXISTS tbl_cds_artist( clean_name, name, import );",
        "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexArtistCleanName ON tbl_cds_artist(clean_name);",
        "CREATE TABLE IF NOT EXISTS tbl_cds_resource( ref_obj, protocol_info, path, bitrate, bits_per_sample, color_depth, dlna_profile, duration, framerate, mime_type, nr_audio_channels, resolution, sample_frequency, size );",
        "CREATE INDEX IF NOT EXISTS UniqueIndexResourceRef ON tbl_cds_resource(ref_obj);",
        "CREATE TABLE IF NOT EXISTS tbl_cds_album_art_uri( ref_obj, path, uri, profile);",
        "CREATE INDEX IF NOT EXISTS UniqueIndexAlbumArtUriRef ON tbl_cds_album_art_uri(ref_obj);",
    };

    void cleanup_upnp_devices();
    void ssdp_event( ssdp::SSDP_EVENT_TYPE, std::string, ssdp::SsdpEvent device );

    std::map< std::string, upnp::UpnpDevice > _ssdp_devices;
    std::mutex _ssdp_devices_mutex;
    std::unique_ptr<std::thread> _ssdp_devices_thread;
    bool _ssdp_devices_thread_run = true;

    std::shared_ptr< ssdp::SSDPServerImpl > _ssdp_server;
    std::shared_ptr< http::Server< http::HttpServer > > web_server;
};
}//namespace squawk
#endif // SQUAWKSERVER_H
