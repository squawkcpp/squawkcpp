#ifndef AMAZONFACADE_H
#define AMAZONFACADE_H

#include <map>
#include <string>

#include "gtest/gtest_prod.h"
#include "bookresultitem.h"

namespace utils {

/**
 * @brief The AmazonFacade class
 */
class AmazonFacade {
public:
    AmazonFacade() { }

    /**
     * @brief bookByIsbn
     * @param access_key
     * @param key
     * @param isbn
     */
    static BookResultItem bookByIsbn ( const std::string & access_key, const std::string & key, const std::string & isbn );
    /**
     * @brief parse the response xml
     * @param response
     * @return the result
     */
    static BookResultItem parse ( const std::string & response );

private:
    FRIEND_TEST ( AmazonFacadeTest, TestCanonicalize );
    FRIEND_TEST ( AmazonFacadeTest, TestMac );
    FRIEND_TEST ( AmazonFacadeTest, TestParseResponse );

    static std::string data;
    static size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up);

    /**
     * Canonicalize the query string as required by Amazon.
     *
     * @param sortedParamMap    Parameter name-value pairs in lexicographical order.
     * @return                  Canonical form of query string.
     */
    static std::string canonicalize ( const std::map<std::string, std::string> & sortedParamMap );
    /**
     * @brief get_utc_time_string
     * @return
     */
    static std::string get_utc_time_string();
    /**
     * @brief generate_hmac256bit_hash
     * @param message
     * @param key_buf
     * @return
     */
    static std::string generate_hmac256bit_hash ( const char *message, const char *key_buf );
    /**
     * @brief base64
     * @param input
     * @param length
     * @return
     */
    static char * base64 ( const unsigned char *input, int length );
};
}//namespace amazon
#endif // AMAZONFACADE_H
