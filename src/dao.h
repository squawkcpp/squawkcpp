#ifndef DAO_H
#define DAO_H

#include "easylogging++.h"
#include "fmt/format.h"

#include "database.h"
#include "upnp/upnp.h"

namespace dao {

template< int val, int end >
struct query_iterator_t {
    template< typename E, typename S >
    static void run(E e, S& s) {
        s << std::get< val >( e );
        query_iterator_t<static_cast<int>(val + 1), end>::run( e, s );
    }
};

template<int end>
struct query_iterator_t< end, end > {
    template< typename E, typename S >
    static void run(E,S&) {}
};

template< typename T, typename Connection >
/**
 * @brief count the children.
 * @param connection
 * @param id of the parent
 * @return number of children
 */
static int count ( Connection connection, size_t id ) {
    int count_ = 0;
     if ( ! db::exec ( connection, "select count(*) from tbl_cds_object o JOIN tbl_cds_mapping as m where m.parent=? and o.cls=? and m.child = o.ROWID",
                      [&count_](auto stmt){ count_ = db::get< int >( stmt, 0 ); }, id, T::type ) ) {
        LOG ( ERROR ) << "Database Error: " << db::errmsg( connection );
    }
    return count_;
}

template< typename Connection >
/**
 * @brief count the artists children.
 * @param connection
 * @param id of the parent
 * @return number of children
 */
static int count_artist_childs ( Connection connection, size_t id ) {
    int count_ = 0;
    if( ! db::exec ( connection, "select count(*) from tbl_cds_object o JOIN tbl_cds_mapping as m where m.child=? and o.cls=? and  m.parent = o.ROWID",
                [&count_](auto stmt){ count_ = db::get< int >( stmt, 0 ); }, id, upnp::didl::DidlContainerAlbum::type ) ) {
        LOG ( ERROR ) << "Database Error: " << db::errmsg( connection );
    }
    return count_;
}

template< typename Connection >
/**
 * @brief count nodes by type.
 * @param connection
 * @param type
 * @return number of elements
 */
static int count ( Connection connection, upnp::didl::DIDL_CLASS type ) {
    int count_ = 0;

    if( ! db::exec ( connection, "select count(*) from tbl_cds_object where cls=?",
                [&count_](auto stmt){ count_ = db::get< int >( stmt, 0 ); }, static_cast< int >( type ) ) ) {
        LOG ( ERROR ) << "Database Error: " << db::errmsg( connection );
    }
    return count_;
}
template< typename Connection >
static int touch ( Connection connection, const std::string & path, const unsigned long mtime ) {
    unsigned long time_ = std::time ( 0 );
    if( ! db::exec( connection, "update tbl_cds_object SET timestamp=? where path=? and mtime=?", time_, path, mtime ) ) {
        LOG ( ERROR ) << "Database Error: " << db::errmsg( connection );
    }
    return ( db::changes( connection ) );
}
template< typename T, typename Connection, typename Fn >
static void item ( Connection connection, Fn f, int id ) {
    if( ! db::exec ( connection, fmt::format( "select {0} from tbl_cds_object where ROWID=?", upnp::didl::str< T >() ), f, id ) ) {
        LOG ( ERROR ) << "Database Error: " << db::errmsg( connection );
    }
}
template< typename T, typename Connection, typename Fn >
static void child ( Connection connection, Fn f, int id ) {
    if( ! db::exec ( connection,
        fmt::format( "select {0} from tbl_cds_object as o JOIN tbl_cds_mapping as m " \
                     "where m.parent = ? and o.cls = ? and o.ROWID=m.child " \
                     "order by track asc", upnp::didl::str< T >( ",", "o." ) ), f, id, T::type ) ) {
        LOG ( ERROR ) << "Database Error: " << db::errmsg( connection );
    }
}
template< typename T, typename Connection, typename Fn >
static void child ( Connection connection, std::string filter, Fn f, int id ) {
    std::string _sql_filter = "";
    if( filter.size() > 0 ) {
        _sql_filter = fmt::format ( "AND {0} ", filter );
    }
    if( ! db::exec ( connection,
        fmt::format( "select {0} from tbl_cds_object as o JOIN tbl_cds_mapping as m " \
                     "where m.parent = ? and o.cls = ? and o.ROWID=m.child {1}" \
                     "order by track asc", upnp::didl::str< T >( ",", "o." ), _sql_filter ), f, id, T::type ) ) {
        LOG ( ERROR ) << "Database Error: " << db::errmsg( connection );
    }
}

template< typename Connection, typename Fn >
static void child_artist ( Connection connection, std::string filter, Fn f, int id ) {
    std::string _sql_filter = "";
    if( filter.size() > 0 ) {
        _sql_filter = fmt::format ( "AND {0} ", filter );
    }
    if( ! db::exec ( connection,
        fmt::format( "select {0} from tbl_cds_object as o JOIN tbl_cds_mapping as m " \
                     "where m.child=? and o.cls=? and  m.parent = o.ROWID {1} " \
                     "order by title asc", upnp::didl::str< upnp::didl::DidlContainerAlbum >( ",", "o." ), _sql_filter ), f, id, upnp::didl::DidlContainerAlbum::type ) ) {
        LOG ( ERROR ) << "Database Error: " << db::errmsg( connection );
    }
}

template< typename T >
struct query {
    template< typename Connection, typename Tsort, typename Fn, typename... Args >
    /**
     * @brief query elements by class type
     * @param connection the database connection
     * @param cls the class filter.
     * @param sort sort results
     * @param ascending asc or descending
     * @param fn the callback function
     * @param args the arguments (start and count)
     */
    static void run( Connection connection, std::vector< upnp::didl::DIDL_CLASS > cls, const Tsort& sort, bool asc, Fn fn, Args... args ) {

        std::stringstream _ss;
        _ss << "select ";
        bool _first_column = true;
        upnp::didl::iterator_t< 0, static_cast< int >( T::end ) >::run( [&_first_column, &_ss] ( const int position ) {
            if( _first_column ) _first_column = false; else _ss << ",";
            _ss << T::attribute( position );
        });
        _ss << " from tbl_cds_object where cls in(";
        _first_column = true;
        for( size_t i = 0; i<cls.size(); ++i /*auto c : cls*/ ) {
            if( _first_column ) _first_column = false; else _ss << ",";
            _ss << "?";
        }
        _ss << ") order by ";
        //query_iterator_t< 0, sizeof...(Qs) >::run( queries, _ss );
        _ss << " " << T::attribute( sort ) << " " << (asc?"asc":"desc") << " limit ?, ?";
//        LOG ( TRACE ) << "QUERY: " << _ss.str(); //TODO remove
        if( ! db::exec( connection, _ss.str(), fn, cls, args... ) ) {
            LOG ( ERROR ) << "Database Error: " << db::errmsg( connection );
        }
    }
};
template< typename T >
struct store {
    template< typename Connection, typename... Args >
    static size_t run( Connection connection, const std::string& filename, Args... args ) {
        size_t id_ = 0U;

        db::exec( connection, "select ROWID from tbl_cds_object where path = ?",
                              [&id_] (auto statement) { id_ = db::get< int >( statement, 0 ); }, filename );
        if ( id_ > 0 ) {

            //update
            //TODO cleanup
            std::stringstream _ss;
            _ss << "update tbl_cds_object SET ";
            bool _first_column = true;
            upnp::didl::iterator_t< 0, static_cast< int >( T::end ) >::run( [&_first_column, &_ss] ( const int position ) {
                if( position != T::id ) {
                    if( _first_column ) _first_column = false; else _ss << ",";
                    _ss << T::attribute( position ) << "=?";
                }
            });
            _ss << " where ROWID = ?";
//            LOG ( TRACE ) << "UPDATE: " << _ss.str();
            db::exec( connection, _ss.str(), args..., id_ );

        } else {
            //insert //TODO cleanup
            std::stringstream _ss;
            _ss << "insert into tbl_cds_object( ";
            bool _first_column = true;
            upnp::didl::iterator_t< 0, static_cast< int >( T::end ) >::run( [&_first_column, &_ss] ( const int position ) {
                if( position != T::id ) {
                    if( _first_column ) _first_column = false; else _ss << ",";
                    _ss << T::attribute( position );
                }
            });
            _ss << ") values( ";
            _first_column = true;
            upnp::didl::iterator_t< 0, static_cast< int >( T::end ) >::run( [&_first_column, &_ss] ( const int position ) {
                if( position != T::id ) {
                    if( _first_column ) _first_column = false; else _ss << ",";
                    _ss << "?";
                }
            });
            _ss << ")";
//            LOG ( TRACE ) << "INSERT: " << _ss.str();

            db::exec( connection, _ss.str(), args... );
            id_ = db::last_insert_rowid ( connection );
        }
        return id_;
    }
};
template< typename Connection >
static void mapping ( Connection connection, const int & parent, const int& child ) {
    if( ! db::exec( connection, "select count(*) from tbl_cds_mapping where parent=? and child=?",
              [&connection,&parent,&child] (auto stmt) {
                const int mapping_count_ = db::get< int >( stmt, 0 );
                if( mapping_count_ == 0 ) {
                    db::exec( connection, "insert into tbl_cds_mapping( parent, child ) VALUES( ?, ? )", parent, child );
                }
    }, parent, child ) ) {
        LOG ( ERROR ) << "Database Error: " << db::errmsg( connection );
    }
}

template< typename Connection >
static size_t parent ( Connection connection, const int& child ) {
    size_t _parent_id = 0;
    if( ! db::exec( connection, "select m.parent from tbl_cds_mapping as m where m.child=?",
              [&_parent_id] (auto stmt) {
                    _parent_id = db::get< size_t >( stmt, 0 );
    }, child ) ) {
        LOG ( ERROR ) << "Database Error: " << db::errmsg( connection );
    }
    return _parent_id;
}
}//namespace dao
#endif // DAO_H
