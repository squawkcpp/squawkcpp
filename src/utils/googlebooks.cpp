#include "googlebooks.h"

#include <iostream> //TODO remove
#include <sstream>

#include "http/httpclient.h"
#include "fmt/format.h"
#include "rapidjson/document.h"

namespace utils {

const std::string GoogleBooks::query_uri = "https://www.googleapis.com/books/v1/volumes?q={0}:{1}&key={2}&fields=items(id,volumeInfo/title,volumeInfo/industryIdentifiers)";
const std::string GoogleBooks::volumes_uri = "https://www.googleapis.com/books/v1/volumes/{0}?key={1}";

BookResultItem GoogleBooks::query( const std::string& title, const std::string& isbn ) {
    std::stringstream _ss;
    std::string _volumes_id = "";
    if( ! isbn.empty() ) {
        std::cout << fmt::format( query_uri, "isbn", clean_isbn ( isbn ), "AIzaSyB9vDclrjwujIvHFPVNyGLb7KZ5R8vMRmc" ) << std::endl;
        http::get( fmt::format( query_uri, "isbn", clean_isbn ( isbn ), "AIzaSyB9vDclrjwujIvHFPVNyGLb7KZ5R8vMRmc" ), _ss );
        _volumes_id = parse_query( title, isbn, _ss.str() );
    }
    if( _volumes_id.empty() && ! title.empty() ) {
        _ss.str( "" ); _ss.clear();
        std::cout << fmt::format( query_uri, "title", http::utils::UrlEscape::urlEncode( title ), "AIzaSyB9vDclrjwujIvHFPVNyGLb7KZ5R8vMRmc" ) << std::endl;
        http::get( fmt::format( query_uri, "title", http::utils::UrlEscape::urlEncode( title ), "AIzaSyB9vDclrjwujIvHFPVNyGLb7KZ5R8vMRmc" ), _ss );
        _volumes_id = parse_query( title, isbn, _ss.str() );
    }
    if( ! _volumes_id.empty() ) {
        std::stringstream _ss_volumes;
        http::get( fmt::format( volumes_uri, _volumes_id, "AIzaSyB9vDclrjwujIvHFPVNyGLb7KZ5R8vMRmc" ), _ss_volumes );
        return parse_volumes( _ss_volumes.str() );
    }
    return BookResultItem{ 404, 0, "", "", "", "", "", "", "" };
}

std::string GoogleBooks::parse_query( const std::string& title, const std::string& isbn, const std::string& body ) {
    std::cout << "parse body: '" << body << "'" << std::endl;
    rapidjson::Document document;
    document.Parse( body.c_str() );

    if( document.HasMember( "items" ) ) {
        const auto& _items = document[ "items" ];
        if( _items.IsArray() ) {
            for( auto& _item : _items.GetArray() ) {
                bool _found = false;
                std::string _id = _item["id"].GetString();
                auto& _volume_info = _item["volumeInfo"];
                if( _volume_info.IsObject() ) {
                    //TODO
                    std::cout << "check if titles match: " << title << "==" << _volume_info["title"].GetString() << std::endl;
                    if( isbn.empty() && clean_title( title ) == clean_title( _volume_info["title"].GetString() ) ) { //TODO
                        std::cout << "found title match: " << title << "==" << _volume_info["title"].GetString() << std::endl;
                        _found = true;
                    } else {
                        if( _volume_info.HasMember ( "industryIdentifiers" ) ) {
                            auto& _industryIdentifiers = _volume_info["industryIdentifiers"];
                            if( _industryIdentifiers.IsArray() ) {
                                for( auto& _identifier : _industryIdentifiers.GetArray() ) {
                                    if( clean_isbn ( isbn ) == clean_isbn ( _identifier["identifier"].GetString() ) ) {
                                        _found = true;
                                    }
                                }
                            }
                        }
                    }
                }
                if( _found ) return _id;
            }
        }
    }
    return "";
}

BookResultItem GoogleBooks::parse_volumes( const std::string& body ) {
    std::cout << body << std::endl;
    rapidjson::Document document;
    document.Parse( body.c_str() );

    BookResultItem _item;
    const auto& _volumes = document[ "volumeInfo" ];
    if ( _volumes.HasMember( "title" ) && _volumes["title" ].IsString() )
    { _item.title             = _volumes["title"].GetString(); }
    if ( _volumes.HasMember( "authors" ) && _volumes["authors" ].IsArray() )
    { _item.author            = _volumes["authors"].GetArray()[0].GetString(); }
    if ( _volumes.HasMember( "description" ) && _volumes["description" ].IsString() )
    { _item.description       = _volumes["description"].GetString(); }
    if ( _volumes.HasMember( "publishedDate" ) && _volumes["publishedDate" ].IsString() )
    { _item.publicationDate   = _volumes["publishedDate"].GetString(); }
    if ( _volumes.HasMember( "publisher" ) && _volumes["publisher" ].IsString() )
    { _item.publisher         = _volumes["publisher"].GetString(); }
    if ( _volumes.HasMember( "imageLinks" ) && _volumes["imageLinks" ].IsObject() ) {
        auto _image_links = _volumes["imageLinks" ].GetObject();
        if( _image_links.HasMember( "large" ) ) {
            _item.coverUri          = _image_links["large"].GetString();
        } else if( _image_links.HasMember( "medium" ) ) {
            _item.coverUri          = _image_links["medium"].GetString();
        } else if( _image_links.HasMember( "small" ) ) {
            _item.coverUri          = _image_links["small"].GetString();
        } else if( _image_links.HasMember( "thumbnail" ) ) {
            _item.coverUri          = _image_links["thumbnail"].GetString();
        } else if( _image_links.HasMember( "smallThumbnail" ) ) {
            _item.coverUri          = _image_links["smallThumbnail"].GetString();
        } else {
            _item.coverUri          = "";
        }
    }
    if ( _volumes.HasMember( "industryIdentifiers" ) && _volumes["industryIdentifiers" ].IsArray() )
    _item.isbn              = _volumes["industryIdentifiers"].GetArray()[0].GetObject()["identifier"].GetString();
    return _item;
}

//API KEY
//TODO //AIzaSyB9vDclrjwujIvHFPVNyGLb7KZ5R8vMRmc
}//namespace utils
