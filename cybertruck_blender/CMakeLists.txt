cmake_minimum_required(VERSION 3.5.0)
project(assignment-3 VERSION 0.1.0 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 17)

# Main executable
file(GLOB_RECURSE src CONFIGURE_DEPENDS "src/*.cpp")
add_executable(${PROJECT_NAME} ${src})

# Enable warnings
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Libraries
add_subdirectory(vendor/assimp EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glew-cmake EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glm EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME}
    PRIVATE assimp
    PRIVATE libglew_static
    PRIVATE glfw
    PRIVATE glm
)

