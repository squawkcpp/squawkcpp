#ifndef TEXTUTILS_H
#define TEXTUTILS_H

#include <string>

namespace squawk {
namespace utils {

class TextUtils {
public:

    /**
     * @brief clean isbn. remove special characters.
     * @param isbn the input isbn
     * @return clean isbn
     */
    static std::string clean_isbn( std::string isbn );
    /**
     * @brief strip html tags from string.
     * @param body
     * @return
     */
    static const std::string strip_html( const std::string& body );
    /**
     * @brief find and return isbn string
     * @param body the body to search in.
     * @return  the isbn
     */
    static const std::string find_isbn ( const std::string& body );

    //TODO
    static const std::string remove_special_characters( const std::string& body );

private:
    TextUtils() {}
    ~TextUtils() {}
};
}//namespace squawk
}//namespace utils
#endif // TEXTUTILS_H
