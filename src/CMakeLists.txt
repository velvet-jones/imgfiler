set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set (TARGET_NAME imgfiler)

file (GLOB folder_SOURCES *.c)
file (GLOB folder_HEADERS *.h)

add_executable(${TARGET_NAME} ${folder_SOURCES} ${folder_HEADERS})
target_link_libraries (${TARGET_NAME} ${EXIF_LIBRARY} ${EXTRACTOR_LIBRARY} ${OPENSSL_LIBRARIES})
include_directories(${CMAKE_BINARY_DIR})

install(TARGETS ${TARGET_NAME} DESTINATION bin)
