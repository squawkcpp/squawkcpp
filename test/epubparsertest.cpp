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

#include <string>
#include <map>

#include "../src/utils/epubparser.h"
#include "re2/re2.h"

#include <gtest/gtest.h>

namespace epub {

TEST( EpubParserTest, parse_isbn_regex_identifier ) {
    std::string _result;
    for( auto& regex : EpubParser::isbn_regex_list ) {
        if( RE2::FullMatch ( "<dc:identifier opf:scheme=\"ISBN\">9781408868744</dc:identifier>", regex, &_result ) )
            break;
    }
    EXPECT_EQ( "9781408868744", _result );
}

TEST( EpubParserTest, parse_isbn_regex_identifier_scheme ) {
    std::string _result;
    for( auto& regex : EpubParser::isbn_regex_list ) {
        if( RE2::PartialMatch ( "abc \n\n <dc:identifier id=\"a9780446511070\" opf:scheme=\"ISBN\">9780446511070</dc:identifier>\ncdef", regex, &_result ) )
            break;
    }
    EXPECT_EQ( "9780446511070", _result );
}

TEST( EpubParserTest, parse_isbn_regex_identifier_id ) {
    std::string _result;
    for( auto& regex : EpubParser::isbn_regex_list ) {
        if( RE2::PartialMatch ( "abc \n<dc:identifier id=\"ISBN-978-0-231-16890-8\" opf:scheme=\"URI\">ISBN-978-0-231-16890-8</dc:identifier>\n \ncdef", regex, &_result ) )
            break;
    }
    EXPECT_EQ( "ISBN-978-0-231-16890-8", _result );
}

TEST( EpubParserTest, parse_isbn_regex_identifier_urn ) {
    std::string _result;
    for( auto& regex : EpubParser::isbn_regex_list ) {
        if( RE2::PartialMatch ( "abc \n<dc:identifier xmlns:dc=\"http://purl.org/dc/elements/1.1/\" id=\"bookid\">urn:isbn:9781449311599</dc:identifier>\n \ncdef", regex, &_result ) )
            break;
    }
    EXPECT_EQ( "9781449311599", _result );
}

TEST( EpubParserTest, parse_isbn_regex_identifier_source_urn ) {
    std::string _result;
    for( auto& regex : EpubParser::isbn_regex_list ) {
        if( RE2::PartialMatch ( "<dc:source id=\"src-id\">urn:isbn:9780544715271</dc:source>", regex, &_result ) )
            break;
    }
    EXPECT_EQ( "9780544715271", _result );
}

TEST( EpubParserTest, DISABLED_parse_isbn ) {
    std::string _filename = std::string(TESTFILES) + "GeographyofBliss_oneChapter.epub";
    EXPECT_EQ( "978-0-446-51107-0", EpubParser::get_isbn( _filename ) );
}
}//namespace epub
