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

#ifndef PDFPARSER_H
#define PDFPARSER_H

#include <string>

#include "gtest/gtest_prod.h"

//TODO add namespace
class PdfParser {
public:
    /**
     * @brief parse PDF file and return ISBN.
     * @param filename
     * @return isbn
     */
    static std::string parsePdf( const std::string & filename );

private:
    FRIEND_TEST( IsbnParserTest, TestRegexpIsbnOnly );
    FRIEND_TEST( IsbnParserTest, TestRegexp );
    static const std::string isbn_regex;
    static const std::string isbn_number_only_regex;

    static const std::string remove_special_characters( const std::string& body );
};

#endif // PDFPARSER_H
