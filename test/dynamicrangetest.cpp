/*
    <one line to give the library's name and an idea of what it does.>
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

#include <vector>

#include <gtest/gtest.h>

#include "../src/utils/dynamicrange.h"

namespace media {

TEST( DynamicRangeTest, dynamic_range_16 ) {
    std::string _filename = std::string(TESTFILES) + "BIS1536-001-flac_16.flac";

    DynamicRange range;
    range.calculate( _filename );

//    EXPECT_EQ( upnp::didl::DIDL_CLASS::objectContainerSeries, _cls2.at( 1 ) );

}
TEST( DynamicRangeTest, dynamic_range_24 ) {
    std::string _filename = std::string(TESTFILES) + "BIS1536-001-flac_24.flac";

    DynamicRange range;
    range.calculate( _filename );

//    EXPECT_EQ( upnp::didl::DIDL_CLASS::objectContainerSeries, _cls2.at( 1 ) );
}
}//namepsace media
