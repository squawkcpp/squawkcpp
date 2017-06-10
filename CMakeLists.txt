cmake_minimum_required(VERSION 3.0)

project (squawk)

SET(MAJOR_VERSION "0")
SET(MINOR_VERSION "1")
SET(PATCH_VERSION "1" CACHE PATCH_VERSION "The package build number." )

# -------------------------------------------------------------------------
# -----------------------   Options and Parameters  -----------------------
# -------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
option(build_tests "Build all squawk unit tests." ON)
SET(TESTFILES "/home/e3a/testfiles" CACHE TESTFILES "The path to the testfiles.")
SET(BOWER_COMPONENTS "videogular" "videogular-themes-default" "videogular-controls" "videogular-buffering"
                     "videogular-overlay-play" "videogular-poster" "videogular-ima-ads" "videogular-angulartics"
#                     "bootstrap"
                     "angular-animate" "angular-aside" "angular-bootstrap" "angular-route" "angular-sanitize" "blueimp-gallery"
                     "ngInfiniteScroll#1.0.0" "api-console" )
#                     "ngInfiniteScroll#1.0.0" "dirPagination" "font-awesome" "api-console" "jquery" "angular-utils-pagination")

# -------------------------------------------------------------------------
# -----------------------   Config Toolchain        -----------------------
# -------------------------------------------------------------------------
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++14)
endif()
set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG} -O0 -g)
#add_definitions(-DASIO_STANDALONE -DASIO_ENABLE_HANDLER_TRACKING -DASIO_ENABLE_BUFFER_DEBUGGING )
add_definitions(-DASIO_STANDALONE -DASIO_ENABLE_BUFFER_DEBUGGING )
add_definitions(-DDEBUG )
add_definitions(-DSQUAWK_RELEASE="${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
add_definitions(-DTESTFILES="${TESTFILES}/")
add_definitions(-DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}/")
if( build_tests )
add_definitions(-DBUILD_TESTS)
endif()
add_definitions(-Wall -Wextra)

IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
    add_definitions(-DNDEBUG)
    add_definitions(-DELPP_DISABLE_TRACE_LOGS)
ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)

#add headers for editor
FILE(GLOB_RECURSE HeaderFiles "${PROJECT_SOURCE_DIR}/src/*.h")
add_custom_target(headers SOURCES ${HeaderFiles})

# -------------------------------------------------------------------------
# ---------------------   External Dependencies       ---------------------
# -------------------------------------------------------------------------
include(ExternalProject)
include(cmake/external/LibAvCpp.cmake)
include(cmake/external/LibImageCpp.cmake)
include(cmake/external/re2.cmake)
include(cmake/external/gtest.cmake)
include(cmake/external/asio.cmake)
include(cmake/external/fmt.cmake)
include(cmake/external/easylogging.cmake)
include(cmake/external/lightning.cmake)
include(cmake/external/rapidjson.cmake)

SET( INCLUDES ${INCLUDES} ${GTEST_INCLUDE_DIR} ${FMT_INCLUDE_DIR}
            ${EASYLOGGING_INCLUDE_DIR} ${RE2_INCLUDE_DIR} ${ASIO_INCLUDE_DIR}
            ${LIGHTNING_INCLUDE_DIR} ${RAPIDJSON_INCLUDE_DIR} ${LIBAV_INCLUDE_DIR} ${LIB_IMAGE_INCLUDE_DIR} )

