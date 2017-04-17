#include "textutils.h"

#include <algorithm>
#include <regex>
#include <iostream> //TODO remove

#include <boost/algorithm/string/case_conv.hpp>

namespace squawk {
namespace utils {

std::string TextUtils::clean_isbn( std::string isbn ) {
    if( isbn.find( "urn:isbn:" ) == 0 ) {
        isbn = isbn.substr( 9 );
    }
    isbn.erase(std::remove_if(isbn.begin(), isbn.end(),
        [](char x){return !std::isalpha(x) && !std::isdigit(x);}), isbn.end());

    if( isbn.find( "ISBN" ) != std::string::npos ) {
        return isbn.substr( isbn.find( "ISBN" ) + 4 );
    } else return isbn;
}

const std::string TextUtils::strip_html( const std::string& body ) {
    return ( std::regex_replace ( body, std::regex ( "<[^>]*>" ), " " ) );
}

const std::string TextUtils::find_isbn ( const std::string& body ) {
    std::string _body = body;
    static std::array< std::regex, 2 > _reg_list = std::array< std::regex, 2 > ({
        std::regex( "([\\d]+[- ·]?[\\d]*[- ·]{1}[\\d]*[- ·]{1}[\\d]*[- ·]{1}[\\dX]*)" ),
        std::regex( "([\\d]{10,13})" )
    });

    boost::to_upper( _body );
    size_t _isbn_pos = _body.find( "ISBN" );
    if(  _isbn_pos != std::string::npos ) {
        const std::string _snipped = remove_special_characters( _body.substr( _isbn_pos, 100 ) );
        std::cout << "\nsearch for ISBN in subsctring \"" << _snipped << "\"" << std::endl; //TODO
        for( auto& __re : _reg_list ) {
            std::smatch match;
            if (std::regex_search( _snipped , match, __re ) && match.size() > 1)
            { return match.str( 1 ); }
        }
    }
    return "";
}

const std::string TextUtils::remove_special_characters( const std::string& body ) {
    std::string _result = body;
    std::replace_if( _result.begin(), _result.end(),
        [](char c) { return c == '\n'; }, '_' );
    _result.erase( std::remove_if( _result.begin(), _result.end(),
        [](char c) { return !( std::isalnum( c ) || std::isalpha(c ) || c == '-' || c == ':' || c == ' ' || c == '_' ); } ),
        _result.end());
    return _result;
}

}//namespace squawk
}//namespace utils
