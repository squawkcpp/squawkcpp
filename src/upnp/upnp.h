#ifndef UPNP_H
#define UPNP_H

#include <chrono>
#include <string>

#include "../ssdp/ssdp.h"

#include "format.h"
#include "codec.h"

#include "../utils/xml.h"
#include "http/httpclient.h"

#include "easylogging++.h"

namespace upnp {
namespace soap {
/** @brief XML SOAP Namespace */
const static std::string XML_NS_SOAP = "http://schemas.xmlsoap.org/soap/envelope/";
/** @brief XML SOAP Encoding Namespace */
const static std::string XML_NS_SOAPENC = "http://schemas.xmlsoap.org/soap/encoding/";
/** @brief XML Schema Namespace */
const static std::string XML_NS_SCHEMA = "http://www.w3.org/2001/XMLSchema";
/** @brief XML Schema Instance Namespace */
const static std::string XML_NS_SCHEMA_INSTANCE = "http://www.w3.org/2001/XMLSchema-instance";
/** @brief XML Schema result node */
const static std::string XML_RESULT = "Result";
/** @brief XML Schema result node */
const static std::string XML_RESULT_BODY = "Body";
/** @brief XML Schema type */
const static std::string XML_TYPE = "type";
/** @brief XML Schema type string */
const static std::string XML_TYPE_STRING = "xsd:string";
/** @brief XML Schema type int */
const static std::string XML_TYPE_INT = "xsd:int";

inline commons::xml::Node make_soap_envelope ( commons::xml::XMLWriter* xml_writer ) {
    commons::xml::Node envelope_node = xml_writer->element ( "Envelope" );
    xml_writer->ns ( envelope_node, XML_NS_SOAP, "s", true );
    xml_writer->ns ( envelope_node, XML_NS_SOAPENC, "soapenc", false );
    xml_writer->ns ( envelope_node, XML_NS_SCHEMA, "xsd", false );
    xml_writer->ns ( envelope_node, XML_NS_SCHEMA_INSTANCE, "xsi", false );
    xml_writer->attribute ( envelope_node, XML_NS_SOAP, "encodingStyle", XML_NS_SOAPENC );
    return envelope_node;
}
}//namespace soap
}//namespace upnp

