# -Weffc++ causes included libs to break
COMPILER_FLAGS =		-Werror \
						-Wall \
						-Wno-system-headers \
						-pedantic-errors
DEBUG_FLAG = 			-ggdb
LANG_STD = 				-std=c++17
SRC_FILES = 			./src/*.cpp \
						./src/engine/*.cpp \
						./libs/imgui/*.cpp \
						./libs/imgui/backends/imgui_impl_sdl2.cpp \
						./libs/imgui/backends/imgui_impl_sdlrenderer2.cpp
LINKER_FLAGS = 			-lSDL2 \
						-lSDL2_image \
						-lspdlog \
						-lfmt
INCLUDE_PATH = 			-isystem"./libs/entt/src/" -isystem"./libs/imgui" -isystem"/usr/include/SDL2/"
ADDITIONAL_INCLUDES =	-I"./src/engine/components/" -I"./src/engine/"
OUTPUT = isometric-game

build:
	g++ \
		${DEBUG_FLAG} \
		${COMPILER_FLAGS} \
		${LANG_STD}  \
		${SRC_FILES} \
		${LINKER_FLAGS} \
		${INCLUDE_PATH} \
		${ADDITIONAL_INCLUDES} \
		-o ${OUTPUT};

run:	
	./${OUTPUT}

clean:
	rm ${OUTPUT}