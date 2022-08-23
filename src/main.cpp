#include <stdio.h>
#include "game.h"

int main()
{
    Game game = Game("Tower Defense", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 840, 900, false);
    game.Play();
    return 0;
}