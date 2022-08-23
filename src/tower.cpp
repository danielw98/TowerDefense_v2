#include "tower.h"
#include "defines.h"
#include <iostream>
using namespace std;
// find a workaround the global variables without saving them in each tower - probably static variables of class
Uint32 towerCosts[NUM_TOWERS] = {50, 100, 250, 500, 1200};
Uint32 towerDamagePerHit[NUM_TOWERS] = {10, 20, 50, 100, 240};
Uint32 towerRadius[NUM_TOWERS] = {180, 250, 290, 320, 350};

Tower::Tower()
{
}
Tower::~Tower()
{
}
Uint32 Tower::GetCost(TD::TowerType type)
{
    return towerCosts[type-1];
}
Uint32 Tower::GetDamage(TD::TowerType type)
{
    return towerDamagePerHit[type-1];
}
Uint32 Tower::GetRadius(TD::TowerType type)
{
    return towerRadius[type-1];
}

// texture and dstrect set in superclass
Tower::Tower(SDL_Texture *texture, TD::TowerType type, SDL_Rect dstRect)
{
    this->texture = texture;
    this->dstRect = dstRect;
    this->type = type;
    this->cost = towerCosts[this->type-1];
    this->damage = towerDamagePerHit[this->type-1];
    this->radius = towerRadius[this->type-1];
}
