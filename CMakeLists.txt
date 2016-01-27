cmake_minimum_required (VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# g++ toolchain, all platforms
if(CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g")
endif(CMAKE_COMPILER_IS_GNUCXX)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g")
endif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")

add_subdirectory (src)
