#include <iostream>
#include "game/game.h"

int main() {
    Game game;
    game.initialise();
    game.run();
    game.destroy();
}