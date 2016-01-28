# - Find Asciidoc
# this module looks for asciidoc
#
# ASCIIDOC_EXECUTABLE - the full path to asciidoc
# ASCIIDOC_FOUND - If false, don't attempt to use asciidoc.

FIND_PROGRAM(ASCIIDOC_EXECUTABLE
asciidoc
$ENV{PROGRAMFILES}/asciidoc
)

MARK_AS_ADVANCED(
ASCIIDOC_EXECUTABLE
)

if(WIN32)
  find_path(ASCIIDOC_DOCBOOK_XSL_DIR htmlhelp.xsl
            PATHS
            $ENV{PROGRAMFILES}/Asciidoc/docbook-xsl)
else (WIN32)
  find_path(ASCIIDOC_DOCBOOK_XSL_DIR htmlhelp.xsl
            PATHS
            /usr/share/asciidoc/docbook-xsl)
endif (WIN32)

IF (NOT ASCIIDOC_EXECUTABLE)
SET(ASCIIDOC_FOUND "NO")
ELSE (NOT ASCIIDOC_EXECUTABLE)
SET(ASCIIDOC_FOUND "YES")
ENDIF (NOT ASCIIDOC_EXECUTABLE)

IF (NOT ASCIIDOC_FOUND AND Asciidoc_FIND_REQUIRED)
MESSAGE(FATAL_ERROR "Could not find asciidoc")
ENDIF (NOT ASCIIDOC_FOUND AND Asciidoc_FIND_REQUIRED)