namespace upnp {
namespace didl {
/** @brief DIDL root node name */
const static std::string DIDL_ROOT_NODE = "DIDL-Lite";
/** @brief DIDL container name */
const static std::string DIDL_ELEMENT_CONTAINER = "container";
/** @brief DIDL element class */
const static std::string DIDL_ELEMENT_CLASS = "class";
/** @brief DIDL TITLE */
const static std::string DIDL_ELEMENT_TITLE = "title";
/** @brief DIDL id attribute name */
const static std::string DIDL_ATTRIBUTE_ID = "id";
/** @brief DIDL parent id attribute name */
const static std::string DIDL_ATTRIBUTE_PARENT_ID = "parentID";
/** @brief DIDL restricted attribute name */
const static std::string DIDL_ATTRIBUTE_RESTRICTED = "restricted";
/** @brief DIDL child count attribute name */
const static std::string DIDL_ATTRIBUTE_CHILD_COUNT = "childCount";
/** @brief DIDL album art uri */
const static std::string DIDL_ALBUM_ART_URI = "albumArtURI";

enum DIDL_CLASS {
    object = 0,
    objectContainer = 1,
    objectContainerAlbumMusicAlbum = 2,
    objectContainerPersonMusicArtist = 3,
    objectItem = 4,
    objectItemAudioItemMusicTrack = 5,
    objectItemImageItemPhoto = 6,
    objectItemVideoItemMovie = 7,
    objectItemEBook = 8,
    objectContainerAlbumPhotoAlbum = 9,
    objectAlbumArt = 10,
    objectPersonPublisher = 11,
    objectPersonAuthor = 12,
    objectContainerSeries = 13,
    objectContainerStorageFolder = 14,
    objectItemVideoItemEpisode = 15
};

const std::array<std::string, 16> DIDL_CLASS_NAMES {{
    "object",       "object.container", "object.container.album.musicAlbum",    "artist",  "object.item",
    "object.item.audioItem.musicTrack", "object.item.photo", "object.item.movie", "object.item.ebook",
    "object.container.album.photoAlbum", "artwork", "publisher", "author", "object.container.series",
    "object.container.storageFolder", "object.item.episode"
}};

/* \brief Get the class name as string */
inline std::string className ( DIDL_CLASS cls ) {
    return DIDL_CLASS_NAMES.at( cls );
}

#define DIDL_OBJECT_ATTRIBUTES cls=0, id=1, parent_id=2, title=3, path=4, mime_type=5, mtime=6, object_update_id=7, profile=8
#define DIDL_ARTIST_ATTRIBUTES DIDL_OBJECT_ATTRIBUTES, end=9
#define DIDL_PHOTO_ATTRIBUTES DIDL_OBJECT_ATTRIBUTES, size=9, width=10, height=11, color_depth=12, import=13, end=14
#define DIDL_ALBUM_ATTRIBUTES DIDL_OBJECT_ATTRIBUTES, rating=9, year=10, playback_count=11, genre=12, import=13
#define DIDL_SERIES_ATTRIBUTES DIDL_OBJECT_ATTRIBUTES, rating=9, year=10, playback_count=11, comment=12, tmdb_id=13, import=14
#define DIDL_TRACK_ATTRIBUTES DIDL_ALBUM_ATTRIBUTES, size=14, track=15, disc=16, bitrate=17, bits_per_sample=18, \
                                nr_audio_channels=19, duration=20, sample_frequency=21, comment=22, album=23, end=24

#define DIDL_OBJECT_ATTRIBUTE_NAMES "cls", "ROWID", "parent_id", "title", "path", "mime_type", "mtime", "object_update_id", "profile"
#define DIDL_ARTIST_ATTRIBUTE_NAMES DIDL_OBJECT_ATTRIBUTE_NAMES
#define DIDL_PHOTO_ATTRIBUTE_NAMES DIDL_OBJECT_ATTRIBUTE_NAMES, "size", "width", "height", "color_depth", "import"
#define DIDL_ALBUM_ATTRIBUTE_NAMES DIDL_OBJECT_ATTRIBUTE_NAMES, "rating", "year", "playback_count", "genre", "import"
#define DIDL_SERIES_ATTRIBUTE_NAMES DIDL_OBJECT_ATTRIBUTE_NAMES, "rating", "year", "playback_count", "comment", "tmdb_id", "import"
#define DIDL_TRACK_ATTRIBUTE_NAMES DIDL_ALBUM_ATTRIBUTE_NAMES, \
    "size", "track", "disc", "bitrate", "bits_per_sample", \
    "nr_audio_channels", "duration", "sample_frequency", "comment", "album"

struct DidlObject {
    static const DIDL_CLASS type = DIDL_CLASS::object;
    enum attributes { DIDL_OBJECT_ATTRIBUTES, end=9 };
    static inline std::string attribute( const int& index ) {
        static auto attributes_list = std::vector< std::string >( { DIDL_OBJECT_ATTRIBUTE_NAMES } );
        return attributes_list.at( static_cast< size_t >( index ) );
    }
};
struct DidlContainerAlbum {
    static const DIDL_CLASS type = DIDL_CLASS::objectContainerAlbumMusicAlbum;
    enum attributes { DIDL_ALBUM_ATTRIBUTES, end=14 };
    static inline std::string attribute( const int& index ) {
        static auto attributes_list = std::vector< std::string >( { DIDL_ALBUM_ATTRIBUTE_NAMES } );
        return attributes_list.at( static_cast< size_t >( index ) );
    }
};
struct DidlContainerSeries {
    static const DIDL_CLASS type = DIDL_CLASS::objectContainerSeries;
    enum attributes { DIDL_SERIES_ATTRIBUTES, end=15 };
    static inline std::string attribute( const int& index ) {
        static auto attributes_list = std::vector< std::string >( { DIDL_SERIES_ATTRIBUTE_NAMES } );
        return attributes_list.at( static_cast< size_t >( index ) );
    }
};
struct DidlContainerArtist{
    static const DIDL_CLASS type = DIDL_CLASS::objectContainerPersonMusicArtist;
    enum attributes { DIDL_ARTIST_ATTRIBUTES };
    static inline std::string attribute( const int& index ) {
        static auto attributes_list = std::vector< std::string >( { DIDL_ARTIST_ATTRIBUTE_NAMES } );
        return attributes_list.at( static_cast< size_t >( index ) );
    }
};
struct DidlPersonPublisher {
    static const DIDL_CLASS type = DIDL_CLASS::objectPersonPublisher;
    enum attributes { DIDL_ARTIST_ATTRIBUTES };
    static inline std::string attribute( const int& index ) {
        static auto attributes_list = std::vector< std::string >( { DIDL_ARTIST_ATTRIBUTE_NAMES } );
        return attributes_list.at( static_cast< size_t >( index ) );
    }
};
struct DidlPersonAuthor {
    static const DIDL_CLASS type = DIDL_CLASS::objectPersonAuthor;
    enum attributes { DIDL_ARTIST_ATTRIBUTES };
    static inline std::string attribute( const int& index ) {
        static auto attributes_list = std::vector< std::string >( { DIDL_ARTIST_ATTRIBUTE_NAMES } );
        return attributes_list.at( static_cast< size_t >( index ) );
    }
};
struct DidlMusicTrack {
    static const DIDL_CLASS type = DIDL_CLASS::objectItemAudioItemMusicTrack;
    enum attributes { DIDL_TRACK_ATTRIBUTES };
    static inline std::string attribute( const int& index ) {
        static auto attributes_list = std::vector< std::string >( { DIDL_TRACK_ATTRIBUTE_NAMES } );
        return attributes_list.at( static_cast< size_t >( index ) );
    }
};
struct DidlPhoto {
    static const DIDL_CLASS type = DIDL_CLASS::objectItemImageItemPhoto;
    enum attributes { DIDL_PHOTO_ATTRIBUTES };
    static inline std::string attribute( const int& index ) {
        static auto attributes_list = std::vector< std::string >( { DIDL_PHOTO_ATTRIBUTE_NAMES } );
        return attributes_list.at( static_cast< size_t >( index ) );
    }
};
struct DidlAlbumArt {
    static const DIDL_CLASS type = DIDL_CLASS::objectAlbumArt;
    enum attributes { DIDL_PHOTO_ATTRIBUTES };
    static inline std::string attribute( const int& index ) {
        static auto attributes_list = std::vector< std::string >( { DIDL_PHOTO_ATTRIBUTE_NAMES } );
        return attributes_list.at( static_cast< size_t >( index ) );
    }
};struct DidlEBook {
    static const DIDL_CLASS type = DIDL_CLASS::objectItemEBook;
    enum attributes { DIDL_OBJECT_ATTRIBUTES, size=9, isbn=10, year=11, comment=12, import=13, end=14 };
    static inline std::string attribute( const int& index ) {
        static auto attributes_list = std::vector< std::string >( { DIDL_OBJECT_ATTRIBUTE_NAMES, "size", "isbn", "year", "comment", "import" } );
        return attributes_list.at( static_cast< size_t >( index ) );
    }
};
struct DidlEpisode {
    static const DIDL_CLASS type = DIDL_CLASS::objectItemVideoItemEpisode;
    enum attributes { DIDL_OBJECT_ATTRIBUTES,
                      size=9, season=10, episode=11, width=12, height=13, bitrate=14, color_depth=15, duration=16, import=17, end=18 };
    static inline std::string attribute( const int& index ) {
        static auto attributes_list = std::vector< std::string >( {
            DIDL_OBJECT_ATTRIBUTE_NAMES, "size", "season", "episode", "width", "height", "bitrate", "color_depth", "duration", "import" } );
        return attributes_list.at( static_cast< size_t >( index ) );
    }
};
struct DidlMovie {
    static const DIDL_CLASS type = DIDL_CLASS::objectItemVideoItemMovie;
    enum attributes { DIDL_OBJECT_ATTRIBUTES,
                      size=9, season=10, episode=11, width=12, height=13, bitrate=14, color_depth=15, duration=16, import=17, end=18 };
    static inline std::string attribute( const int& index ) {
        static auto attributes_list = std::vector< std::string >( {
            DIDL_OBJECT_ATTRIBUTE_NAMES, "size", "season", "episode", "width", "height", "bitrate", "color_depth", "duration", "import" } );
        return attributes_list.at( static_cast< size_t >( index ) );
    }
};

template< int val, int end >
struct iterator_t {
    template< typename Fn >
    static void run(Fn f) {
        f(val);
        iterator_t<static_cast<int>(val + 1), end>::run( f );
    }
};
template<int end>
struct iterator_t< end, end > {
    template< typename Fn >
    static void run(Fn) {}
};
template< typename T >
static std::string str ( const std::string delim = ",", const std::string prefix = "" ) {
    std::stringstream _ss;
    bool _first_column=true;
    didl::iterator_t< 0, static_cast< int >( T::end ) >::run( [&delim,&prefix,&_ss,&_first_column] ( const int position ) {
        if( _first_column ) _first_column = false; else _ss << delim;
        _ss << prefix << T::attribute( position );
    });
    return _ss.str();
}

/**
 * @brief epoch time from year
 * @param year 4 digit year
 * @return epoch time
 */
inline size_t epoch_time( const size_t year ) {
    struct tm st_year_ = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    st_year_.tm_hour = 0;   st_year_.tm_min = 0; st_year_.tm_sec = 0;
    st_year_.tm_mon = 0; st_year_.tm_mday = 1;
    st_year_.tm_year = year - 1900;

    time_t tmp_time_ = mktime ( &st_year_ );
    return time_t( tmp_time_ );
}
}//namespace upnp
}//namespace didl

