/*
    Test the Squawk configuration parser.
    Copyright (C) 2013  <copyright holder> <email>

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

#include <fstream>
#include <string>
#include <map>

#include "../src/config.h"
#include <gtest/gtest.h>

namespace squawk {

TEST(ParseOptionsTest, TestConfigfile) {
    const char * options[3];
    options[0] = "-r";
    options[1] = "--config-file";
    options[2] = "/foo/bar.xml";
    squawk_config_ptr config = Config::instance( true );

    ASSERT_TRUE(config->parse(3, options));
    EXPECT_EQ(std::string("/foo/bar.xml"), config->configFile() );
    ASSERT_TRUE(config->rescan);
}

TEST(ParseOptionsTest, TestAllOptions) {
    const char * options[25];
    options[0] = "-r";
    options[1] = "--config-file";
    options[2] = "/foo/bar.xml";
    options[3] = "--logger";
    options[4] = "/foo/bar.logger";
    options[5] = "--media-directory";
    options[6] = "/foo/bar";
    options[7] = "--http-ip";
    options[8] = "127.0.0.1";
    options[9] = "--http-port";
    options[10] = "8080";
    options[11] = "--http-docroot";
    options[12] = "/foo/bar/docroot";
    options[13] = "--http-threads";
    options[14] = "20";
    options[15] = "--database-file";
    options[16] = "/foo/bar.db";
    options[17] = "--tmp-directory";
    options[18] = "/tmp/foo/bar";
    options[19] = "--local-address";
    options[20] = "127.0.0.1";
    options[21] = "--multicast-address";
    options[22] = "254.0.0.2";
    options[23] = "--multicast-port";
    options[24] = "1900";

    squawk_config_ptr config = Config::instance( true );

    ASSERT_TRUE(config->parse(25, options));
    EXPECT_EQ(std::string("/foo/bar.xml"), config->configFile() );
    ASSERT_TRUE(config->rescan);
    EXPECT_EQ(std::string("/foo/bar.logger"), config->logger() );
    EXPECT_EQ(std::string("/foo/bar"), config->mediaDirectories().front() );
    EXPECT_EQ(std::string("127.0.0.1"), config->httpAddress() );
    EXPECT_EQ(8080, config->httpPort() );
    EXPECT_EQ(std::string("/foo/bar/docroot"), config->docRoot() );
    EXPECT_EQ(std::string("/foo/bar.db"), config->databaseFile() );
    EXPECT_EQ(std::string("/tmp/foo/bar"), config->tmpDirectory() );
    EXPECT_EQ(std::string("127.0.0.1"), config->localListenAddress() );
    EXPECT_EQ(std::string("254.0.0.2"), config->multicastAddress() );
    EXPECT_EQ(1900, config->multicastPort() );
}

TEST(ParseOptionsTest, TestIncompleteOptions) {
    const char * options[23];
    options[0] = "-r";
    options[1] = "--config-file";
    options[2] = "/foo/bar.xml";
    options[3] = "--logger";
    options[4] = "/foo/bar.logger";
    options[5] = "--media-directory";
    options[6] = "/foo/bar";
    options[7] = "--http-ip";
    options[8] = "127.0.0.1";
    options[9] = "--http-port";
    options[10] = "8080";
    options[11] = "--http-docroot";
    options[12] = "/foo/bar/docroot";
    options[13] = "--http-threads";
    options[14] = "20";
    options[15] = "--database-file";
    options[16] = "/foo/bar.db";
    options[17] = "--local-address";
    options[18] = "127.0.0.1";
    options[19] = "--multicast-address";
    options[20] = "254.0.0.2";
    options[21] = "--multicast-port";
    options[22] = "1900";

    squawk_config_ptr config = Config::instance( true );

    ASSERT_TRUE(config->parse(23, options));
    ASSERT_FALSE(config->validate());
}

TEST(ParseOptionsTest, TestDefaultOptions) {
    const char * options[18];
    options[0] = "--media-directory";
    options[1] = "/foo/bar";
    options[2] = "--media-directory";
    options[3] = "/foo/bar2";
    options[4] = "--http-docroot";
    options[5] = "/foo/bar/docroot";
    options[6] = "--database-file";
    options[7] = "/foo/bar.db";
    options[8] = "--tmp-directory";
    options[9] = "/tmp/foo/bar";
    options[10] = "--config-file";
    options[11] = "/foo/bar.xml";
    options[12] = "--http-ip";
    options[13] = "127.0.0.1";
    options[14] = "--local-address";
    options[15] = "127.0.0.1";
    options[16] = "--http-bower";
    options[17] = "/path/bower";

    squawk_config_ptr config = Config::instance( true );

    ASSERT_TRUE(config->parse(18, options));
    ASSERT_TRUE(config->validate());
    EXPECT_EQ(std::string("/foo/bar.xml"), config->configFile() );

    EXPECT_EQ(2U, config->mediaDirectories().size() );

    EXPECT_EQ(std::string("/foo/bar"),  *(config->mediaDirectories().begin() ) );
    EXPECT_EQ(std::string("/foo/bar2"), *(++config->mediaDirectories().begin() ) );

    EXPECT_EQ(std::string("127.0.0.1"), config->httpAddress());
    EXPECT_EQ(8080, config->httpPort());
    EXPECT_EQ(std::string("/foo/bar/docroot"), config->docRoot() );
    EXPECT_EQ(std::string("/foo/bar.db"), config->databaseFile() );
    EXPECT_EQ(std::string("/tmp/foo/bar"), config->tmpDirectory() );
    EXPECT_EQ(std::string("127.0.0.1"), config->localListenAddress() );
    EXPECT_EQ(std::string("239.255.255.250"), config->multicastAddress() );
    EXPECT_EQ(1900, config->multicastPort() );
}

TEST(ParseOptionsTest, TestMergedOptions) {

    const char * options[6];
    options[0] = "--http-ip";
    options[1] = "127.0.0.1";
    options[2] = "--config-file";
    options[3] = "/foo/bar.xml";
    options[4] = "--local-address";
    options[5] = "127.0.0.1";

    squawk_config_ptr config = Config::instance( true );

    ASSERT_TRUE(config->parse(6, options));
    config->load(std::string( CMAKE_SOURCE_DIR ) + "config/squawk.xml");
    ASSERT_TRUE(config->validate());

    EXPECT_EQ(std::string("127.0.0.1"), config->httpAddress());

    EXPECT_EQ(std::string("/home/Pictures"), config->mediaDirectories().front() );
    EXPECT_EQ(8080, config->httpPort());
    EXPECT_EQ(std::string("/usr/local/var/lib/squawk/www"), config->docRoot() );
    EXPECT_EQ(std::string("/var/cache/squawk/squawk.db"), config->databaseFile() );
    EXPECT_EQ(std::string("/var/cache/squawk/"), config->tmpDirectory() );
    EXPECT_EQ(std::string("127.0.0.1"), config->localListenAddress() );
    EXPECT_EQ(std::string("239.255.255.250"), config->multicastAddress() );
    EXPECT_EQ(1900, config->multicastPort() );
}

TEST(ParseOptionsTest, TestWriteConfigfile) {
    const char * options[16];
    options[0] = "--media-directory";
    options[1] = "/foo/bar";
    options[2] = "--media-directory";
    options[3] = "/foo/bar2";
    options[4] = "--http-docroot";
    options[5] = "/foo/bar/docroot";
    options[6] = "--database-file";
    options[7] = "/foo/bar.db";
    options[8] = "--tmp-directory";
    options[9] = "/tmp/foo/bar";
    options[10] = "--config-file";
    options[11] = "/foo/bar.xml";
    options[12] = "--http-ip";
    options[13] = "127.0.0.1";
    options[14] = "--local-address";
    options[15] = "127.0.0.1";

    squawk_config_ptr config = Config::instance( true );

    ASSERT_TRUE(config->parse(16, options));
    EXPECT_EQ(std::string("/foo/bar.xml"), config->configFile() );
    ASSERT_FALSE(config->rescan);

    config->save("/tmp/squawk.xml");

    std::string xml =
        "<?xml version=\"1.0\"?>\n" \
        "<squawk>\n" \
        "  <config-file>/foo/bar.xml</config-file>\n" \
        "  <database-file>/foo/bar.db</database-file>\n" \
        "  <http-docroot>/foo/bar/docroot</http-docroot>\n" \
        "  <http-ip>127.0.0.1</http-ip>\n" \
        "  <local-address>127.0.0.1</local-address>\n" \
        "  <media-directories>\n    <media-directory>/foo/bar</media-directory>\n    <media-directory>/foo/bar2</media-directory>\n  </media-directories>\n" \
        "  <tmp-directory>/tmp/foo/bar</tmp-directory>\n" \
        "</squawk>\n";

    std::ifstream t( "/tmp/squawk.xml" );
    std::stringstream buffer;
    buffer << t.rdbuf();

    ASSERT_EQ(xml, buffer.str());
}
}//namepsace squawk
