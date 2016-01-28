cmake_minimum_required (VERSION 2.6)

project(imgfiler C)

# include our extra cmake utilities
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# version number of the app
set (APP_VERSION_MAJOR 1)
set (APP_VERSION_MINOR 0)

# create config.h
include (CheckIncludeFiles)
check_include_files (extractor.h HAVE_EXTRACTOR_H)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in ${CMAKE_BINARY_DIR}/config.h)

# specific settings we want for toolchains
if(CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g")
endif(CMAKE_COMPILER_IS_GNUCXX)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g")
endif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")

# locate packages we need or want
find_package (EXIF REQUIRED)
find_package (OpenSSL REQUIRED)
find_package (Extractor)
find_package (Asciidoc)

add_subdirectory (src)

# don't build the documentation if asciidoc isn't installed
if(ASCIIDOC_FOUND)
  add_subdirectory (doc)
endif(ASCIIDOC_FOUND)