namespace upnp {
namespace dlna {
/** @brief UPNP element profile id */
const static std::string DLNA_PROFILE_ID = "profileID";
/** @brief UPNP profile jpeg thumbnail */
const static std::string DLNA_PROFILE_JPEG_TN = "JPEG_TN";
}//namespace upnp
}//namespace dlna

namespace upnp {
    /** @brief XML UPNP Content Directory NAMESPACE */
    const static std::string XML_NS_UPNP_CDS = "urn:schemas-upnp-org:service:ContentDirectory:1";
    /**
     * @brief XML UPNP Connection Manager NAMESPACE
     */
    const static std::string XML_NS_UPNP_CMS = "urn:schemas-upnp-org:service:ConnectionManager:1";

    /** @brief upnp:class:object.container */
    const static std::string UPNP_CLASS_CONTAINER = "object.container";
    /** @brief upnp:class:object.container.album.musicAlbum */
    const static std::string UPNP_CLASS_MUSIC_ALBUM = "object.container.album.musicAlbum";
    /** @brief upnp:class:object.container.person.musicArtist */
    const static std::string UPNP_CLASS_MUSIC_ARTIST = "object.container.person.musicArtist";
    /** @brief upnp:class:object.container.person.musicArtist */
    const static std::string UPNP_CLASS_MUSIC_TRACK = "object.item.audioItem.musicTrack";

