set(CMAKE_INCLUDE_CURRENT_DIR ON)

install(FILES imgfiler.1 DESTINATION share/man/man1)

add_custom_command(
    OUTPUT imgfiler.1
    DEPENDS imgfiler.1.txt
#  asciidoc prints a warning about destination dir, but the default is to put it in the source dir anyway
#    COMMAND a2x --doctype manpage --format manpage -D ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgfiler.1.txt
    COMMAND a2x --doctype manpage --format manpage ${CMAKE_CURRENT_SOURCE_DIR}/imgfiler.1.txt
    COMMENT "Asciidoc man page")

add_custom_command(
    OUTPUT imgfiler.1.html
    DEPENDS imgfiler.1.txt
#  asciidoc prints a warning about destination dir, but the default is to put it in the source dir anyway
#    COMMAND a2x --doctype manpage --format manpage -D ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgfiler.1.txt
    COMMAND asciidoc ${CMAKE_CURRENT_SOURCE_DIR}/imgfiler.1.txt
    COMMENT "Asciidoc html page")

add_custom_target(man DEPENDS imgfiler.1)
add_custom_target(html DEPENDS imgfiler.1.html)
