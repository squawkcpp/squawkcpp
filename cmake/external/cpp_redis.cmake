include(ExternalProject)
# - download and build cpp_redis
# Once done this will define
#  CPP_REDIS_INCLUDE_DIR - The cpp_redis include directory
#  CPP_REDIS_LIBRARIES - The libraries needed to use cpp_redis

ExternalProject_Add(
  cpp_redis_master
  #URL "https://github.com/Cylix/cpp_redis/archive/master.zip"
  GIT_REPOSITORY "https://github.com/Cylix/cpp_redis.git"
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  BUILD_BYPRODUCTS "cpp_redis_master-prefix/src/cpp_redis_master-build/libcpp_redis.a"
)
ExternalProject_Get_Property(cpp_redis_master source_dir)
set(CPP_REDIS_INCLUDE_DIR ${source_dir}/includes/)
ExternalProject_Get_Property(cpp_redis_master binary_dir)
set(CPP_REDIS_LIBRARIES ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}cpp_redis.a)
set(CPP_REDIS_LIBRARY cpp_redis)
add_library(${CPP_REDIS_LIBRARY} UNKNOWN IMPORTED)
set_property(TARGET ${CPP_REDIS_LIBRARY} PROPERTY IMPORTED_LOCATION ${CPP_REDIS_LIBRARIES} )
add_dependencies(${CPP_REDIS_LIBRARY} cpp_redis_master)
