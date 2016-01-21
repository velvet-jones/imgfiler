cmake_minimum_required (VERSION 2.6)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set (TARGET_NAME imgfiler)

file (GLOB folder_SOURCES *.c)
file (GLOB folder_HEADERS *.h)

find_package (EXIF)

add_executable(${TARGET_NAME} ${folder_SOURCES} ${folder_HEADERS})

include_directories()

target_link_libraries (${TARGET_NAME} ${EXIF_LIBRARY})