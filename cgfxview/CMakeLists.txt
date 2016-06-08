project(cgfxview)

#
# Libraries
#

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)

#
# Source files and linking
#

file(GLOB CGFXVIEWER_SRC source/*.cpp)

add_executable(cgfxview ${CGFXVIEWER_SRC})

set_target_properties(cgfxview PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(cgfxview PRIVATE cxx_range_for)

target_include_directories(cgfxview PUBLIC ${OPENGL_INCLUDE_DIRS})
target_link_libraries(cgfxview ${OPENGL_LIBRARIES})

target_include_directories(cgfxview PUBLIC ${GLEW_INCLUDE_DIRS})
target_link_libraries(cgfxview ${GLEW_LIBRARIES})

target_include_directories(cgfxview PUBLIC ${SDL2_INCLUDE_DIR})
target_link_libraries(cgfxview ${SDL2_LIBRARY})

target_include_directories(cgfxview PUBLIC ${LIBCGFX_INCLUDE_DIR})
target_link_libraries(cgfxview libcgfx)