    /** @brief Object ID */
    const static std::string OBJECT_ID = "ObjectID";
    const static std::string BROWSE_FLAG = "BrowseFlag";
    const static std::string BROWSE_METADATA = "BrowseMetadata";
    const static std::string BROWSE_DIRECT_CHILDREN = "BrowseDirectChildren";
    const static std::string BROWSE_RESPONSE = "BrowseResponse";

    const static std::string PARENT_ID = "parentID";


    /** @brief Start Index Parameter */
    const static  std::string START_INDEX = "StartingIndex";
    /** @brief Requested Count Parameter */
    const static std::string REQUESTED_COUNT = "RequestedCount";

    /** @brief Results returned */
    const static std::string NUMBER_RESULT = "NumberReturned";
    /** @brief total matches */
    const static std::string TOTAL_MATCHES = "TotalMatches";
    /** @brief cds update id */
    const static std::string UPDATE_ID = "UpdateID";

    /** @brief XML DIDL NAMESPACE */
    const static std::string XML_NS_DIDL = "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/";
    /** @brief XML PURL NAMESPACE */
    const static std::string XML_NS_PURL = "http://purl.org/dc/elements/1.1/";
    /** @brief XML DLNA NAMESPACE */
    const static std::string XML_NS_DLNA = "urn:schemas-dlna-org:metadata-1-0/";
    /** @brief XML DLNA Metadata NAMESPACE */
    const static std::string XML_NS_DLNA_METADATA = "urn:schemas-dlna-org:metadata-1-0/";
    /** @brief XML PV NAMESPACE */
    const static std::string XML_NS_PV = "http://www.pv.com/pvns/";
    /** @brief XML UPNP NAMESPACE */
    const static std::string XML_NS_UPNP = "urn:schemas-upnp-org:metadata-1-0/upnp/";

/**
 * @brief The UpnpException class
 *
 * Error Codes:
 * 1 Can not parse Upnp SOAP Envelope
 * 2 Unknown UPNP command
 * 3
 * 4
 * 5
 *
 */
class UpnpException : public std::exception {
public:
  explicit UpnpException (int _code, std::string _what) throw() : _code(_code), _what(_what) {}
  virtual ~UpnpException() throw() {}
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

static std::string profile ( const av::Format& format ) {
    std::stringstream _result_VIDEO;
    std::stringstream _result_AUDIO;
//    auto _streams = format.streams();

//    if( _streams.size() > 0 ) {
//TODO        for( auto stream : _streams ) {
//            if( stream.codec_type() == av::CodecType::AUDIO ) {
//                switch( stream.codec() ) {
//                    case av::Codec::FLAC:
//                        _result_AUDIO << "FLAC";
//                        if( stream.bits_per_sample() >> 16 ) {
//                            _result_AUDIO << "_HD";
//                        }
//                    break;

//                case av::Codec::MP3:
//                    _result_AUDIO << "MP3";
//                    break;

//                default:
//                    std::cout << "unkown audio codec: " << av::Codec::name( stream.codec() ) << std::endl;
//                }
//            } else if( stream.codec_type() == av::CodecType::VIDEO) {
//                std::cout << "video: " << std::endl;
//            }
//        }
//    }
    return _result_VIDEO.str() + _result_AUDIO.str();
}

/**
 * @brief The UpnpContentDirectoryRequest class
 */
class UpnpContentDirectoryRequest {
public:

