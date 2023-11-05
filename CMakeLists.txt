cmake_minimum_required(VERSION 3.18)
project(null0
    DESCRIPTION "null0 game engine"
    HOMEPAGE_URL "https://github.com/konsumer/null0"
    VERSION 0.0.1
    LANGUAGES C
)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CFLAGS "${CMAKE_CFLAGS} -Wno-deprecated-opengl")

option(SDL "Build SDL" FALSE)
option(RAYLIB "Build raylib" TRUE)
option(LIBRETRO "Build libretro" FALSE)
option(CLI "Build CLI" FALSE)
option(WEB "Build Web" FALSE)

set(WAMR_SDK "${CMAKE_CURRENT_LIST_DIR}/wamr-sdk")
include_directories(${WAMR_SDK}/runtime-sdk/include)
link_directories(${WAMR_SDK}/runtime-sdk/lib)

set(SOURCES src/main.c)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/tools/build")
list(APPEND LIBRARIES vmlib)

find_package(pntr_app_starter)

add_subdirectory(example)

add_executable(host src/main.c)
target_link_libraries(host vmlib)
