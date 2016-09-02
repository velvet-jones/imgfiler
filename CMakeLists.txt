cmake_minimum_required (VERSION 2.6)

project(imgfiler C)

# include our extra cmake utilities
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# version number of the app
set (APP_VERSION_MAJOR 1)
set (APP_VERSION_MINOR 2)
set (APP_VERSION_PATCH 0)

# create config.h
include (CheckIncludeFiles)
check_include_files (extractor.h HAVE_EXTRACTOR_H)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in ${CMAKE_BINARY_DIR}/config.h)

# cpack stuff for creating packages
set(CPACK_PACKAGE_VERSION ${VERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR "${APP_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${APP_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${APP_VERSION_PATCH}")
set(CPACK_GENERATOR "TGZ;RPM;DEB")
set(CPACK_PACKAGE_NAME "imgfiler")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Bud Millwood")
set(CPACK_PACKAGE_VENDOR "")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
include(CPack)


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

# always install the man page, even if not rebuilding it
install(FILES doc/imgfiler.1 DESTINATION share/man/man1)
