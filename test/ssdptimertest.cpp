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

#include <string>

#include "../src/ssdp/ssdp.h"
#include "../src/ssdp/ssdpserverimpl.h"

#include <gtest/gtest.h>

namespace ssdp {
TEST( SSDPTimerTest, ParseTimeTest ) {

    auto time = SSDPServerImpl::parse_keep_alive( "max-age=1800" );
    EXPECT_EQ( 1800, time );
}
TEST( SSDPTimerTest, ParseTimeSpacesTest ) {

    auto time = SSDPServerImpl::parse_keep_alive( "max-age = 1800" );
    EXPECT_EQ( 1800, time );
}
}//namespace ssdp
