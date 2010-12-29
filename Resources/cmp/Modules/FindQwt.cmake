
# - Find QWT library
# - Derived from the FindQwt.cmake that is included with cmake
# Find the native QWT includes and library
# This module defines
#  QWT_INCLUDE_DIR, where to find qwt.h, etc.
#  QWT_LIBRARIES, libraries to link against to use QWT.
#  QWT_FOUND, If false, do not try to use QWT.
# also defined, but not for general use are
#  QWT_LIBRARY, where to find the QWT library.
#  QWT_LIBRARY_DEBUG - Debug version of qwt library
#  QWT_LIBRARY_RELEASE - Release Version of qwt library


set (QWT_CMAKE_DEBUG 0)
if (QWT_CMAKE_DEBUG)
    MESSAGE (STATUS "Finding Qwt library and headers..." )
endif()


# Only set QWT_INSTALL to the environment variable if it is blank
if ("${QWT_INSTALL}" STREQUAL "")
    SET (QWT_INSTALL  $ENV{QWT_INSTALL})
endif()

# Look for the header file.
SET(QWT_INCLUDE_SEARCH_DIRS
  ${QWT_INSTALL}/include
)

SET (QWT_LIB_SEARCH_DIRS
  ${QWT_INSTALL}/lib
  )

SET (QWT_BIN_SEARCH_DIRS
  ${QWT_INSTALL}/bin
)

FIND_PATH(QWT_INCLUDE_DIR 
  NAMES qwt.h
  PATHS ${QWT_INCLUDE_SEARCH_DIRS} 
  NO_DEFAULT_PATH
)

IF (WIN32 AND NOT MINGW)
    SET (QWT_SEARCH_DEBUG_NAMES "qwt_debug;libqwt_debug")
    SET (QWT_SEARCH_RELEASE_NAMES "qwt;libqwt")
ELSE (WIN32 AND NOT MINGW)
    SET (QWT_SEARCH_DEBUG_NAMES "qwt_debug")
    SET (QWT_SEARCH_RELEASE_NAMES "qwt")
ENDIF(WIN32 AND NOT MINGW)

# Look for the library.
FIND_LIBRARY(QWT_LIBRARY_DEBUG 
  NAMES ${QWT_SEARCH_DEBUG_NAMES}
  PATHS ${QWT_LIB_SEARCH_DIRS} 
  NO_DEFAULT_PATH
  )
  
FIND_LIBRARY(QWT_LIBRARY_RELEASE 
  NAMES ${QWT_SEARCH_RELEASE_NAMES}
  PATHS ${QWT_LIB_SEARCH_DIRS} 
  NO_DEFAULT_PATH
  )

if ( QWT_CMAKE_DEBUG )
message(STATUS "QWT_INCLUDE_SEARCH_DIRS: ${QWT_INCLUDE_SEARCH_DIRS}")
message(STATUS "QWT_LIB_SEARCH_DIRS: ${QWT_LIB_SEARCH_DIRS}")
message(STATUS "QWT_BIN_SEARCH_DIRS: ${QWT_BIN_SEARCH_DIRS}")
message(STATUS "QWT_SEARCH_DEBUG_NAMES: ${QWT_SEARCH_DEBUG_NAMES}")
message(STATUS "QWT_SEARCH_RELEASE_NAMES: ${QWT_SEARCH_RELEASE_NAMES}")

MESSAGE(STATUS "QWT_INCLUDE_DIR: ${QWT_INCLUDE_DIR}")
MESSAGE(STATUS "QWT_LIBRARY_DEBUG: ${QWT_LIBRARY_DEBUG}")
MESSAGE(STATUS "QWT_LIBRARY_RELEASE: ${QWT_LIBRARY_RELEASE}")
MESSAGE(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

# include the macro to adjust libraries
INCLUDE (${CMP_MODULES_SOURCE_DIR}/cmpAdjustLibVars.cmake)
cmp_ADJUST_LIB_VARS(QWT)

IF(QWT_INCLUDE_DIR AND QWT_LIBRARY)
  SET(QWT_FOUND 1)
  SET(QWT_LIBRARIES ${QWT_LIBRARY})
  SET(QWT_INCLUDE_DIRS ${QWT_INCLUDE_DIR})
  IF (QWT_LIBRARY_DEBUG)
    GET_FILENAME_COMPONENT(QWT_LIBRARY_PATH ${QWT_LIBRARY_DEBUG} PATH)
    SET(QWT_LIB_DIR  ${QWT_LIBRARY_PATH})
  ELSEIF(QWT_LIBRARY_RELEASE)
    GET_FILENAME_COMPONENT(QWT_LIBRARY_PATH ${QWT_LIBRARY_RELEASE} PATH)
    SET(QWT_LIB_DIR  ${QWT_LIBRARY_PATH})
  ENDIF(QWT_LIBRARY_DEBUG)
  
  IF (QWT_DUMP_PROG)
    GET_FILENAME_COMPONENT(QWT_BIN_PATH ${QWT_DUMP_PROG} PATH)
    SET(QWT_BIN_DIR  ${QWT_BIN_PATH})
  ENDIF (QWT_DUMP_PROG)
ELSE(QWT_INCLUDE_DIR AND QWT_LIBRARY)
  SET(QWT_FOUND 0)
  SET(QWT_LIBRARIES)
  SET(QWT_INCLUDE_DIRS)
ENDIF(QWT_INCLUDE_DIR AND QWT_LIBRARY)

# Report the results.
IF(NOT QWT_FOUND)
  SET(QWT_DIR_MESSAGE
    "Qwt was not found. Make sure QWT_LIBRARY and QWT_INCLUDE_DIR are set or set the QWT_INSTALL environment variable.")
  IF(NOT QWT_FIND_QUIETLY)
    MESSAGE(STATUS "${QWT_DIR_MESSAGE}")
  ELSE(NOT QWT_FIND_QUIETLY)
    IF(QWT_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Qwt was NOT found and is Required by this project")
    ENDIF(QWT_FIND_REQUIRED)
  ENDIF(NOT QWT_FIND_QUIETLY)
ENDIF(NOT QWT_FOUND)

IF (QWT_FOUND)
  INCLUDE(CheckSymbolExists)
  #############################################
  # Find out if QWT was build using dll's
  #############################################
  # Save required variable
  SET(CMAKE_REQUIRED_INCLUDES_SAVE ${CMAKE_REQUIRED_INCLUDES})
  SET(CMAKE_REQUIRED_FLAGS_SAVE    ${CMAKE_REQUIRED_FLAGS})
  # Add QWT_INCLUDE_DIR to CMAKE_REQUIRED_INCLUDES
  SET(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES};${QWT_INCLUDE_DIRS}")

  CHECK_SYMBOL_EXISTS(QWT_BUILT_AS_DYNAMIC_LIB "QWTTypes.h" HAVE_QWT_DLL)

  IF (HAVE_QWT_DLL STREQUAL "1")
    SET (QWT_IS_SHARED 1 CACHE INTERNAL "Qwt Built as DLL or Shared Library")
  ENDIF (HAVE_QWT_DLL STREQUAL "1")

  # Restore CMAKE_REQUIRED_INCLUDES and CMAKE_REQUIRED_FLAGS variables
  SET(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVE})
  SET(CMAKE_REQUIRED_FLAGS    ${CMAKE_REQUIRED_FLAGS_SAVE})
  #
  #############################################
ENDIF ()
