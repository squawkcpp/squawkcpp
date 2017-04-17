#ifndef TMDB_H
#define TMDB_H

#include <string>
#include <vector>

namespace tmdb {

struct Series {
    std::string title, poster, backdrop, comment;
    size_t tmdb_id, first_air_date;
};

class Tmdb {
public:
    static std::string series ( const std::string& api_key, const std::string& name );
    static std::vector < Series > parse_series( const std::string& result );
    static void get( const std::string& uri, const std::string& path );

private:
    static std::string BACKDROP_PATH;
    static std::string FIRST_AIR_DATE;
    static std::string ID;
    static std::string NAME;
    static std::string OVERVIEW;
    static std::string POSTER_PATH;
};
}//namespace tmdb
#endif // TMDB_H
