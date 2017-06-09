# -------------------------------------------------------------------------
# Build squawk gui
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.0)

project( gui )

# -------------------------------------------------------------------------
# ---------------------   External Dependencies       ---------------------
# -------------------------------------------------------------------------
find_package(Boost 1.54.0 COMPONENTS filesystem system)
find_package(LibAVCodec COMPONENTS avformat avcodec avfilter avresample swscale avutil )
SET( LIBS ${LIBS}
    ${Boost_LIBRARIES} ${AVCODEC_LIBRARIES}
#    -lpcrecpp -lpthread -luuid -lswresample -lm -lz
)
SET( INCLUDES ${INCLUDES} ${Boost_INCLUDE_DIRS} ${AVCODEC_INCLUDES} )

# -------------------------------------------------------------------------
# -----------------------   Compile Binary          -----------------------
# -------------------------------------------------------------------------
#include_directories(${ROOT} ${INCLUDES})
#aux_source_directory( ${PROJECT_SOURCE_DIR} SQUAWK_SOURCES )

FIND_PACKAGE(wxWidgets)
    IF(wxWidgets_FOUND)
    INCLUDE("${wxWidgets_USE_FILE}")
    ADD_EXECUTABLE(gui main.cpp)
    # and for each of your dependant executable/library targets:
    TARGET_LINK_LIBRARIES(gui ${wxWidgets_LIBRARIES})
ELSE(wxWidgets_FOUND)
    # For convenience. When we cannot continue, inform the user
    MESSAGE("wxWidgets not found!")
ENDIF(wxWidgets_FOUND)
