cmake_minimum_required(VERSION 3.10)

project(rerogue VERSION 0.1)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_compile_options(-Wno-unknown-warning-option)

add_executable(rerogue src/main.cc src/init.cc src/events.cc src/update.cc src/draw.cc src/gfx/gfx.cc src/input/input.cc src/mixer/mixer.cc src/game/game.cc src/tilemap/tilemap.cc src/_core/fs.cc src/lua/lua.cc)

target_link_libraries(rerogue PRIVATE SDL2)
target_link_libraries(rerogue PRIVATE SDL2_image)
target_link_libraries(rerogue PRIVATE SDL2_mixer)
target_link_libraries(rerogue PRIVATE SDL2_ttf)
target_link_libraries(rerogue PRIVATE lua5.4)
