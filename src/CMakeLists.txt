set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set (TARGET_NAME imgfiler)

file (GLOB folder_SOURCES *.c)
file (GLOB folder_HEADERS *.h)

find_package (EXIF REQUIRED)
find_package (OpenSSL REQUIRED)

add_executable(${TARGET_NAME} ${folder_SOURCES} ${folder_HEADERS})

target_link_libraries (${TARGET_NAME} ${EXIF_LIBRARY} ${OPENSSL_LIBRARIES})