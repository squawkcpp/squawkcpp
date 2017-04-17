include(ExternalProject)


# ---------------------         easylogging++          ---------------------
ExternalProject_Add(
  easyloggingpp
  URL "https://github.com/easylogging/easyloggingpp/archive/v9.80.zip"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
)
ExternalProject_Get_Property(easyloggingpp source_dir)
set(EASYLOGGING_INCLUDE_DIR "${source_dir}/src" )
set(EASYLOGGING_LIBRARY easylogging)
add_library(${EASYLOGGING_LIBRARY} INTERFACE IMPORTED)
add_dependencies(${EASYLOGGING_LIBRARY} easyloggingpp )
