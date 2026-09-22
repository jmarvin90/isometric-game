# isometric-game

The purpose of the code in this repository is to demonstrate implementations for
some of the logic necessary to implement a 2.5D (isometric) game world.

Much of the logic is an elaboration on the patterns introduced in the 
[2D Game Engine with C++](https://pikuma.com/courses/cpp-2d-game-engine-development) 
course on [Pikuma.com](pikuma.com).

My objective was to become proficient in C++ development to the extent that I could 
develop the foundations of 2D game.

## Overview

![Game demo](output.gif)

The implementation comprises:
 - Isometric projection of 2D to 2.5D cooardinates, per [this video](https://youtu.be/04oQ2jOUjkU?si=yIhHjz9SubxI9CQB) by Jordan West
 - A spatial partition, informed by the [Game Programming Patterns](https://gameprogrammingpatterns.com/spatial-partition.html) book by Robert Nystrom
 - A pathfinding algorithm, informed by a [Red Blob Games](https://www.redblobgames.com/pathfinding/a-star/introduction.html) article on the subject

Additional logic includes:
 - Game loop fundamentals, including timestep and sprite ordering
 - Game data serialisation, supported by the chosen ECS library (EnTT)

## Getting Started

The game can be build using CMake, by issuing the following commands from the
root of the repository (Linux, Mac):

```bash
mkdir build/ && cd build
cmake ..
cmake --build .
```

Running the game from the root directory of the repo:

```bash
./IsometricGame
```

The game starts with the camera oriented in the top-left of the game world.

You can use the mouse to pan around the world by moving the cursor toward the
edge of the screen.

Pressing `d` brings up the Debug UI, which enables you to select tiles on the map
and emplace tiles on the screen.

Placing a pair of tall and short buildings will spawn a walker which, in case
a viable path can be found, will navigate between the two.

## Technologies/Libraries

 - [EnTT](https://github.com/skypjack/entt) - the entity, component, system framework
 - [ImGUI](https://github.com/ocornut/imgui) - debug user interface
 - [GLM](https://github.com/g-truc/glm) - mathematics
 - [nlohmann::json](https://github.com/nlohmann/json) - game data


