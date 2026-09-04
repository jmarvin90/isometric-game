# -Weffc++ causes included libs to break
COMPILER_FLAGS =		-Wno-system-headers \
						-pedantic-errors \
						-O2
DEBUG_FLAG = 			-ggdb
LANG_STD = 				-std=c++17
SRC_FILES = 			./src/*.cpp \
						./src/engine/*.cpp \
						./src/engine/systems/*.cpp \
						./src/engine/components/*.cpp \
						./libs/imgui/*.cpp \
						./libs/imgui/backends/imgui_impl_sdl2.cpp \
						./libs/imgui/backends/imgui_impl_sdlrenderer2.cpp
LINKER_FLAGS = 			-L/opt/homebrew/lib \
						-lSDL2 \
						-lSDL2_image \
						-lspdlog \
						-lfmt
INCLUDE_PATH = 			-isystem"./libs/imgui" \
						-isystem"./libs/entt/src/" \
						-isystem"/usr/include/SDL2" 
# 						-isystem"/opt/homebrew/include/" \
# 						-isystem"/opt/homebrew/include/SDL2/"
PROFILING_FLAG = 		-pg

ADDITIONAL_INCLUDES =	-I"./src/engine/components/" -I"./src/engine/"
OUTPUT = isometric-game

build:
	g++ \
		${DEBUG_FLAG} \
		${LANG_STD} \
		${COMPILER_FLAGS} \
		${PROFILING_FLAG} \
		${SRC_FILES} \
		${LINKER_FLAGS} \
		${PROFILING_FLAG} \
		${INCLUDE_PATH} \
		${ADDITIONAL_INCLUDES} \
		-o ${OUTPUT}

run:	
	./${OUTPUT}

clean:
	rm ${OUTPUT}