//#ifndef LIBAV_H
//#define LIBAV_H

//#include <memory>
//#include <system_error>

//#include "mediainfo.h"

//namespace libav {

//class LibavWrapper;

//class LibAv {
//public:
//    LibAv();
//    ~LibAv();

//    /**
//    * @brief input from file by name.
//    * @param filename the filename
//    * @return error
//    */
//    std::error_code input ( const std::string& filename );
//    std::error_code input ( std::istream* is );
//    /**
//     * @brief output to file by name
//     * @param filename the filename
//     * @return error
//     */
//    std::error_code output ( const std::string& filename );
//    std::error_code output ( MediaInfo& media_info );

//    std::error_code transcode ( int stream_number, MediaInfo& media_info );

//    /** @brief number of streams. */
//    unsigned short nb_streams ();

//    /** get the media information */
//    MediaInfo media_info ( const int index );

//    /* io_stream methods */
//    std::streamsize readsome ( unsigned char* s, std::streamsize n );

//    std::string& get_output ();
//    Metadata& metadata();

//private:
//    std::unique_ptr< LibavWrapper > av_;
//};
//}//namespace libav
//#endif // LIBAV_H
