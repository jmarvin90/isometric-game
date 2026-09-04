#include "engine/game.h"

int main()
{
    Game game;

    game.initialise();
    game.run();
    game.destroy();
}