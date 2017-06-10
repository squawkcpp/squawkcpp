# -------------------------------------------------------------------------
# Build content directory server
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.0)

project( cds )

# -------------------------------------------------------------------------
# ---------------------   External Dependencies       ---------------------
# -------------------------------------------------------------------------
find_package(Boost 1.54.0 COMPONENTS filesystem system)
find_package(Hiredis)
find_package(Libev)
find_package(Boost 1.54.0 COMPONENTS filesystem system)

#include(ExternalProject)
include(${CMAKE_SOURCE_DIR}/cmake/external/redox.cmake)

SET( INCLUDES ${INCLUDES}
    ${Boost_INCLUDE_DIRS}
    ${AVCODEC_INCLUDES}
    ${GTEST_INCLUDE_DIR}
    ${REDOX_INCLUDE_DIR}
)
SET( LIBS ${LIBS}
    ${Boost_LIBRARIES} ${AVCODEC_LIBRARIES}
    ${CPP_REDIS_LIBRARIES} ${LIBHIREDIS_LIBRARIES} ${LIBEV_LIBRARIES}
#    -lpcrecpp -lpthread -luuid -lswresample -lm -lz
)

SET( INCLUDES ${INCLUDES} ${Boost_INCLUDE_DIRS} ${AVCODEC_INCLUDES} ${CPP_REDIS_INCLUDE_DIR})

# -------------------------------------------------------------------------
# -----------------------   Compile Binary          -----------------------
# -------------------------------------------------------------------------
aux_source_directory( ${PROJECT_SOURCE_DIR} CDS_SOURCES )
include_directories(${ROOT} ${INCLUDES} ${GTEST_INCLUDE_DIR})
add_executable(cds ${CDS_SOURCES})
target_link_libraries(cds redox ${LIBS} )

