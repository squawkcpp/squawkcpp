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

#ifndef DATABASE_H
#define DATABASE_H

#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include <sqlite3.h>

/* --------------------------------------------------- */
/* ---     utilities                               --- */
/* --------------------------------------------------- */

namespace db {
namespace utils {
template <typename T> struct container_traits {
  static bool const value = false;
};
template <typename... Args>
struct container_traits<std::vector<Args...>> {
  static bool const value = true;
};

template<class F, class...Args>
struct is_callable {
    template<class U> static auto test(U* p) -> decltype((*p)(std::declval<Args>()...), void(), std::true_type());
    template<class U> static auto test(...) -> decltype(std::false_type());

    static constexpr bool value = decltype(test<F>(0))::value;
};
}}

enum db_errc {
OK          = SQLITE_OK         /* Successful result */,
ERROR       = SQLITE_ERROR      /* SQL error or missing database */,
INTERNAL    = SQLITE_INTERNAL   /* Internal logic error in SQLite */,
PERM        = SQLITE_PERM       /* Access permission denied */,
ABORT       = SQLITE_ABORT      /* Callback routine requested an abort */,
BUSY        = SQLITE_BUSY       /* The database file is locked */,
LOCKED      = SQLITE_LOCKED     /* A table in the database is locked */,
NOMEM       = SQLITE_NOMEM      /* A malloc() failed */,
READONLY    = SQLITE_READONLY   /* Attempt to write a readonly database */,
INTERRUPT   = SQLITE_INTERRUPT  /* Operation terminated by sqlite3_interrupt() */,
IOERR       = SQLITE_IOERR      /* Some kind of disk I/O error occurred */,
CORRUPT     = SQLITE_CORRUPT    /* The database disk image is malformed */,
NOTFOUND    = SQLITE_NOTFOUND   /* Unknown opcode in sqlite3_file_control() */,
FULL        = SQLITE_FULL       /* Insertion failed because database is full */,
CANTOPEN    = SQLITE_CANTOPEN   /* Unable to open the database file */,
PROTOCOL    = SQLITE_PROTOCOL   /* Database lock protocol error */,
EMPTY       = SQLITE_EMPTY      /* Database is empty */,
SCHEMA      = SQLITE_SCHEMA     /* The database schema changed */,
TOOBIG      = SQLITE_TOOBIG     /* String or BLOB exceeds size limit */,
CONSTRAINT  = SQLITE_CONSTRAINT /* Abort due to constraint violation */,
MISMATCH    = SQLITE_MISMATCH   /* Data type mismatch */,
MISUSE      = SQLITE_MISUSE     /* Library used incorrectly */,
NOLFS       = SQLITE_NOLFS      /* Uses OS features not supported on host */,
AUTH        = SQLITE_AUTH       /* Authorization denied */,
FORMAT      = SQLITE_FORMAT     /* Auxiliary database format error */,
RANGE       = SQLITE_RANGE      /* 2nd parameter to sqlite3_bind out of range */,
NOTADB      = SQLITE_NOTADB     /* File opened that is not a database file */,
NOTICE      = SQLITE_NOTICE     /* Notifications from sqlite3_log() */,
WARNING     = SQLITE_WARNING    /* Warnings from sqlite3_log() */,
ROW         = SQLITE_ROW        /* sqlite3_step() has another row ready */,
DONE        = SQLITE_DONE       /* sqlite3_step() has finished executing */,
UNKNOWN
};

namespace std {
template<> struct is_error_condition_enum<db_errc> : public true_type {};
}

