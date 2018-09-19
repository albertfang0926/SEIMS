# This module accepts the following environment variables:
#
#   BSON_DIR, BSON_ROOT, or BSON_ROOT_DIR - Specify the location of BSON
#
# Read-Only variables:
#  BSON_FOUND - system has the BSON library
#  BSON_INCLUDE_DIR - the BSON include directory
#  BSON_LIBRARIES - The libraries needed to use BSON
#  BSON_DLLS - The dynamic libraries needed to run BSON program on Windows
#  BSON_VERSION - This is set to $major.$minor.$revision$path (eg. 0.4.1)

if (UNIX)
    find_package(PkgConfig QUIET)
    pkg_check_modules(_BSON QUIET libbson-1.0)
endif ()

find_path(BSON_INCLUDE_DIR
        NAMES
        libbson-1.0/bson.h
        HINTS
        CMAKE_PREFIX_PATH
        ENV BSON_DIR
        ENV BSON_ROOT
        ENV BSON_ROOT_DIR
        ENV MONGOC_ROOT
        PATH_SUFFIXES
        include
        )
IF(NOT ${BSON_INCLUDE_DIR} MATCHES "libbson-1.0" AND NOT ${BSON_INCLUDE_DIR} MATCHES "BSON_INCLUDE_DIR-NOTFOUND")
  set(BSON_INCLUDE_DIR "${BSON_INCLUDE_DIR}/libbson-1.0")
ENDIF()

if (WIN32 AND NOT CYGWIN AND NOT MINGW)
    if (MSVC)
        find_library(BSON
                NAMES
                "bson-1.0"
                HINTS
                CMAKE_PREFIX_PATH
                ENV BSON_DIR
                ENV BSON_ROOT
                ENV BSON_ROOT_DIR
                ENV MONGOC_ROOT
                PATH_SUFFIXES
                bin
                lib
                )
        if (NOT ${BSON} MATCHES "BSON-NOTFOUND")
            mark_as_advanced(BSON)
            set(BSON_LIBRARIES ${BSON} ws2_32)
            find_file(BSON_DLL
                    NAMES
                    "libbson-1.0.dll"
                    HINTS
                    CMAKE_PREFIX_PATH
                    ENV BSON_DIR
                    ENV BSON_ROOT
                    ENV BSON_ROOT_DIR
                    ENV MONGOC_ROOT
                    PATH_SUFFIXES
                    bin
                    )
        endif ()
    else ()
        # bother supporting this?
    endif ()
else ()
    find_library(BSON_LIBRARY
            NAMES
            "bson-1.0"
            "libbson-1.0"
            HINTS
            CMAKE_PREFIX_PATH
            ENV BSON_DIR
            ENV BSON_ROOT
            ENV BSON_ROOT_DIR
            ENV MONGOC_ROOT
            PATH_SUFFIXES
            lib
            )
    if (NOT ${BSON_LIBRARY} MATCHES "BSON_LIBRARY-NOTFOUND")
        mark_as_advanced(BSON_LIBRARY)
        find_package(Threads REQUIRED)
        set(BSON_LIBRARIES ${BSON_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
    endif ()
endif ()

if (BSON_INCLUDE_DIR)
    if (_BSON_VERSION)
        set(BSON_VERSION "${_BSON_VERSION}")
    elseif (BSON_INCLUDE_DIR AND EXISTS "${BSON_INCLUDE_DIR}/bson-version.h")
        file(STRINGS "${BSON_INCLUDE_DIR}/bson-version.h" bson_version_str
                REGEX "^#define[\t ]+BSON_VERSION[\t ]+\([0-9.]+\)[\t ]+$")

        string(REGEX REPLACE "^.*BSON_VERSION[\t ]+\([0-9.]+\)[\t ]+$"
                "\\1" BSON_VERSION "${bson_version_str}")
    endif ()
endif ()

include(FindPackageHandleStandardArgs)

if (BSON_VERSION)
    find_package_handle_standard_args(BSON
            REQUIRED_VARS
            BSON_LIBRARIES
            BSON_INCLUDE_DIR
            VERSION_VAR
            BSON_VERSION
            FAIL_MESSAGE
            "Could NOT find BSON version"
            )
else ()
    find_package_handle_standard_args(BSON "Could NOT find BSON"
            BSON_LIBRARIES
            BSON_INCLUDE_DIR
            )
endif ()

mark_as_advanced(BSON_INCLUDE_DIR BSON_LIBRARIES)
