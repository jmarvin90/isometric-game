# -Weffc++ causes included libs to break
COMPILER_FLAGS =	-Werror \
					-Wall \
					-Wno-system-headers \
					-pedantic-errors
LANG_STD = 			-std=c++17
SRC_FILES = 		./src/*.cpp \
					./src/game/*.cpp
LINKER_FLAGS = 		-lSDL2 \
					-lSDL2_image \
					-lspdlog \
					-lfmt
INCLUDE_PATH = -isystem"./libs/"
OUTPUT = isometric-game

build:
	g++ \
		${COMPILER_FLAGS} \
		${LANG_STD}  \
		${SRC_FILES} \
		${LINKER_FLAGS} \
		${INCLUDE_PATH} \
		-o ${OUTPUT};

run:	
	./${OUTPUT}

clean:
	rm ${OUTPUT}