class db_category_t : public std::error_category {
public:
  virtual const char* name() const noexcept { return "db"; }
  virtual std::error_condition default_error_condition (int ev) const noexcept {
    if ( ( ev>=SQLITE_OK ) && ( ev<= SQLITE_DONE)  )
    { return std::error_condition( static_cast< db_errc >( ev ) ); }
    else return std::error_condition( db_errc::UNKNOWN );
  }
  virtual bool equivalent ( const std::error_code& code, int condition ) const noexcept {
    return *this==code.category() &&
           static_cast< int >( default_error_condition( code.value()).value() ) == condition;
  }
  virtual std::string message(int ev) const {
    switch ( ev ) {
    case SQLITE_OK:        return "Successful result";
    case SQLITE_ERROR:     return "SQL error or missing database";
    case SQLITE_INTERNAL:  return "Internal logic error in SQLite";
    case SQLITE_PERM:      return "Access permission denied";
    case SQLITE_ABORT:     return "Callback routine requested an abort";
    case SQLITE_BUSY:      return "The database file is locked";
    case SQLITE_LOCKED:    return "A table in the database is locked";
    case SQLITE_NOMEM:     return "A malloc() failed";
    case SQLITE_READONLY:  return "Attempt to write a readonly database";
    case SQLITE_INTERRUPT: return "Operation terminated by sqlite3_interrupt()";
    case SQLITE_IOERR:     return "Some kind of disk I/O error occurred";
    case SQLITE_CORRUPT:   return "The database disk image is malformed";
    case SQLITE_NOTFOUND:  return "Unknown opcode in sqlite3_file_control()";
    case SQLITE_FULL:      return "Insertion failed because database is full";
    case SQLITE_CANTOPEN:  return "Unable to open the database file";
    case SQLITE_PROTOCOL:  return "Database lock protocol error";
    case SQLITE_EMPTY:     return "Database is empty";
    case SQLITE_SCHEMA:    return "The database schema changed";
    case SQLITE_TOOBIG:    return "String or BLOB exceeds size limit";
    case SQLITE_CONSTRAINT:return "Abort due to constraint violation";
    case SQLITE_MISMATCH:  return "Data type mismatch";
    case SQLITE_MISUSE:    return "Library used incorrectly";
    case SQLITE_NOLFS:     return "Uses OS features not supported on host";
    case SQLITE_AUTH:      return "Authorization denied";
    case SQLITE_FORMAT:    return "Auxiliary database format error";
    case SQLITE_RANGE:     return "2nd parameter to sqlite3_bind out of range";
    case SQLITE_NOTADB:    return "File opened that is not a database file";
    case SQLITE_NOTICE:    return "Notifications from sqlite3_log()";
    case SQLITE_WARNING:   return "Warnings from sqlite3_log()";
    case SQLITE_ROW:       return "sqlite3_step() has another row ready";
    case SQLITE_DONE:      return "sqlite3_step() has finished executing";
    default: return "Unknown error";
    }
  }
} static db_category;

// make_error_code overload to generate custom conditions:
static std::error_condition make_error_condition (db_errc e) {
    return std::error_condition(static_cast<int>(e), db_category);
}

