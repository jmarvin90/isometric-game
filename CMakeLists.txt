cmake_minimum_required(VERSION 3.21)
project(IsometricGame LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# ---------------------------------------------------------------------------
# Dependencies
#
# Fetched as SYSTEM so their headers don't trigger our strict warning flags
# below. SYSTEM on FetchContent_Declare requires CMake >= 3.25 — if you're on
# an older CMake, drop SYSTEM here and instead mark each target's include
# dirs as SYSTEM further down (see comment there).
# ---------------------------------------------------------------------------

set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  entt
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        v3.13.2
  GIT_SHALLOW    TRUE
  SYSTEM
)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.3
  GIT_SHALLOW    TRUE
  SYSTEM
)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.14.1
  GIT_SHALLOW    TRUE
  SYSTEM
)

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        release-2.30.6
  GIT_SHALLOW    TRUE
  SYSTEM
)

FetchContent_Declare(
  SDL_image
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
  GIT_TAG        release-2.8.2
  GIT_SHALLOW    TRUE
  SYSTEM
)

# ImGui has no CMakeLists.txt of its own — fetched as raw source and built
# as a static lib target below.
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.90.9
  GIT_SHALLOW    TRUE
  SYSTEM
)

# SDL2 pulls in its own options; keep it quiet and static for a simple build.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)

set(SDL2IMAGE_VENDORED ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(entt json spdlog SDL2 SDL_image imgui)

# ---------------------------------------------------------------------------
# ImGui target
#
# Backend assumed: SDL2 + SDL_Renderer. If you're rendering with OpenGL or
# Vulkan instead, swap imgui_impl_sdlrenderer2.cpp for the matching backend
# file(s) (e.g. imgui_impl_opengl3.cpp) and link the relevant graphics libs.
# ---------------------------------------------------------------------------

add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

target_include_directories(imgui SYSTEM PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC SDL2::SDL2-static)

# ---------------------------------------------------------------------------
# Engine sources
# ---------------------------------------------------------------------------

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/engine/*.cpp
)

if(NOT ENGINE_SOURCES)
  message(FATAL_ERROR "No .cpp files found under src/engine/ — check the path.")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(IsometricGame
  ${CMAKE_SOURCE_DIR}/src/main.cpp
  ${ENGINE_SOURCES}
)

target_include_directories(IsometricGame PRIVATE
  ${CMAKE_SOURCE_DIR}/src/engine
)

# libs/ holds any external headers you've vendored directly rather than
# fetched (per your repo layout). Marked SYSTEM so they're exempt from
# -Werror too.
if(EXISTS ${CMAKE_SOURCE_DIR}/libs)
  target_include_directories(IsometricGame SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/libs
  )
endif()

target_link_libraries(IsometricGame PRIVATE
  EnTT::EnTT
  nlohmann_json::nlohmann_json
  spdlog::spdlog
  SDL2::SDL2-static
  SDL2::SDL2main
  SDL2_image
  imgui
)

# ---------------------------------------------------------------------------
# Strict warnings — applied ONLY to our own target, not dependencies above.
# -Wsystem-headers is deliberately NOT added, so warnings in anything marked
# SYSTEM (all the FetchContent deps, and libs/ if present) stay suppressed.
# ---------------------------------------------------------------------------

target_compile_options(IsometricGame PRIVATE
  -Wall
  -Wextra
  -Werror
  -pedantic-errors
)