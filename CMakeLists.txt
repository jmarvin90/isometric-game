cmake_minimum_required(VERSION 3.10)

project(isometric-game LANGUAGES CXX)

# set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
# set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
# set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

link_libraries(SDL2 SDL2_image spdlog fmt)

include_directories(
    SYSTEM libs/entt/src/ 
    SYSTEM libs/imgui 
    SYSTEM /usr/include/SDL2 
    src/engine
    src/engine/components
    src/engine/sytems
)

add_executable(
    isometric-game 
    src/main.cpp 
    src/engine/game.cpp
    src/engine/scene.cpp
    src/engine/tilemap.cpp
    src/engine/systems/render.cpp
    libs/imgui/imgui.cpp
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/backends/imgui_impl_sdl2.cpp
    libs/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

target_compile_options(
    isometric-game
    PRIVATE -Wall 
    PRIVATE -Werror 
    PRIVATE -Wno-system-headers 
    PRIVATE -pedantic-errors
)

set_property(TARGET isometric-game PROPERTY CXX_STANDARD 17)