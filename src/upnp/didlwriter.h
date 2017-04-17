#ifndef DIDLWRITER_H
#define DIDLWRITER_H

#include <string>
#include <unordered_map>

#include "upnp.h"

#include "../database.h"
#include "../dao.h"
#include "../utils/xml.h"

namespace upnp {
/**
 * @brief Write the didl content as XML.
 */
class DidlWriter {
public:
    DidlWriter( commons::xml::XMLWriter * xml_writer, db::sqlite3_ptr db, const std::string& parent );
    ~DidlWriter() {}

    void write( db::sqlite3_stmt_ptr statement );
    void node( const std::unordered_map< std::string, didl::DIDL_CLASS >& root_nodes );

    int count()
    { return count_; }

    static const char* PREFIX_ALBUM_ID;
    static const char* PREFIX_ARTIST_ID;
    static const char* PREFIX_SERIE_ID;

private:
    commons::xml::XMLWriter * xml_writer_;
    db::sqlite3_ptr db_;
    const std::string& parent_;
    commons::xml::Node didl_element_;
    int count_ = 0;

    static std::string http_uri( const std::string & suffix = "" );
};
}//namespace upnp
#endif // DIDLWRITER_H