    /** The Reqquest Types */
    enum TYPE { BROWSE, PROTOCOL_INFO, X_FEATURE_LIST };
    /**
     * @brief UpnpContentDirectoryRequest
     * @param type
     */
    UpnpContentDirectoryRequest( TYPE type ) : type( type ) {}
    const TYPE type;
    void addValue( std::string key, std::string value ) {
        props[ key ] = value;
    }
    /**
     * @brief get the property names.
     * @return
     */
    std::vector< std::string > getNames() const {
        std::vector< std::string > names;
        for( const auto &iter : props ) {
            names.push_back( iter.first );
        }
        return names;
    }
    /**
     * @brief get property value for a key.
     * @param key
     * @return
     */
    std::string getValue( const std::string & key ) {
        return props[ key ];
    }
    /**
     * @brief test if properties contains a key.
     * @param key
     * @return
     */
    bool contains( const std::string & key ) {
        return( props.find( key ) != props.end() );
    }
    /**
     * @brief test if properties with key and value are equal.
     * @param key
     * @param value
     * @return
     */
    bool contains( const std::string & key, const std::string & value ) {
        if( contains( key ) ) {
            return props[ key ] == value;
        }
        return false;
    }
    /**
     * @brief operator write the properties to the ostream.
     */
    friend std::ostream& operator <<(std::ostream &os,const UpnpContentDirectoryRequest &obj) {
        os <<  "UpnpContentDirectoryRequest:" << std::endl;
        for( const auto &iter : obj.props ) {
            os << "\t" << iter.first << " = " << iter.second << std::endl;
        }
        return os;
    }
private:
    std::map< std::string, std::string > props;
};

inline UpnpContentDirectoryRequest parseRequest( std::string request ) {

    std::cout << "parse upnp request:" << request << std::endl;

    commons::xml::XMLReader reader( request );
    std::vector< commons::xml::Node > root_node = reader.getElementsByName( soap::XML_NS_SOAP, soap::XML_RESULT_BODY );
    if( root_node.size() == 1 ) {
        std::vector< commons::xml::Node > upnp_command = root_node[0].children();
        if( upnp_command.size() == 1 ) {
            if( upnp_command[0].ns() == XML_NS_UPNP_CDS && upnp_command[0].name() == "Browse" ) {
                UpnpContentDirectoryRequest request( UpnpContentDirectoryRequest::BROWSE );
                std::vector< commons::xml::Node > upnp_props = upnp_command[0].children();
                for( auto prop : upnp_props ) {
                    request.addValue( prop.name(), prop.content() );
                }
                return request;
            } else if( upnp_command[0].ns() == XML_NS_UPNP_CMS && upnp_command[0].name() == "GetProtocolInfo" ) {
                UpnpContentDirectoryRequest request( UpnpContentDirectoryRequest::PROTOCOL_INFO );
                return request;

            } else if( upnp_command[0].ns() == XML_NS_UPNP_CDS && upnp_command[0].name() == "UPNP_COMMAND_FEATURE_LIST" ) {
                UpnpContentDirectoryRequest request( UpnpContentDirectoryRequest::X_FEATURE_LIST );
                return request;

            } else {
                throw UpnpException(1, "unkown upnp command: " +  upnp_command[0].ns() + ":" + upnp_command[0].name() );
            }

        } else {
            throw UpnpException(1, "unkown upnp command." );
        }
    } else {
        throw UpnpException(1, "can not parse upnp SOAP envelope." );
    }
};

/**
 * @brief The Icon struct
 */
struct Icon {
public:
    Icon() : width_(0), height_(0), depth_(0) {}
    Icon ( const Icon& ) = default;
    Icon ( Icon&& ) = default;
    Icon& operator= ( const Icon& ) = default;
    Icon& operator= ( Icon&& ) = default;
    ~Icon() {}

    /**
     * @brief width
     * @return
     */
    int width() const { return width_; }
    /**
     * @brief width
     * @param width
     */
    void width( const int & width ) { width_ = width; }
    /**
     * @brief height
     * @return
     */
    int height() const { return height_; }
    /**
     * @brief height
     * @param height
     */
    void height( const int & height ) { height_ = height; }
    /**
     * @brief depth
     * @return
     */
    int depth() const { return depth_; }
    /**
     * @brief depth
     * @param depth
     */
    void depth( const int & depth ) { depth_ = depth; }
    /**
     * @brief mimeType
     * @return
     */
    std::string mimeType() const { return mimeType_; }
    /**
     * @brief mimeType
     * @param mimeType
     */
    void mimeType( const std::string & mimeType ) { mimeType_ = mimeType; }
    /**
     * @brief url
     * @return
     */
    std::string url() const { return url_; }
    /**
     * @brief url
     * @param url
     */
    void url( const std::string & url ) { url_ = url; }

private:
    int width_, height_, depth_;
    std::string mimeType_, url_;
};

struct Service {
public:
    Service() {}
    Service ( const Service& ) = default;
    Service ( Service&& ) = default;
    Service& operator= ( const Service& ) = default;
    Service& operator= ( Service&& ) = default;
    ~Service() {}

