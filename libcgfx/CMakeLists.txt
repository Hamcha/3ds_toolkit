project(libcgfx)

#
# Source files and linking
#

file(GLOB LIBCGFX_SRC source/*.cpp source/formatters/*.cpp)

add_library(libcgfx STATIC ${LIBCGFX_SRC} ${LIBCGFX_HEADERS})

target_include_directories(libcgfx PUBLIC include)

set_target_properties(libcgfx PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(libcgfx PRIVATE cxx_range_for)