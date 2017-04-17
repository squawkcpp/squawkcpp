#ifndef EPUBPARSER_H
#define EPUBPARSER_H

#include <array>
#include <string>

namespace epub {

class EpubParser {
public:
    EpubParser() {}
    static std::string get_isbn( const std::string& filename );

    static std::array < std::string, 6 > isbn_regex_list;
};
}//namespace epub
#endif // EPUBPARSER_H
