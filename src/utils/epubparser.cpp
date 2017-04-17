#include "epubparser.h"

#include <array>
#include <cstring>
#include <iostream>
#include <sstream>

#include "re2/re2.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <archive.h>
#include <archive_entry.h>

#include "textutils.h"

namespace epub {

std::array < std::string, 6 > EpubParser::isbn_regex_list = std::array< std::string, 6 > ( {
    "<dc:identifier.*opf:scheme=\".*ISBN.*\".*>(.*)</dc:identifier>",
    "<dc:identifier.*opf:scheme=\".*URI.*\".*>(.*)</dc:identifier>",
    "<dc:identifier.*id=\".*PackageID.*\".*>(.*)</dc:identifier>",
    "urn:isbn:([\\d]{10,13})",
//TODO
    "([\\d]+[- ·]?[\\d]*[- ·]{1}[\\d]*[- ·]{1}[\\d]*[- ·]{1}[\\dX]*)",
    "([\\d]{10,13})"
});

//const std::string EpubParser::isbn_identifier_regex = "<dc:identifier.*opf:scheme=\".*ISBN.*\".*>(.*)</dc:identifier>";
//const std::string EpubParser::isbn_identifier_uri_regex = "<dc:identifier.*opf:scheme=\".*URI.*\".*>(.*)</dc:identifier>";
//const std::string EpubParser::isbn_identifier_package_regex = "<dc:identifier.*id=\".*PackageID.*\".*>(.*)</dc:identifier>";
//const std::string EpubParser::isbn_urn_regex = "urn:isbn:([\\d]{10,13})";
//const std::string EpubParser::isbn_source_regex = "<dc:source id=\"src-id\">(.*)</dc:source>";
//const std::string EpubParser::isbn_regex = "([\\d]+[- ·]?[\\d]*[- ·]{1}[\\d]*[- ·]{1}[\\d]*[- ·]{1}[\\dX]*)";
//const std::string EpubParser::isbn_number_only_regex = "([\\d]{10,13})";

std::string EpubParser::get_isbn( const std::string& filename ) {

    std::cout << "parse ebook " << filename << std::endl;

    std::string _isbn;

    struct archive *a;
    struct archive_entry *entry;
    int r;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    r = archive_read_open_filename(a, filename.c_str(), 10240);
    if (r != ARCHIVE_OK)
        exit(1);
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char* _entry_name = archive_entry_pathname(entry);
        std::cout << "'" << _entry_name << "'" << std::endl;
        if( strcmp ( ".html", _entry_name + strlen( _entry_name ) - 5 ) == 0 ||
            strcmp ( ".xhtml", _entry_name + strlen( _entry_name ) - 6 ) == 0 ) {
            std::stringstream _ss;
            std::array< char, 1024 > _buff;
            ssize_t size = 0;
            for (;;) {
                size = archive_read_data( a, _buff.data(), 1024 );
                if (size < 0) {
                    /* TODO ERROR */
                }
                if (size == 0)
                    break;
                _ss << std::string( _buff.data(), size );
            }

            std::string _page = _ss.str();
            boost::to_upper( _page );
            _page = squawk::utils::TextUtils::strip_html ( _page );
            std::cout << _page << std::endl; //TODO
            std::string _isbn = squawk::utils::TextUtils::find_isbn( _page );

            if( !_isbn.empty() ) {
                std::cout << "Epub found ISBN \'" << _isbn << "\'" << std::endl;
                return _isbn; }

        } else if( strcmp ( ".opf", _entry_name + strlen( _entry_name ) - 4 ) == 0 ) {
            std::stringstream _ss;
            std::array< char, 1024 > _buff;
            ssize_t size = 0;
            for (;;) {
                size = archive_read_data( a, _buff.data(), 1024 );
                if (size < 0) {
                    /* TODO ERROR */
                }
                if (size == 0)
                    break;
                std::cout << std::string( _buff.data(), size );
                _ss << std::string( _buff.data(), size );
            }

            std::cout << _ss.str() << std::endl;
            //SEARCH ISBN
//TODO            RE2::PartialMatch ( _ss.str(), EpubParser::isbn_identifier_regex, &_isbn );
//TODO            if( _isbn.empty() )
//TODO                RE2::PartialMatch ( _ss.str(), EpubParser::isbn_identifier_uri_regex, &_isbn);
//TODO            if( _isbn.empty() )
//TODO                RE2::PartialMatch ( _ss.str(), EpubParser::isbn_identifier_package_regex, &_isbn);
//TODO            if( _isbn.empty() )
//TODO                RE2::PartialMatch ( _ss.str(), EpubParser::isbn_urn_regex, &_isbn);
//TODO            if( _isbn.empty() )
//TODO                RE2::PartialMatch ( _ss.str(), EpubParser::isbn_source_regex, &_isbn);

        } else {
            archive_read_data_skip(a);
        }
    }
    r = archive_read_free(a);
    return ( !_isbn.empty() ? squawk::utils::TextUtils::clean_isbn ( _isbn ) : "" ); //TODO
}

//TODO
//const std::string EpubParser::strip_html( const std::string& body ) {
//    std::string _result = body;
//    RE2::GlobalReplace ( &_result, "<[^>]*>", " " );
//    return _result;
//}

//TODO
//const std::string EpubParser::clean_isbn( const std::string& isbn ) {
//    std::string _isbn = isbn;
//    if( _isbn.find( "ISBN" ) != std::string::npos ) {
//        return _isbn.substr( isbn.find( "ISBN" ) + 4 );
//    } else return _isbn;
//}
}//namespace epub
