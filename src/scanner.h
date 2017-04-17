/*
    <squawk multimedia server>
    Copyright (C) 2017  <etienne knecht> <squawkcpp@gmail.com>

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

#ifndef SCANNER_H
#define SCANNER_H

#include <stdexcept>
#include <string>
#include <vector>

namespace squawk {
class Scanner {
public:
    Scanner();
    void parse ( const std::vector< std::string >& directories ) throw ( std::invalid_argument );
};
}//namespace squawk
#endif // SCANNER_H
