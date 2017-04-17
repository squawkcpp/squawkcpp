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

#include "re2/re2.h"

#include "../src/utils/pdfparser.h"

#include <gtest/gtest.h>


TEST( IsbnParserTest, TestRegexpIsbnOnly ) {

    std::string dummy_, isbn_;
///    const std::string isbn_regex = "(([\\d]*-? ?·?)*X?)";
    const std::string isbn_regex = "([\\d]+[- ]?[\\d]*[- ]{1}[\\d]*[- ]{1}[\\d]*[- ]{1}[\\dX]*)";

    EXPECT_TRUE( RE2::FullMatch( "99921-58-10-7", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "99921-58-10-7" ) );

    EXPECT_TRUE( RE2::PartialMatch( "9971-5-0210-0", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "9971-5-0210-0" ) );

    EXPECT_TRUE( RE2::PartialMatch( "960-425-059-0", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "960-425-059-0" ) );

    EXPECT_TRUE( RE2::PartialMatch( "80-902734-1-6", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "80-902734-1-6" ) );

    EXPECT_TRUE( RE2::PartialMatch( "85-359-0277-5", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "85-359-0277-5" ) );

    EXPECT_TRUE( RE2::PartialMatch( "1-84356-028-3", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "1-84356-028-3" ) );

    EXPECT_TRUE( RE2::PartialMatch( "0-684-84328-5", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "0-684-84328-5" ) );

    EXPECT_TRUE( RE2::PartialMatch( "0-8044-2957-X", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "0-8044-2957-X" ) );

    EXPECT_TRUE( RE2::PartialMatch( "0-85131-041-9", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "0-85131-041-9" ) );

    EXPECT_TRUE( RE2::PartialMatch( "0-943396-04-2", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "0-943396-04-2" ) );

    EXPECT_TRUE( RE2::PartialMatch( "0-9752298-0-X", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "0-9752298-0-X" ) );

    EXPECT_TRUE( RE2::PartialMatch( "0 9752298 0 X", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "0 9752298 0 X" ) );

    EXPECT_TRUE( RE2::PartialMatch( "lakdfj 0-9752298-0-X aksdfj ", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "0-9752298-0-X" ) );

    EXPECT_TRUE( RE2::PartialMatch( "xxx ISBN: 978-0-9922794-3-1 xxxx", isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "978-0-9922794-3-1" ) );
}

TEST(IsbnParserTest, TestRegexp) {

    std::string dummy_, isbn_;

    EXPECT_TRUE( RE2::PartialMatch( "xxx ISBN: 978-0-9922794-3-1 xxxx", PdfParser::isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "978-0-9922794-3-1" ) );

    EXPECT_TRUE( RE2::PartialMatch( "xxx ISBN-10: 1-59327-580-3 xxxx", PdfParser::isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "1-59327-580-3 " ) );

    EXPECT_TRUE( RE2::PartialMatch( "xxx ISBN 0-415-93551-2 (pbk.)", PdfParser::isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "0-415-93551-2 " ) );

    EXPECT_TRUE( RE2::PartialMatch( "f Congress Control Number: 2008943eISBN : 978-0-786-75107-5", PdfParser::isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "978-0-786-75107-5" ) );


    EXPECT_TRUE( RE2::PartialMatch( "ISBN 0 240 51587 0", PdfParser::isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "0 240 51587 0" ) );

    EXPECT_TRUE( RE2::PartialMatch( "ISBN-13: 978-0-13-438358-3", PdfParser::isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "978-0-13-438358-3" ) );

    EXPECT_TRUE( RE2::PartialMatch( "ISBN: 978-1-118-83207-3", PdfParser::isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "978-1-118-83207-3" ) );

    EXPECT_TRUE( RE2::PartialMatch( "ISBN-13: 978·0·321·54372·1", PdfParser::isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "978·0·321·54372·1" ) );

    EXPECT_TRUE( RE2::PartialMatch( "International Standard Book Number-13: 978-1-4987-1956-8", PdfParser::isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "978-1-4987-1956-8" ) );

    EXPECT_TRUE( RE2::PartialMatch( "isbn 978-0-231-15344-7", PdfParser::isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "978-0-231-15344-7" ) );

    EXPECT_TRUE( RE2::PartialMatch( "ISBN 978-1-4027-9847-4", PdfParser::isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "978-1-4027-9847-4" ) );

    EXPECT_TRUE( RE2::PartialMatch( "Hardcover ISBN 9780399578069", PdfParser::isbn_number_only_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "9780399578069" ) );

    EXPECT_TRUE( RE2::PartialMatch( "ISBN-13 (pbk): 978-1-4842-2495-3", PdfParser::isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "978-1-4842-2495-3" ) );

    EXPECT_TRUE( RE2::PartialMatch( "ISBN 978-3-95843-037-2", PdfParser::isbn_regex, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string( "978-3-95843-037-2" ) );

}