    std::string serviceType() const { return service_type_; }
    void serviceType( const std::string & service_type ) { service_type_ = service_type; }
    std::string serviceId() const { return service_id_; }
    void serviceId( const std::string & service_id ) { service_id_ = service_id; }
    std::string scpdUrl() const { return scpd_url_; }
    void scpdUrl( const std::string & scpd_url ) { scpd_url_ = scpd_url; }
    std::string controlUrl() const { return control_url_; }
    void controlUrl( const std::string & control_url ) { control_url_ = control_url; }
    std::string eventSubUrl() const { return event_sub_url_; }
    void eventSubUrl( const std::string & event_sub_url ) { event_sub_url_ = event_sub_url; }

private:
    std::string service_type_, service_id_, scpd_url_, control_url_, event_sub_url_;
};

/**
 * @brief The UpnpDevice item
 */
struct UpnpDevice {
public:
    UpnpDevice() : timestamp_( std::chrono::high_resolution_clock::now() ) {}
    UpnpDevice ( const UpnpDevice& ) = default;
    UpnpDevice ( UpnpDevice&& ) = default;
    UpnpDevice& operator= ( const UpnpDevice& ) = default;
    UpnpDevice& operator= ( UpnpDevice&& ) = default;
    ~UpnpDevice() {}

