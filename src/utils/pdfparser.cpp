/*
    get ISBN from pdf file.
    Copyright (C) 2014  <e.knecht@netwings.ch>

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

#include <iostream> //TODO remove

#include "xml.h"

#include "poppler/cpp/poppler-document.h"
#include "poppler/cpp/poppler-page.h"
#include "re2/re2.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include "pdfparser.h"
#include "textutils.h"

const std::string PdfParser::isbn_regex = "([\\d]+[- ·]?[\\d]*[- ·]{1}[\\d]*[- ·]{1}[\\d]*[- ·]{1}[\\dX]*)";
const std::string PdfParser::isbn_number_only_regex = "([\\d]{10,13})";

std::string PdfParser::parsePdf( const std::string & filename ) {

    std::cerr << "search ISBN in: " << filename << std::endl;
    poppler::document * doc = poppler::document::load_from_file  ( filename );
    if( doc == nullptr ) {
        std::cerr << "unable to open pdf document" << std::endl;
        return "";
    }

    for( size_t i=0U; i<static_cast< size_t >( doc->pages() ); i++ ) {
        poppler::page * p = doc->create_page( i );
        std::string _page_string = p->text().to_latin1();
//TODO        std::cout << "PAGE:" << i << " -> " <<_page_string << std::endl;
        std::string _isbn = squawk::utils::TextUtils::find_isbn( _page_string );
        if( ! _isbn.empty() ) {
            std::cout << "found isbn: " << _isbn << std::endl;
            return _isbn;
        }
    }
    std::cout << "\nno ISBN found in: " << filename << std::endl;
    return "";
}

const std::string PdfParser::remove_special_characters( const std::string& body ) {
    std::string _result = body;
    std::replace_if( _result.begin(), _result.end(),
        [](char c) { return c == '\n'; }, '_' );
    _result.erase( std::remove_if( _result.begin(), _result.end(),
        [](char c) { return !( std::isalnum( c ) || std::isalpha(c ) || c == '-' || c == ':' || c == ' ' || c == '_' ); } ),
        _result.end());
    return _result;
}
