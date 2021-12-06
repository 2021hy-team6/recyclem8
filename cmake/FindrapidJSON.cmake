set(rapidJSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/rapidjson/include/rapidjson)

find_package(PkgConfig)
pkg_check_modules(PC_rapidJSON QUIET rapidJSON)

find_path(rapidJSON_INCLUDE_DIR
    NAMES rapidjson.h
    PATHS ${PC_rapidJSON_INCLUDE_DIRS}
    PATH_SUFFIXES rapidjson
)

set(rapidJSON_VERSION ${PC_rapidJSON_VERSION})
mark_as_advanced(rapidJSON_FOUND rapidJSON_INCLUDE_DIR rapidJSON_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(rapidJSON
    REQUIRED_VARS rapidJSON_INCLUDE_DIR
    VERSION_VAR rapidJSON_VERSION
)

if(rapidJSON_FOUND AND NOT TARGET rapidJSON::rapidJSON)
    add_library(rapidJSON::rapidJSON INTERFACE IMPORTED)
    set_target_properties(rapidJSON::rapidJSON PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${rapidJSON_INCLUDE_DIRS}"
    )
endif()
