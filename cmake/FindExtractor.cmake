# This module finds libextractor libraries.
#
# It sets the following variables:
#  EXTRACTOR_FOUND       - Set to false, or undefined, if extractor libraries aren't found.
#  EXTRACTOR_INCLUDE_DIR - The extractor include directory.
#  EXTRACTOR_LIBRARY     - The extractor library to link against.

INCLUDE(CheckFunctionExists)

FIND_PATH(EXTRACTOR_INCLUDE_DIR NAMES extractor.h)
FIND_LIBRARY(EXTRACTOR_LIBRARY NAMES extractor)

IF (EXTRACTOR_LIBRARY)
   SET(EXTRACTOR_FOUND TRUE)
ENDIF (EXTRACTOR_LIBRARY)

IF (EXTRACTOR_FOUND)
   IF (NOT EXTRACTOR_FIND_QUIETLY)
      MESSAGE(STATUS "Found extractor: ${EXTRACTOR_LIBRARY}")
   ENDIF (NOT EXTRACTOR_FIND_QUIETLY)

ELSE (EXTRACTOR_FOUND)
   # fatal error if mp4v2 is required but not found
   IF (EXTRACTOR_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find extractor")
   ENDIF (EXTRACTOR_FIND_REQUIRED)
ENDIF (EXTRACTOR_FOUND)
