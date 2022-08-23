#ifndef _ENEMY_H_
#define _ENEMY_H_
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "entity.h"
#include "singletons.h"
#include "structs.h"
#include "defines.h"
#include <iostream>
using namespace std;

class Enemy : public Entity
{
public:
    Enemy(int health, int gold, int score);
    ~Enemy();
    void Render(void);
    void Update(void);
    void DecreaseHealth(Uint32 health);
    void SetCurrentHealthRect(SDL_Rect &rect)         { currentHealthRect = rect; }
    void SetTotalHealthRect(SDL_Rect &rect)           { totalHealthRect = rect; }
    void SetPath(int i, Uint8 value)                  { path[i] = value; }
    void SetDirection(int i, TD::Direction direction) { directions[i] = direction; }
    void SetSpeed(Uint8 speed)                        { this->speed = speed; }
    void SetPathLength(int path_length)               { this->path_length = path_length; }
    void SetVisible(bool visible)                     { this->visible = visible; }
    bool IsVisible()                                  { return visible; }
    bool ShouldDestroy()                              { return destroy; }
    TD::Direction GetDirection(int i)                 { return directions[i]; }
    TD::Direction GetCurrentDirection(void)           { return directions[current_piece]; }
    TD::Direction* GetDirections(void)                { return directions; }
    int GetPathLength(void)                           { return path_length; }
    int GetGold(void)                                 { return gold; }
    int GetScore(void)                                { return score; }
    Uint8 GetPath(int i)                              { return path[i]; }
private:
    // stats
    int health;
    int currentHealth;
    int score;
    int gold;
    SDL_Rect totalHealthRect;
    SDL_Rect currentHealthRect;
    Uint8 speed;
    // pathfinding and position
    TD::Direction directions[NUM_VERTICES];
    Uint8 path[NUM_VERTICES];

    int path_length;
    int current_piece;
    int numcalls; // rename this, sounds non-intuitive
    bool visible;
    bool destroy;
};
#endif // _ENEMY_H_