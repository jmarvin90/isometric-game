cmake_minimum_required(VERSION 3.10)
project(isometric-game LANGUAGES CXX)

# --- Common libraries ---
link_libraries(SDL2 SDL2_image spdlog fmt)

# --- Common include dirs ---
include_directories(
    SYSTEM libs/entt/src/
    SYSTEM libs/imgui
    SYSTEM /usr/include/SDL2
    SYSTEM libs/rapidjson/include/rapidjson
    src/engine
    src/engine/components
    src/engine/systems
)

# =====================================================
# 1. ImGui as its own library target
# =====================================================
add_library(imgui STATIC
    libs/imgui/imgui.cpp
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/backends/imgui_impl_sdl2.cpp
    libs/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

target_include_directories(imgui SYSTEM PUBLIC libs/imgui)
target_compile_options(imgui PRIVATE -w)  # disable all warnings for ImGui

# =====================================================
# 2. Main executable (your code)
# =====================================================
add_executable(isometric-game
    src/main.cpp
    src/engine/game.cpp
    src/engine/systems/render_system.cpp
    src/engine/systems/camera_system.cpp
    src/engine/systems/mouse_system.cpp
    src/engine/systems/tilemap_system.cpp
    src/engine/spritesheet.cpp
    src/engine/position.cpp
    src/engine/components/tilemap_component.cpp
    src/engine/systems/tilemap_system.cpp
)

target_compile_options(isometric-game PRIVATE
    # -Weffc++
    -Wextra
    -Wall
    -Werror
    -Wno-system-headers
    -pedantic-errors
)

target_link_libraries(isometric-game PRIVATE imgui)

set_property(TARGET isometric-game PROPERTY CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)