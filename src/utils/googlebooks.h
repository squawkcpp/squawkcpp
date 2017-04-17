#ifndef GOOGLEBOOKS_H
#define GOOGLEBOOKS_H

#include <algorithm>
#include <string>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "bookresultitem.h"

namespace utils {

class GoogleBooks {
public:
    static BookResultItem query( const std::string& name, const std::string& isbn );
    static std::string parse_query( const std::string& title, const std::string& isbn, const std::string& body );
    static BookResultItem parse_volumes( const std::string& body );
private:
    GoogleBooks();
    static const std::string query_uri;
    static const std::string volumes_uri;

    static std::string clean_title ( const std::string& title ) {
        std::string s;
        if( title.find( "[" ) == 0 ) {
            s = title.substr( title.find( "]" ) + 1 );
        } else if( title.find( "(" ) == 0 ) {
            s = title.substr( title.find( ")" ) + 1 );
        } else s = title;
        boost::to_lower( s );
        boost::trim( s );
        s.erase(std::remove_if(s.begin(), s.end(),
                   [](char x){return !std::isalpha(x) && !std::isdigit(x);}),
                   s.end());
        return s;
    }

    static std::string clean_isbn( std::string isbn ) {
        if( isbn.find( "urn:isbn:" ) == 0 ) {
            isbn = isbn.substr( 9 );
        }
        isbn.erase(std::remove_if(isbn.begin(), isbn.end(),
                                  [](char x){return !std::isalpha(x) && !std::isdigit(x);}),
                   isbn.end());
        return isbn;
    }
};
}//namespace utils
#endif // GOOGLEBOOKS_H
