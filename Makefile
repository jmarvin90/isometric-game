# -Weffc++ causes included libs to break
COMPILER_FLAGS =		-Werror \
						-Wall \
						-Wno-system-headers \
						-pedantic-errors
LANG_STD = 				-std=c++17
SRC_FILES = 			./src/*.cpp \
						./src/engine/*.cpp
LINKER_FLAGS = 			-lSDL2 \
						-lSDL2_image \
						-lspdlog \
						-lfmt
INCLUDE_PATH = 			-isystem"./libs/"
ADDITIONAL_INCLUDES =	-I"./src/engine/components/" -I"./src/engine/"
OUTPUT = isometric-game

build:
	g++ \
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