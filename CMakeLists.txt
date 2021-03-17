cmake_minimum_required(VERSION 2.8)

project(cosmeye)

# Must manually link to math library and to sdl2
# Note: Debug information currently enabled with -g flag
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -g -lm -lSDL2")

add_executable(${PROJECT_NAME}
    src/cosmeye.c
)

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARIES}
)