# -------------------------------------------------------------------------
# -----------------------   Find Dependencies       -----------------------
# -------------------------------------------------------------------------
find_package(Boost 1.54.0 COMPONENTS filesystem system)
#find_package(CURLpp REQUIRED)
find_package(CURL REQUIRED)
find_package(PCRE++ REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(Sqlite3 REQUIRED)
find_package(Imlib2 REQUIRED)
find_package(LibAVCodec COMPONENTS avformat avcodec avfilter avresample avutil )
find_package(Poppler REQUIRED cpp)
find_package(OpenSSL REQUIRED)
find_package(Flac++ REQUIRED)
find_package(LibArchive REQUIRED)
find_package(LibSigc++ REQUIRED)

SET( LIBS ${LIBS}
    ${Boost_LIBRARIES} ${CURL_LIBRARIES}
    ${LIBXML2_LIBRARIES} ${SQLITE3_LIBRARY} ${IMLIB2_LIBRARIES}
    ${AVCODEC_LIBRARIES} ${POPPLER_LIBRARIES} ${OPENSSL_LIBRARIES}
    ${FLAC++_LIBRARIES} ${LIBSIGCXX2_LIBRARIES} ${LibArchive_LIBRARIES}
    -lpcrecpp -lpthread -luuid -lswresample -lm -lz
)
if(build_tests)
    SET( LIBS ${LIBS} gtest)
endif()

SET( INCLUDES ${INCLUDES}
    ${Boost_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIR} ${CURL_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIR} ${IMLIB2_INCLUDE_DIR} ${AVCODEC_INCLUDES}
    ${OPENSSL_INCLUDES} ${FLAC++_INCLUDE_DIR} ${PCREPP_INCLUDE_DIR}
    ${LIBSIGCXX2_INCLUDE_DIRS} ${LibArchive_INCLUDE_DIRS}
)

# -------------------------------------------------------------------------
# -----------------------   Compile Binary          -----------------------
# -------------------------------------------------------------------------
include_directories(${ROOT} ${INCLUDES})
aux_source_directory( src/ SQUAWK_SOURCES )
aux_source_directory( src/db SQUAWK_SOURCES_DB )
aux_source_directory( src/utils SQUAWK_SOURCES_UTILS )
aux_source_directory( src/upnp SQUAWK_SOURCES_UPNP )
aux_source_directory( src/ssdp SQUAWK_SOURCES_SSDP )
aux_source_directory( src/ssdp/asio SQUAWK_SOURCES_SSDP_ASIO )
aux_source_directory( src/libav SQUAWK_SOURCES_LIBAV )

add_executable(squawk squawkmain.cpp ${SQUAWK_SOURCES} ${SQUAWK_SOURCES_DB} ${SQUAWK_SOURCES_UPNP}
    ${SQUAWK_SOURCES_SSDP} ${SQUAWK_SOURCES_SSDP_ASIO} ${SQUAWK_SOURCES_UTILS} ${SQUAWK_SOURCES_LIBAV} )
target_link_libraries(squawk easylogging fmt lightning asio re2 rapidjson LibAvCpp LibImageCpp ${LIBS})
install(TARGETS squawk RUNTIME DESTINATION bin)

#add_executable(aws src/utils/aws.cpp src/utils/amazonfacade.cpp ${LIBXML2_INCLUDE_DIR} ${CURL_INCLUDE_DIRS} )
#target_link_libraries(aws ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES} -lm  ${LIBXML2_LIBRARIES} )

add_subdirectory( cds )

# -------------------------------------------------------------------------
# -----------------------   Manage Test Cases       -----------------------
# -------------------------------------------------------------------------
if (build_tests)
    aux_source_directory( test/ SQUAWK_SOURCES_TEST )
    enable_testing()
    include_directories(${ROOT} ${INCLUDES} ${GTEST_INCLUDE_DIR})
    add_executable(test_squawk testmain.cpp ${SQUAWK_SOURCES_TEST} ${SQUAWK_SOURCES}
        ${SQUAWK_SOURCES_DB} ${SQUAWK_SOURCES_UPNP} ${SQUAWK_SOURCES_SSDP}
        ${SQUAWK_SOURCES_SSDP_ASIO} ${SQUAWK_SOURCES_UTILS} ${SQUAWK_SOURCES_LIBAV} )
    target_link_libraries(test_squawk lightning fmt re2 easylogging asio rapidjson LibAvCpp LibImageCpp gtest ${LIBS} )
    add_test(squawk-tests test_squawk)
endif()

add_subdirectory ( gui )

# -------------------------------------------------------------------------
# -----------------------   Bower Components        -----------------------
# -------------------------------------------------------------------------
add_custom_target(bower /usr/local/bin/bower --no-color -f -s --allow-root install ${BOWER_COMPONENTS}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Download Bower Components" VERBATIM
)
# install(DIRECTORY ${CMAKE_BINARY_DIR}/bower_components/ DESTINATION var/lib/squawk/bower_components )

# -------------------------------------------------------------------------
# -----------------------   Doxygen                 -----------------------
# -------------------------------------------------------------------------
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

# -------------------------------------------------------------------------
# -----------------------   Generate Package        -----------------------
# -------------------------------------------------------------------------
IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
install(DIRECTORY ${CMAKE_SOURCE_DIR}/docroot/ DESTINATION var/lib/squawk/docroot )
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_SET_DESTDIR "on")
SET(CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
SET(CPACK_GENERATOR "DEB")

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

SET(CPACK_PACKAGE_DESCRIPTION "loudlabs media server")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "squawk DLNA media server.")
SET(CPACK_PACKAGE_VENDOR "loudlabs")
SET(CPACK_PACKAGE_CONTACT "e.knecht@netwings.ch")
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")

SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libpcrecpp0v5, libimlib2, libavcodec-ffmpeg56, libavformat-ffmpeg56, libavutil-ffmpeg54, libpoppler-cpp0, libboost-filesystem1.58.0")
SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
SET(CPACK_DEBIAN_PACKAGE_SECTION "media")
SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
SET(CPACK_COMPONENTS_ALL applications Libraries ApplicationData)
INCLUDE(CPack)

ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
