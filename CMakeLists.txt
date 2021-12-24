cmake_minimum_required(VERSION 3.16)
project(GFx VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GAME_EXECUTABLE GFx)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories("${CMAKE_SOURCE_DIR}/src")
file(GLOB_RECURSE GAME_SOURCE_FILES "src/*.cpp" "src/*.h")

add_executable(${GAME_EXECUTABLE} ${GAME_SOURCE_FILES})
target_precompile_headers(${GAME_EXECUTABLE} PRIVATE src/pch.h)
target_link_libraries(${GAME_EXECUTABLE} PRIVATE ${CONAN_LIBS})