    /**
     * Required. Major version of the UPnP Device Architecture. Shall be 2 for devices implemented
     * on a UPnP 2. 0 architecture.
     * @brief set version Major
     * @return
     */
    int versionMajor() const { return versionMajor_; }
    /**
     * @brief get version Major
     * @param versionMajor
     */
    void versionMajor( const int & versionMajor) { versionMajor_ = versionMajor; }
    /**
     * Required. Minor version of the UPnP Device Architecture. Shall be 0 for devices implemented
     * on a UPnP 2.0 architecture. Shall accurately reflect the version number of  the UPnP Device
     * Architecture supported by the device.
     * @brief get version Minor
     * @return
     */
    int versionMinor() const { return versionMinor_; }
    /**
     * @brief set version Minor
     * @param versionMinor
     */
    void versionMinor( const int & versionMinor) { versionMinor_ = versionMinor; }
    /**
     * Required. UPnP device type. Single URI.
     * <ul><li>For standard devices  defined by a UPnP Forum working committee, shall begin with
     * "urn :schemas -upnp-org:device:"  followed  by the standardized device type suffix, a colon,
     * and an integer device version i.e. urn:schemas-upnp-org:device:deviceType:ver. The
     * highest supported version of  the device type shall be specified.
     * </li><li>For non-standard devices  specified by UPnP vendors ,  shall begin with “urn:” , followed by
     * a  Vendor Doma in  Name, followed by “:device:”, followed by a  device type suffix, colon,
     * and an integer version, i.e., “urn:domain-name:device:deviceType:ver”. Period
     * characters in the Vendor Domain Name shall be replaced with hyphens in accordance
     * with RFC 2141. The highest supported version of the device type shall be specified.</li></ul>
     * The device type suffix defined by a UPnP Forum working committee or specif ied by a UPnP
     * vendor shall be <= 64 chars, not counting the version suffix and separating colon.
     * @brief device Type
     * @return
     */
    std::string deviceType() const { return deviceType_; }
    /**
     * @brief device Type
     * @param deviceType
     */
    void deviceType( const std::string & deviceType) { deviceType_ = deviceType; }
    /**
     * Required. Short description for end user. Specified by UPnP vendor. String.
     * Should be < 64 characters.
     * @brief friendlyName
     * @return
     */
    std::string friendlyName() const { return friendlyName_; }
    /**
     * @brief friendlyName
     * @param friendlyName
     */
    void friendlyName( const std::string & friendlyName) { friendlyName_ = friendlyName; }
    /**
     * Required.  Manufacturer's name. Specified by UPnP vendor. String. Should be < 64
     * characters.
     * @brief manufacturer
     * @return
     */
    std::string manufacturer() const { return manufacturer_; }
    /**
     * @brief manufacturer
     * @param manufacturer
     */
    void manufacturer( const std::string & manufacturer) { manufacturer_ = manufacturer; }
    /**
     * Allowed. Web site for Manufacturer. Single URL.
     * @brief manufacturerUrl
     * @return
     */
    std::string manufacturerUrl() const { return manufacturerUrl_; }
    /**
     * @brief manufacturerUrl
     * @param manufacturerUrl
     */
    void manufacturerUrl( const std::string & manufacturerUrl) { manufacturerUrl_ = manufacturerUrl; }
    /**
     * Recommended. Long description for end user. Specified by UPnP vendor. String.
     * Should be < 128 characters.
     * @brief modelDescription
     * @return
     */
    std::string modelDescription() const { return modelDescription_; }
    /**
     * @brief modelDescription
     * @param modelDescription
     */
    void modelDescription( const std::string & modelDescription) { modelDescription_ = modelDescription; }
    /**
     * Required. Model name. Specified by UPnP vendor. String. Should be < 32 characters.
     * @brief modelName
     * @return
     */
    std::string modelName() const { return modelName_; }
    /**
     * @brief modelName
     * @param modelName
     */
    void modelName( const std::string & modelName) { modelName_ = modelName; }
    /**
     * Recommended. Model number. Specified by UPnP vendor. String. Should be < 32 characters.
     * @brief modelNumber
     * @return
     */
    std::string modelNumber() const { return modelNumber_; }
    /**
     * @brief modelNumber
     * @param modelNumber
     */
    void modelNumber( const std::string & modelNumber) { modelNumber_ = modelNumber; }
    /**
     * Allowed. Web site for model. Specified by UPnP vendor. Single URL.
     * @brief modelUrl
     * @return
     */
    std::string modelUrl() const { return modelUrl_; }
    /**
     * @brief modelUrl
     * @param modelUrl
     */
    void modelUrl( const std::string & modelUrl) { modelUrl_ = modelUrl; }
    /**
     * Recommended. Serial number.  Specified by UPnP vendor. String. Should be < 64 characters.
     * @brief serialNumber
     * @return
     */
    std::string serialNumber() const { return serialNumber_; }
    /**
     * @brief serialNumber
     * @param serialNumber
     */
    void serialNumber( const std::string & serialNumber) { serialNumber_ = serialNumber; }
    /**
     * Required.  Unique Device Name. Universally- unique identifier for the device, whether  root or
     * embedded. shall be the same over time for a specific device instance ( i. e. , shall  survive
     * reboots) . shall match the field value of the NT header field in device discovery messages.
     * shall match the prefix of the USN header field in all disc overy messages. shall begin with
     * “uuid:” followed by a UUID suffix specified by a UPnP vendor.
     * @brief udn
     * @return
     */
    std::string udn() const { return udn_; }
    /**
     * @brief udn
     * @param udn
     */
    void udn( const std::string & udn) { udn_ = udn; }
    /**
     * Allowed. Universal Product Code.  12-digit,  all- numericc ode that  identifies the consumer
     * package. Managed by the Uniform Code Council. Specified by UPnP vendor. Single UPC.
     * @brief upc
     * @return
     */
    std::string upc() const { return upc_; }
    /**
     * @brief upc
     * @param upc
     */
    void upc( const std::string & upc) { upc_ = upc; }
    /**
     * Recommended. URL to presentation for device. shall be
     * relative to the URL at which the device description is located in accordance with the rules
     * spec ified in clause 5 of  RFC 3986. Specified by UPnP vendor. Single URL.
     * @brief presentationUrl
     * @return
     */
    std::string presentationUrl() const { return presentationUrl_; }
    /**
     * @brief presentationUrl
     * @param presentationUrl
     */
    void presentationUrl( const std::string & presentationUrl) { presentationUrl_ = presentationUrl; }

    /**
     * Required if  and only if  device has one or more icons. Specified by  UPnP vendor.
     * @brief iconList
     * @return
     */
    std::vector< Icon > iconList() const { return images_; }
    /**
     * @brief addIcon
     * @param icon
     */
    void addIcon( Icon icon ) { images_.push_back( icon ); }

    /**
     * @brief serviceList
     * @return
     */
    std::vector< Service > serviceList() const { return services_; }
    /**
     * @brief addService
     * @param service
     */
    void addService( Service service ) { services_.push_back( service ); }

