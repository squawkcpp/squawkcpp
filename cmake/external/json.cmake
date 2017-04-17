#https://github.com/nlohmann/json
include(ExternalProject)

ExternalProject_Add(
  json_master
  URL "https://github.com/nlohmann/json/archive/v2.0.9.zip"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
)
ExternalProject_Get_Property(json_master source_dir)
set(JSON_INCLUDE_DIR ${source_dir}/src/)
set(JSON_LIBRARY json)
add_library(${JSON_LIBRARY} INTERFACE IMPORTED)
add_dependencies(${JSON_LIBRARY} json_master )

