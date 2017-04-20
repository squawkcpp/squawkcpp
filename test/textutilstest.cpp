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

#include "../src/utils/textutils.h"

#include <gtest/gtest.h>

namespace squawk {
namespace utils {

TEST(TextUtilsTest, clean_isbn ) {
    EXPECT_EQ( "123123123123", TextUtils::clean_isbn( "123-123-123-123" ) );
    EXPECT_EQ( "123123123123", TextUtils::clean_isbn( "123 123 123 123" ) );
    EXPECT_EQ( "X123123123123", TextUtils::clean_isbn( "X 123 123 123 123" ) );
    EXPECT_EQ( "9780544715301", TextUtils::clean_isbn( "urn:isbn:9780544715301" ) );
    EXPECT_EQ( "9780544715301", TextUtils::clean_isbn( "epub::ISBN 9780544715301" ) );
    EXPECT_EQ( "9780231168908", TextUtils::clean_isbn( "ISBN-978-0-231-16890-8" ) );
}
TEST( TextUtilsTest, strip_html ) {
    std::string _body = "<html><body><a href="">text<a/></body></html>";
    EXPECT_EQ( "   text   ", TextUtils::strip_html( _body ) );
}
//TEST( EpubParserTest, clean_isbn ) {
//    EXPECT_EQ( "-978-0-231-53758-2", EpubParser::clean_isbn( "eISBN-978-0-231-53758-2" ) );
//}

TEST(TextUtilsTest, DISABLED_find_isbn ) {
    EXPECT_EQ( "978-0-9922794-3-1", TextUtils::find_isbn( "xxx ISBN: 978-0-9922794-3-1 xxxx" ) );
    EXPECT_EQ( "1-59327-580-3 ", TextUtils::find_isbn( "xxx ISBN-10: 1-59327-580-3 bla bla" ) );
    EXPECT_EQ( "0-415-93551-2 ", TextUtils::find_isbn( "xxx ISBN 0-415-93551-2 (pbk.)" ) );
    EXPECT_EQ( "978-0-786-75107-5", TextUtils::find_isbn( "f Congress Control Number: 2008943eISBN : 978-0-786-75107-5" ) );
    EXPECT_EQ( "0 240 51587 0", TextUtils::find_isbn( "ISBN 0 240 51587 0" ) );
    EXPECT_EQ( "978-0-13-438358-3", TextUtils::find_isbn( "ISBN-13: 978-0-13-438358-3" ) );
    EXPECT_EQ( "978-1-118-83207-3", TextUtils::find_isbn( "ISBN: 978-1-118-83207-3" ) );
    EXPECT_EQ( "978·0·321·54372·1", TextUtils::find_isbn( "ISBN-13: 978·0·321·54372·1" ) );
    EXPECT_EQ( "978-1-4987-1956-8", TextUtils::find_isbn( "International Standard Book Number-13: 978-1-4987-1956-8" ) );
    EXPECT_EQ( "978-0-231-15344-7", TextUtils::find_isbn( "isbn 978-0-231-15344-7" ) );
    EXPECT_EQ( "978-1-4027-9847-4", TextUtils::find_isbn( "ISBN 978-1-4027-9847-4" ) );
    EXPECT_EQ( "9780399578069", TextUtils::find_isbn( "Hardcover ISBN 9780399578069" ) );
    EXPECT_EQ( "978-1-4842-2495-3", TextUtils::find_isbn( "ISBN-13 (pbk): 978-1-4842-2495-3" ) );
    EXPECT_EQ( "978-3-95843-037-2", TextUtils::find_isbn( "ISBN 978-3-95843-037-2" ) );
}

TEST(TextUtilsTest, remove_special_characters ) {
    EXPECT_EQ("ISBN: 123-123-123-123-13 ", TextUtils::remove_special_characters( "ISBN: 123-123-123-123-13 " ) );
    EXPECT_EQ("ISBN: 123-123-123-123-X", TextUtils::remove_special_characters( "ISBN: 123-123-123-123-X" ) );
    EXPECT_EQ("ISBN: 123-123-123-123-_13 ", TextUtils::remove_special_characters( "ISBN: 123-123-123-123-\n13 " ) );
}

}//namespace amazon
}//namespace squawk
