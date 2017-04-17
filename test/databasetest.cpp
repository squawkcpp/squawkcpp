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

#include <boost/filesystem.hpp>

#include "../src/database.h"

#include <gtest/gtest.h>

TEST(DatabaseTest, create_database) {
    if( boost::filesystem::exists( "/tmp/testdatabase1.db" ) )
        boost::filesystem::remove( "/tmp/testdatabase1.db" );
    auto database = db::make_connection( "/tmp/testdatabase1.db" );
    try {
        ASSERT_TRUE( db::exec( database, "CREATE TABLE TBL_TEST ( a, b, c, d );" ) );
        ASSERT_TRUE( db::exec( database, "INSERT INTO TBL_TEST( a, b, c, d ) VALUES (?,?,?,?);", 1, 2, 3, 4 ) );
        //test number of updatet rows
        size_t changes_ = db::changes( database );
        EXPECT_EQ( 1U, changes_ );
        //test last inserted row id
        size_t last_row_id_= db::last_insert_rowid( database );
        EXPECT_EQ( 1U, last_row_id_ );
        //test row count
        size_t row_count_ = 0;
        db::exec( database, "SELECT count(*) from TBL_TEST;", [&row_count_] (auto statement) {
            row_count_ = db::get< unsigned long >( statement, 0 );
        });
        EXPECT_EQ( 1U, row_count_ );
        //test get row
        bool row_found_ = false;
        db::exec( database, "SELECT a,b,c,d from TBL_TEST where ROWID=?", [&row_found_](auto statement) {
            row_found_ = true;
            EXPECT_EQ( 1, db::get< int >( statement, 0 ) );
            EXPECT_EQ( 2, db::get< int >( statement, 1 ) );
            EXPECT_EQ( 3, db::get< int >( statement, 2 ) );
            EXPECT_EQ( 4, db::get< int >( statement, 3 ) );
        }, last_row_id_ );
        ASSERT_TRUE( row_found_ );
    } catch( db::DbException& x ) {
        std::cout << "E:" << db::errmsg( database ) << std::endl;
    }
}

TEST(DatabaseTest, create_database_vector) {
    if( boost::filesystem::exists( "/tmp/testdatabase2.db" ) )
        boost::filesystem::remove( "/tmp/testdatabase2.db" );
    auto database = db::make_connection( "/tmp/testdatabase2.db" );
    try {
        db::exec( database, "CREATE TABLE TBL_TEST ( a, b, c, d );" );
        std::vector< int > v_ = {1,2,3,4};
        db::exec( database, "INSERT INTO TBL_TEST( a, b, c, d ) VALUES (?,?,?,?)", v_ );
        //test number of updatet rows
        size_t changes_ = db::changes( database );
        EXPECT_EQ( 1U, changes_ );
        //test last inserted row id
        size_t last_row_id_= db::last_insert_rowid( database );
        EXPECT_EQ( 1U, last_row_id_ );
        //test row count
        size_t row_count_ = 0;
        db::exec( database, "SELECT count(*) from TBL_TEST", [&row_count_] (auto statement) {
            row_count_ = db::get< unsigned long >( statement, 0 );
        });
        EXPECT_EQ( 1U, row_count_ );
        //test get row
        bool row_found_ = false;
        db::exec( database, "SELECT a,b,c,d from TBL_TEST where ROWID=?", [&row_found_](auto statement) {
            row_found_ = true;
            EXPECT_EQ( 1, db::get< int >( statement, 0 ) );
            EXPECT_EQ( 2, db::get< int >( statement, 1 ) );
            EXPECT_EQ( 3, db::get< int >( statement, 2 ) );
            EXPECT_EQ( 4, db::get< int >( statement, 3 ) );
        }, last_row_id_ );
        ASSERT_TRUE( row_found_ );
    } catch( db::DbException& x ) {
        std::cout << "E:" << db::errmsg( database ) << std::endl;
    }
}
