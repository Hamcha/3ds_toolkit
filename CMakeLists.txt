# Global project settings
cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(3ds_toolkit)
set(MAJOR "0")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${3ds_toolkit_SOURCE_DIR}/cmake")

#
# Libraries
#

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)

#
# Common code
#

include_directories(common)

file(GLOB COMMON_CODE common/*.h common/formats/*.h)
source_group("Headers" FILES ${COMMON_CODE})

#
# Projects and tools
#

# CGFX Viewer
file(GLOB CGFXVIEWER_SRC cgfxview/*.cpp)
add_executable(cgfxview ${CGFXVIEWER_SRC})

set_target_properties(cgfxview PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(cgfxview PRIVATE cxx_range_for)

target_include_directories(cgfxview PUBLIC ${OPENGL_INCLUDE_DIRS})
target_link_libraries(cgfxview ${OPENGL_LIBRARIES})

target_include_directories(cgfxview PUBLIC ${GLEW_INCLUDE_DIRS})
target_link_libraries(cgfxview ${GLEW_LIBRARIES})

target_include_directories(cgfxview PUBLIC ${SDL2_INCLUDE_DIR})
target_link_libraries(cgfxview ${SDL2_LIBRARY})

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