namespace db {

/* --------------------------------------------------- */
/* ---     type and class definitions              --- */
/* --------------------------------------------------- */

typedef std::shared_ptr< sqlite3 > sqlite3_ptr;
typedef std::shared_ptr< sqlite3_stmt > sqlite3_stmt_ptr;

/**
 * @brief The DatabaseException class
 */
class DbException : public std::exception {
public:
    explicit DbException ( int _code ) throw() : _code ( _code ) {}
    explicit DbException ( int _code, std::string _what ) throw() : _code ( _code ), _what ( _what ) {}
    virtual ~DbException() throw() {}
    virtual const char* what() const throw() {
        return _what.c_str();
    }
    int code() throw() {
        return _code;
    }
private:
    int _code;
    std::string _what;
};

/* --------------------------------------------------- */
/* ---     make database objects                   --- */
/* --------------------------------------------------- */

template< typename DB >
static sqlite3_stmt_ptr make_statement ( DB db, const std::string& statement ) {
    sqlite3_stmt* sqlite3_statement;
    if ( int res = sqlite3_prepare ( db, statement.c_str(), -1, &sqlite3_statement, 0 ) != SQLITE_OK )
    { throw DbException ( res, sqlite3_errmsg ( db ) ); }

    sqlite3_stmt_ptr sqlite3_statement_ ( sqlite3_statement, [] ( sqlite3_stmt * sqlite3_statement ) {
        sqlite3_finalize ( sqlite3_statement );
    } );
    return sqlite3_statement_;
}

static sqlite3_ptr make_connection ( const std::string& path ) {
    sqlite3 * _sqlite3_db;

    if ( int res = sqlite3_open ( path.c_str(), &_sqlite3_db ) != SQLITE_OK )
    { throw DbException ( res, sqlite3_errmsg ( _sqlite3_db ) ); }

    sqlite3_ptr database_ptr_ = std::unique_ptr< sqlite3, std::function<void ( sqlite3* ) > > ( _sqlite3_db,
        [] ( auto * database ) {
            sqlite3_close ( database );
        }
    );
    return database_ptr_;
}

/* --------------------------------------------------- */
/* ---     bind values to Statement                --- */
/* --------------------------------------------------- */

template< typename T, typename Statement >
static typename std::enable_if<std::is_same<bool, T>::value, T>::type get( Statement statement, const int position ) {
    sqlite3_value* sql3_msgid = sqlite3_column_value ( statement.get(), position );
    return ( sqlite3_value_int ( sql3_msgid ) == 0 ? false : true );
}
template< typename T, typename Statement >
static typename std::enable_if<std::is_same<int, T>::value, T>::type get( Statement statement, const int position ) {
    sqlite3_value* sql3_msgid = sqlite3_column_value ( statement.get(), position );
    return sqlite3_value_int ( sql3_msgid );
}
template< typename T, typename Statement >
static typename std::enable_if<std::is_same<long, T>::value, T>::type get( Statement statement, const int position ) {
    sqlite3_value* sql3_msgid = sqlite3_column_value ( statement.get(), position );
    return sqlite3_value_int64 ( sql3_msgid );
}
template< typename T, typename Statement >
static typename std::enable_if<std::is_same<unsigned long, T>::value, T>::type get( Statement statement, const int position ) {
    sqlite3_value* sql3_msgid = sqlite3_column_value ( statement.get(), position );
    return static_cast< unsigned long >( sqlite3_value_int64( sql3_msgid ) );
}
template< typename T, typename Statement >
static typename std::enable_if<std::is_same<const char*, T>::value, T>::type get( Statement statement, const int position ) {
    sqlite3_value* sql3_msgid = sqlite3_column_value ( statement.get(), position );
    return reinterpret_cast< const char* >( sqlite3_value_text( sql3_msgid ) );
}
template< typename T, typename Statement >
static typename std::enable_if<std::is_enum<T>::value, T>::type get( Statement statement, const int position ) {
    sqlite3_value* sql3_msgid = sqlite3_column_value ( statement.get(), position );
    return static_cast< T >( sqlite3_value_int( sql3_msgid ) );
}

template<typename Statement >
static void bind( Statement statement, const int& position, const bool& t ) {
    if ( const int res_ = sqlite3_bind_int ( statement.get(), position, ( t ? 1 : 0 ) ) != SQLITE_OK )
    { throw DbException ( res_ ); }
}
template<typename Statement >
static void bind( Statement statement, const int& position, const int& t ) {
    if ( const int res_ = sqlite3_bind_int ( statement.get(), position, t ) != SQLITE_OK )
    { throw DbException ( res_ ); }
}
template<typename Statement >
static void bind( Statement statement, const int& position, const std::string& t ) {
    if ( const int res_ = sqlite3_bind_text ( statement.get(), position, t.c_str(), t.length(), SQLITE_TRANSIENT ) != SQLITE_OK )
    { throw DbException ( res_ ); }
}
template<typename Statement >
static void bind(Statement statement, const int& position, const char*& t ) {
    if ( const int res_ = sqlite3_bind_text ( statement.get(), position, t, std::strlen(t), SQLITE_TRANSIENT ) != SQLITE_OK )
    { throw DbException ( res_ ); }
}
template<typename Statement >
static void bind( Statement statement, const int& position, const long& t ) {
    if ( const int res = sqlite3_bind_int64 ( statement.get(), position, t ) != SQLITE_OK )
    { throw DbException ( res ); }
}
template<typename Statement >
static void bind( Statement statement, int& position, const unsigned long& t ) {
    if ( const int res = sqlite3_bind_int64 ( statement.get(), position, t ) != SQLITE_OK )
    { throw DbException ( res ); }
}
template<typename Statement, typename T >
static typename std::enable_if<std::is_enum<T>::value>::type bind( Statement statement, const int& position, const T& t ) {
    if ( const int res = sqlite3_bind_int ( statement.get(), position, static_cast< int >( t ) ) != SQLITE_OK )
    { throw DbException ( res ); }
}

/* --------------------------------------------------- */
/* ---     column bind iterator                    --- */
/* --------------------------------------------------- */

template< typename Statement, typename T >
static typename std::enable_if<not ::db::utils::container_traits<T>::value, void >::type bind_iterator( Statement statement, int& position, T& t ) {
    db::bind( statement, ++position, t);
}
template< typename Statement, typename T >
static typename std::enable_if<std::is_same<T, std::vector< typename T::value_type, typename T::allocator_type > >::value >::type bind_iterator( Statement statement, int& position, T& t ) {
    for( auto i : t ) {
        db::bind( statement, ++position, i );
    }
}

template< int val, int end, typename... Args >
struct make_bind_iterator {
    template< typename DB >
    static void run(DB db, int& position, Args... args ) {
        std::tuple< Args... > args_( args... );
        bind_iterator( db, position, std::get< val >( args_ ) );
        make_bind_iterator<static_cast<int>(val + 1), end, Args...>::run( db, position, args... );
    }
};
template< int end, typename... Args >
struct make_bind_iterator< end, end, Args... > {
    template< typename DB >
    static void run(DB /*db*/, int& /*position*/, Args... /*args*/ ) {}
};

/* --------------------------------------------------- */
/* ---     database utilities                      --- */
/* --------------------------------------------------- */

template< typename DB >
static void transaction_start ( DB db ) {
    sqlite3_stmt_ptr stmt_ = make_statement ( db.get(), "BEGIN;" );
    sqlite3_step ( stmt_.get() );
}
template< typename DB >
static void transaction_end ( DB db ) {
    sqlite3_stmt_ptr stmt_ = make_statement ( db.get(), "END;" );
    sqlite3_step ( stmt_.get() );
}
template< typename DB >
static unsigned long last_insert_rowid ( DB db ) {
    return sqlite3_last_insert_rowid ( db.get() );
}
template< typename DB >
static unsigned long changes ( DB db ) {
    return ( sqlite3_changes ( db.get() ) );
}
template< typename DB >
static std::string errmsg ( DB db ) {
    return sqlite3_errmsg ( db.get() );
}

/* --------------------------------------------------- */
/* ---     modify data                             --- */
/* --------------------------------------------------- */

template< typename DB >
static bool exec ( DB db, const std::string& query ) {
    sqlite3_stmt_ptr stmt_ = make_statement ( db.get(), query );
    while( int code_ = sqlite3_step ( stmt_.get() ) ) {
        if (  code_ == SQLITE_DONE) {
            return true;
        } else if( code_ != SQLITE_ROW ) {
            return false;
        }
    }
    return false;
}
template< typename DB, typename... Args >
static bool exec ( DB db, const std::string& query, Args... args ) {
    sqlite3_stmt_ptr stmt_ = make_statement ( db.get(), query );
    int position = 0;
    make_bind_iterator< 0, (sizeof...(Args)), Args... >::run( stmt_, position, args... );
    while( int code_ = sqlite3_step ( stmt_.get() ) ) {
        if (  code_ == SQLITE_DONE) {
            return true;
        } else if (  code_ != SQLITE_ROW ) {
             return false;
        }
    }
    return false;
}
template< typename DB, typename Fn >
static typename std::enable_if< db::utils::is_callable<Fn, sqlite3_stmt_ptr>::value, bool >::type exec ( DB db, const std::string& query, Fn f ) {
    sqlite3_stmt_ptr stmt_ = make_statement ( db.get(), query );
    while( int code_ = sqlite3_step ( stmt_.get() ) ) {
        if (  code_ == SQLITE_DONE) {
            return true;
        } else if (  code_ == SQLITE_ROW ) {
            f( stmt_ );
        } else {
            return false;
        }
    }
    return false;
}
template< typename DB, typename Fn, typename... Args >
static typename std::enable_if< db::utils::is_callable<Fn, sqlite3_stmt_ptr>::value, bool >::type exec ( DB db, const std::string& query, Fn f, Args... args ) {
    sqlite3_stmt_ptr stmt_ = make_statement ( db.get(), query );
    int position = 0;
    make_bind_iterator< 0, (sizeof...(Args)), Args... >::run( stmt_, position, args... );
    while( int code_ = sqlite3_step ( stmt_.get() ) ) {
        if (  code_ == SQLITE_DONE) {
            return true;
        } else if (  code_ == SQLITE_ROW ) {
            f( stmt_ );
        } else {
            return false;
        }
    }
    return false;
}
}//namespace db
#endif // DATABASE_H
