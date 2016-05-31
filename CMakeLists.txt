# Global project settings
cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(3ds_toolkit)
set(MAJOR "0")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${3ds_toolkit_SOURCE_DIR}/cmake")

#
# Project flags
#

set(BUILD_CGFXVIEW TRUE CACHE BOOL "Build cgfxview command line (requires some dependencies)")

#
# Projects and tools
#

add_subdirectory(libcgfx)

if(BUILD_CGFXVIEW)
	add_subdirectory(cgfxview)
endif()

#
# Utils / Extra
#

# Get git tag/hash for version string, if possible
include(FindGit OPTIONAL)
if(GIT_FOUND)
    execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND ${GIT_EXECUTABLE} describe --always --long --dirty --tags
        OUTPUT_VARIABLE CURRENTREV
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    add_definitions(-DCURRENTREV=\"${CURRENTREV}\")
endif()

# Enable as many warnings as possible
if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

# Remove console window on Windows
#if(MSVC)
#    set_target_properties(${PROJECT_NAME}_cmd PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
#    set_target_properties(${PROJECT_NAME}_cmd PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
#    set_target_properties(${PROJECT_NAME}_cmd PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
#    set_target_properties(${PROJECT_NAME}_cmd PROPERTIES RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
#    set_target_properties(${PROJECT_NAME}_cmd PROPERTIES MINSIZEREL "/SUBSYSTEM:WINDOWS")
#endif(MSVC)