    std::chrono::high_resolution_clock::time_point timestamp() {
        return timestamp_;
    }
    void touch() {
        timestamp_ = std::chrono::high_resolution_clock::now();
    }
    bool timeout() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto dur = end_time - timestamp_;
        auto f_secs = std::chrono::duration_cast<std::chrono::duration<unsigned int>> ( dur );
        return( f_secs.count() >= timeout_ );
    }
    void timeout(time_t timeout ) {
        timeout_ = timeout;
    }

private:
    int versionMajor_, versionMinor_;
    std::string deviceType_, friendlyName_, manufacturer_, manufacturerUrl_, modelDescription_, modelName_,
                modelNumber_, modelUrl_, serialNumber_, udn_, upc_, presentationUrl_;
    std::vector< Icon > images_;
    std::vector< Service > services_;
    std::vector< UpnpDevice > devices_;
    std::chrono::high_resolution_clock::time_point timestamp_;
    time_t timeout_ = 1800;
};


inline void parseDescription( const std::string & description, UpnpDevice & device ) {
    commons::xml::XMLReader reader( description );
    commons::xml::Node rootNode = reader.getElementsByName("root")[0];
    for( auto node : rootNode.children() ) {
        if( node.name() == "specVersion" ) {
            for( auto specVersionNode : node.children() ) {
                if( specVersionNode.name() == "major" ) {
                    device.versionMajor( std::stoi( specVersionNode.content() ) );
                } else if( specVersionNode.name() == "minor") {
                    device.versionMinor( std::stoi( specVersionNode.content() ) );
                }
            }
        } else if( node.name() == "device" ) {
            for( auto deviceNode : node.children() ) {
                if( deviceNode.name() == "UDN" ) {
                    device.udn( deviceNode.content() );
                } else if( deviceNode.name() == "UPC" ) {
                    device.upc( deviceNode.content() );
                } else if( deviceNode.name() == "serialNumber" ) {
                    device.serialNumber( deviceNode.content() );
                } else if( deviceNode.name() == "modelNumber" ) {
                    device.modelNumber( deviceNode.content() );
                } else if( deviceNode.name() == "modelName" ) {
                    device.modelName( deviceNode.content() );
                } else if( deviceNode.name() == "modelDescription" ) {
                    device.modelDescription( deviceNode.content() );
                } else if( deviceNode.name() == "modelURL" ) {
                    device.modelUrl( deviceNode.content() );
                } else if( deviceNode.name() == "manufacturerURL" ) {
                    device.manufacturerUrl( deviceNode.content() );
                } else if( deviceNode.name() == "manufacturer" ) {
                    device.manufacturer( deviceNode.content() );
                } else if( deviceNode.name() == "friendlyName" ) {
                    device.friendlyName( deviceNode.content() );
                } else if( deviceNode.name() == "presentationURL" ) {
                    device.presentationUrl( deviceNode.content() );
                } else if( deviceNode.name() == "deviceType" ) {
                    device.deviceType( deviceNode.content() );
                }  else if( deviceNode.name() == "iconList" ) {
                    for( auto iconListNode : deviceNode.children() ) {
                        if( iconListNode.name() == "icon" ) {
                            upnp::Icon icon;
                            for( auto iconNode : iconListNode.children() ) {
                                if( iconNode.name() == "mimetype" ) {
                                    icon.mimeType( iconNode.content() );
                                } else if( iconNode.name() == "width" ) {
                                    icon.width( std::stoi( iconNode.content() ) );
                                } else if( iconNode.name() == "height" ) {
                                    icon.height( std::stoi ( iconNode.content() ) );
                                } else if( iconNode.name() == "depth" ) {
                                    icon.depth( std::stoi ( iconNode.content() ) );
                                } else if( iconNode.name() == "url" ) {
                                    icon.url( iconNode.content() );
                                }
                            }
                            device.addIcon( std::move( icon ) );
                        }
                    }
                }  else if( deviceNode.name() == "serviceList" ) {
                    for( auto serviceListNode : deviceNode.children() ) {
                        if( serviceListNode.name() == "service" ) {
                            upnp::Service service;
                            for( auto serviceNode : serviceListNode.children() ) {
                                if( serviceNode.name() == "serviceType" ) {
                                    service.serviceType( serviceNode.content() );
                                } else if( serviceNode.name() == "serviceId" ) {
                                    service.serviceId( serviceNode.content() );
                                } else if( serviceNode.name() == "SCPDURL" ) {
                                    service.scpdUrl( serviceNode.content() );
                                } else if( serviceNode.name() == "controlURL" ) {
                                    service.controlUrl( serviceNode.content() );
                                } else if( serviceNode.name() == "eventSubURL" ) {
                                    service.eventSubUrl( serviceNode.content() );
                                }
                            }
                            device.addService( std::move( service ) );
                        }
                    }
                }
            }
        } else if( node.name() == "URLBase" ) {
            //TODO
        } else { CLOG(DEBUG, "upnp") << "unknown element: " << node.name(); }
    }
};
inline upnp::UpnpDevice deviceDescription( const ssdp::SsdpEvent& event ) {
    upnp::UpnpDevice _device;

    //make the request
    std::stringstream _sstream;
    std::cout << "get device description " << event.location << std::endl;
//    auto _response = http::get( event.location, _sstream );
//    if( _response.status() == http::http_status::OK ) {
//        parseDescription( _sstream.str(), _device );
//    } else {
//        CLOG( ERROR, "upnp" ) << "get device description returned " << static_cast< int >( _response.status() ) << ", for " << event.location;
//    }
    return _device;
}

}//namespace upnp
#endif // UPNP_H
