#ifndef _GAME_H_
#define _GAME_H_

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "defines.h"
#include "tower.h"
#include "enemy.h"
#include "singletons.h"
#include "structs.h"
#include "pathfinding.h"

using namespace std;

class Game
{
public:
    Game(const char *title, int xpos, int ypos, int width, int height, bool fullscreen);
    ~Game();
    void Play(void);
private:
    void InitTowers(void);
    void InitEnemies(void);
    void InitMap(void);
    void HandleEvents(void);
    void HandleMouseMotion(SDL_Event *event);
    void HandleMouseDown(SDL_Event *event);
    void HandleMouseUp(SDL_Event *event);
    void Render(void);
    void Update(void);
    void DisplayShop(void);
    void DisplayScore(void);
    void DisplayLevel(void);
    void DisplayMoney(void);
    void DisplayLives(void);
    void DisplayMovingTower(void);
    void DisplayTowers(void);
    void DisplayEnemies(void);
    void DisplayProjectiles(void);
    void DisplayBackground(void) {
        SDL_RenderCopy(GUIManager::getInstance()->GetRenderer(), TextureManager::getInstance()->GetBackground(), NULL, NULL);
    }
    void NextLevel(void) { level++; }
    bool InsideRect(int pointX, int pointY, SDL_Rect *rect);
    bool InTowerRange(Enemy *enemy, Tower &tower);

// data section
private:
    // map
    SDL_Rect mapDestR;

    // towers
    Tower towerMap[BOARD_SIZE_X/RECT_W][BOARD_SIZE_Y/RECT_H];
    Tower towerShop[NUM_TOWERS];
    bool insideTowerShop[NUM_TOWERS];
    bool insideTower[BOARD_SIZE_X/RECT_W][BOARD_SIZE_Y/RECT_H];
    int mouseOverTower; // FIXME: rename

    SDL_Texture *towerTempTexture;
    SDL_Rect towerTempDestR; // temporary tower when placing
    TD::TowerType currentTower; 
    
    // Enemies
    vector<Enemy*> enemies; // enemies[NUM_ENEMIES] max technically

    // Projectile
    SDL_FRect  projectileDstR[BOARD_SIZE_X/RECT_W][BOARD_SIZE_Y/RECT_H];
    SDL_FPoint projectileStartPoint[BOARD_SIZE_X/RECT_W][BOARD_SIZE_Y/RECT_H];
    SDL_FPoint projectileEndPoint[BOARD_SIZE_X/RECT_W][BOARD_SIZE_Y/RECT_H];
    // gamestate
    int mouseX, mouseY;
    bool running; // when game loop is executing
    bool paused;  // when enemies are not advancing
    Uint32 level;
    Uint32 lives; 
    Uint32 money;
    Uint32 score;
    Uint64 frames; // TODO: rename to currentFrame
};

#endif // _GAME_H_