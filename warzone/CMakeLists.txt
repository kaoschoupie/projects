cmake_minimum_required(VERSION 3.5.0)
project(project-1 VERSION 0.1.0 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 17)

# Enable warnings
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add all C++ files in `src`
file(GLOB_RECURSE src CONFIGURE_DEPENDS "src/*.cpp")
add_executable(project-1 ${src})
