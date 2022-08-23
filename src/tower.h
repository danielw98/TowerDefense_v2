#ifndef _TOWER_H_
#define _TOWER_H_
#include "entity.h"
#include "structs.h"
class Tower : public Entity
{
public:
    Tower();
    Tower(SDL_Texture *texture, TD::TowerType type, SDL_Rect dstRect);
    Uint32 GetCost(void)             { return cost; }
    Uint32 GetRadius(void)           { return radius; }
    Uint32 GetDamage(void)           { return damage; }
    TD::TowerType GetType(void)      { return type; }
    void SetCost(Uint32 cost)        { this->cost = cost; }
    void SetDamage(Uint32 damage)    { this->damage = damage; }
    void SetRadius(Uint32 radius)    { this->radius = radius; }
    void SetType(TD::TowerType type) { this->type = type; }
    static Uint32 GetCost(TD::TowerType type);
    static Uint32 GetDamage(TD::TowerType type);
    static Uint32 GetRadius(TD::TowerType type);
    ~Tower();
private:
    TD::TowerType type;
    Uint32 cost;
    Uint32 damage;
    Uint32 radius;
};
#endif // _TOWER_H_