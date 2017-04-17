#include "tmdb.h"

#include <fstream>
#include <iostream>
#include <time.h>

#include "fmt/format.h"
#include "rapidjson/document.h"

#include "http/httpclient.h"

namespace tmdb {

std::string Tmdb::BACKDROP_PATH     =   "backdrop_path";
std::string Tmdb::FIRST_AIR_DATE    =   "first_air_date";
std::string Tmdb::ID                =   "id";
std::string Tmdb::NAME              =   "name";
std::string Tmdb::OVERVIEW          =   "overview";
std::string Tmdb::POSTER_PATH       =   "poster_path";

std::string Tmdb::series ( const std::string& api_key, const std::string& name ) {
    http::HttpClient< http::Http>  client_ ( "api.themoviedb.org", "http" );
    http::Request request_ ( fmt::format( "/3/search/tv?api_key={0}&query={1}", api_key, http::utils::UrlEscape::urlEncode( name ) ) );
    std::stringstream _sstream;
    client_.get ( request_, _sstream );
    std::cout << "TMDB RESULT: " << _sstream.str() << std::endl;
    return _sstream.str();
}

std::vector < Series > Tmdb::parse_series( const std::string& result ) {
    std::vector < Series > _series;

    std::cout << result.c_str() << std::endl;
    rapidjson::Document document;
    document.Parse( result.c_str() );

    const auto& results = document[ "results" ];
    assert( results.IsArray() );
    for ( auto itr = results.Begin(); itr != results.End(); ++itr ) {
        Series _serie;
        for (auto _obj = itr->MemberBegin(); _obj != itr->MemberEnd(); ++_obj) {
            if( _obj->name.GetString() == BACKDROP_PATH && _obj->value.IsString() ) {
                _serie.backdrop = _obj->value.GetString();

            } else if( _obj->name.GetString() == OVERVIEW && _obj->value.IsString() ) {
                _serie.comment = _obj->value.GetString();

            } else if( _obj->name.GetString() == POSTER_PATH && _obj->value.IsString() ) {
                _serie.poster = _obj->value.GetString();

            } else if( _obj->name.GetString() == FIRST_AIR_DATE && _obj->value.IsString() ) {
                struct tm _tm = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                if ( !strptime( _obj->value.GetString(), "%Y-%m-%d", &_tm ) ) {
                    std::cout << "ERROR: tmdb date is invalid: " << _obj->value.GetString() << std::endl;
                } else {
                    time_t tmp_time_ = mktime ( &_tm );
                    _serie.first_air_date = tmp_time_;
                }
            } else if( _obj->name.GetString() == NAME && _obj->value.IsString() ) {
                _serie.title = _obj->value.GetString();

            } else if( _obj->name.GetString() == ID && _obj->value.IsInt64() ) {
                _serie.tmdb_id = _obj->value.GetInt64();
            }
        }
        _series.push_back( _serie );
    }
    return _series;
}
void Tmdb::get( const std::string& uri, const std::string& path ) {
    std::ofstream _sstream( path );
    auto _response = http::get ( fmt::format( "http://image.tmdb.org/t/p/original{}", uri ), _sstream ); //TODO check response
    std::cout << "get cover: " << path << " == " << static_cast< int >( _response.status() ) << std::endl;
}
}//namespace tmdb
