project(libcgfx)

#
# Source files and linking
#

file(GLOB_RECURSE LIBCGFX_SRC source/*.cpp)
file(GLOB_RECURSE LIBCGFX_HEADERS include/*.h)

set(LIBCGFX_INCLUDE_DIR include)

add_library(libcgfx STATIC ${LIBCGFX_SRC} ${LIBCGFX_HEADERS})

target_include_directories(libcgfx PUBLIC ${LIBCGFX_INCLUDE_DIR})

set_target_properties(libcgfx PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(libcgfx PRIVATE cxx_range_for)