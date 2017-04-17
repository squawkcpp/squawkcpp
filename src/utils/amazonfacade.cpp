#include "amazonfacade.h"

#include <sstream>

#include <openssl/engine.h>
#include <openssl/hmac.h>

#include <curl/curl.h> //TODO remove curl
////#include "http/httpclient.h"
#include "http/utils/stringutils.h"
#include "textutils.h"
#include "xml.h"

namespace utils {

//TODO remove
std::string AmazonFacade::data;
size_t AmazonFacade::writeCallback(char* buf, size_t size, size_t nmemb, void* /*up*/) {
    //callback must have this declaration
    //buf is a pointer to the data that curl has for us
    //size*nmemb is the size of the buffer

    for (size_t c = 0; c<size*nmemb; c++) {
        data.push_back(buf[c]);
    }
    return size*nmemb; //tell curl how many bytes we handled
}

BookResultItem AmazonFacade::bookByIsbn( const std::string & access_key, const std::string & key, const std::string & isbn ) {

    std::string _isbn = squawk::utils::TextUtils::clean_isbn ( isbn );

    std::map< std::string, std::string > map;
    map["AssociateTag"] = "squawk08-20";
    map["Keywords"] = _isbn;
    map["Operation"] = "ItemSearch";
    map["ResponseGroup"] = "Large";
    map["SearchIndex"] = "Books";
    map["Service"] = "AWSECommerceService";
    map["Timestamp"] = get_utc_time_string();
    map["Version"] = "2009-03-31";
    map["AWSAccessKeyId"] = access_key;

    std::string query = canonicalize( map );

    std::stringstream ss;
    ss << "GET\n" <<
          "ecs.amazonaws.com" << "\n" <<
          "/onca/xml" << "\n" <<
          query;

    std::stringstream buf;
    buf << "http://ecs.amazonaws.com/onca/xml?" << query << "&Signature=" <<
        http::utils::UrlEscape::urlEncode( generate_hmac256bit_hash( ss.str().c_str(), key.c_str() ).c_str() );

    std::cout << "get book from amazon, ISBN: " << _isbn << std::endl;
//    std::stringstream _ss;
//    auto _response = http::get( buf.str(), _ss );
//    if( _response.status() != http::http_status::OK ) {
//        std::cout << "bad amazon response: " << _response << std::endl;
//    }
//    data = _ss.str();

    data = "";
    CURL *curl;
      CURLcode res;

      curl = curl_easy_init();
      if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, buf.str().c_str() );
        /* example.com is redirected, so we tell libcurl to follow redirection */
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &AmazonFacade::writeCallback);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
          fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);

      }
      std::cout << "amazon result: " << data << std::endl;
      return parse( data );
}
std::string AmazonFacade::generate_hmac256bit_hash(const char *message, const char *key_buf) {
    unsigned char* result;
    unsigned int result_len = 32;
    HMAC_CTX ctx;

    result = (unsigned char*) malloc(sizeof(char) * result_len);

    ENGINE_load_builtin_engines();
    ENGINE_register_all_complete();

    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, key_buf, strlen( key_buf )/*16*/, EVP_sha256(), NULL);
    HMAC_Update(&ctx, (unsigned char *)message, strlen( message ) );
    HMAC_Final(&ctx, result, &result_len);
    HMAC_CTX_cleanup(&ctx);

    return std::string( base64( result, result_len ) );
}

char * AmazonFacade::base64(const unsigned char *input, int length) {
   BIO *bmem, *b64;
   BUF_MEM *bptr;

  b64 = BIO_new(BIO_f_base64());
   bmem = BIO_new(BIO_s_mem());
   b64 = BIO_push(b64, bmem);
   BIO_write(b64, input, length);
   int res = BIO_flush(b64);
   if( res < 1 ) {
       std::cerr << "error calculating mac." << std::endl;
   }
   BIO_get_mem_ptr(b64, &bptr);

  char *buff = (char *)malloc(bptr->length);
   memcpy(buff, bptr->data, bptr->length-1);
   buff[bptr->length-1] = 0;

  BIO_free_all(b64);

  return buff;
}
BookResultItem AmazonFacade::parse( const std::string & response ) {

    BookResultItem result;
    commons::xml::XMLReader reader( response );

    std::vector< commons::xml::Node > errors = reader.getElementsByName( "Error" );
    std::vector< commons::xml::Node > results = reader.getElementsByName( "Items" );
    if( ! results.empty() ) {
        for( auto _node_results : results[0].children() ) {
            if( _node_results.name() == "TotalResults" ) {
                result.count = std::atoi( _node_results.content().c_str() );
            }
        }
        if( ! errors.empty() ) {
            for( auto node : errors[0].children() ) {
                if( node.name() == "Code" ) {
                    if( node.content() == "AWS.ECommerceService.NoExactMatches" )
                    { result.status = 404; }
                    else std::cout << "unknown error code " << node.content() << std::endl;
                }
            }
        }
    }

    if( result.count == 0 || result.status != 200 ) return result;

    std::vector< commons::xml::Node > images = reader.getElementsByName( "LargeImage" );
    for( auto node : images[0].children() ) {
        if(node.name() == "URL" ) {
            result.coverUri = node.content();
        }
    }

    std::vector< commons::xml::Node > attributes =  reader.getElementsByName( "ItemAttributes" );
    for( auto node : attributes[0].children() ) {
        if(node.name() == "Author" && result.author == "" ) {
            result.author = node.content();
        } else if(node.name() == "Publisher" ) {
            result.publisher = node.content();
        } else if(node.name() == "PublicationDate" ) {
            result.publicationDate = node.content();
        } else if(node.name() == "Title" ) {
            result.title = node.content();
        } else if(node.name() == "ISBN" ) {
            result.isbn = node.content();
        }
    }

    std::vector< commons::xml::Node > reviews =  reader.getElementsByName( "EditorialReview" );
    if( ! reviews.empty() ) {
        for( auto node : reviews[0].children() ) {
            if(node.name() == "Content" && result.description == "" ) {
                result.description = node.content();
            }
        }
    }
    return result;
}
std::string AmazonFacade::canonicalize( const std::map<std::string, std::string> & sortedParamMap ) {
    if ( sortedParamMap.empty() ) {
        return "";
    }

    std::stringstream ss;
    bool first = true;
    for( auto itr : sortedParamMap ) {
        if( first ) { first = false; }
        else { ss << "&"; }
        ss << http::utils::UrlEscape::urlEncode( itr.first.c_str() ) << "=" << http::utils::UrlEscape::urlEncode( itr.second.c_str() );
    }

    return ss.str();
}
std::string AmazonFacade::get_utc_time_string() {
        char timestamp_str[20];
        time_t rawtime;
        struct tm *ptm;

        time(&rawtime);
        ptm = gmtime(&rawtime);
        sprintf(timestamp_str, "%d-%02d-%02dT%02d:%02d:%02dZ", 1900+ptm->tm_year, 1+ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
        return std::string(timestamp_str);
}
}//namespace utils
