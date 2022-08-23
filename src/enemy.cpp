#include "enemy.h"

Enemy::Enemy(int health, int gold, int score)
{
    cout << "Enemy()\n";
    
    this->health = health;
    this->currentHealth = health;
    this->gold = gold;
    this->score = score;
    this->speed = 1;
    this->numcalls = 0;
    this->current_piece = 0;
    this->visible = false;
    this->destroy = false;
    dstRect.x = BOARD_OFFSET_X - RECT_W;
    dstRect.y = ENEMY_OFFSET + BOARD_OFFSET_Y;
    dstRect.w = RECT_W;
    dstRect.h = RECT_H;

    totalHealthRect.x = BOARD_OFFSET_X - RECT_W;
    totalHealthRect.y = ENEMY_OFFSET + BOARD_OFFSET_Y;
    totalHealthRect.w = RECT_W;
    totalHealthRect.h = 5;

    currentHealthRect.x = BOARD_OFFSET_X - RECT_W;
    currentHealthRect.y = ENEMY_OFFSET + BOARD_OFFSET_Y;
    currentHealthRect.w = RECT_W;
    currentHealthRect.h = 5;
    
    SetTexture(TextureManager::getInstance()->GetEnemy());
}

Enemy::~Enemy()
{
    cout << "~Enemy()\n";
}

void Enemy::DecreaseHealth(Uint32 damage)
{
    currentHealth -= damage;
    if(currentHealth < 0)
    {
        printf("should destroy\n");
        this->destroy = true;
    }

    currentHealthRect.w = ((float) currentHealth / (float) health) * totalHealthRect.w;
}
void Enemy::Update(void)
{
    // static bool isEnabled[NUM_ENEMIES] = {0};
    // the problem is i move enemy according to numcalls,
    // and later enemies enter the frame later. take that into consideration.
    // after 64px it will change position - should start changing when rectangle enters
    // isEnabled[0] = true; // isEnabled true means it can start moving. some rects should
    // start moving later
    // probably need variables for when level starts and ends, so i know when to render/free sprites
    if(!visible)
        return;
    numcalls++;
    switch (GetCurrentDirection())
    {
        case TD::NORTH: {
            dstRect.y -= speed;
            currentHealthRect.y -= speed;
            totalHealthRect.y -= speed;
            break;
        }
        case TD::SOUTH: {
            dstRect.y += speed;
            currentHealthRect.y += speed;
            totalHealthRect.y += speed;
            break;
        }
        case TD::EAST: {
            dstRect.x += speed;
            currentHealthRect.x += speed;
            totalHealthRect.x += speed;
            break;
        }
        case TD::WEST: {
            dstRect.x -= speed;
            currentHealthRect.x -= speed;
            totalHealthRect.x -= speed;
            break;
        }
    }

    if (numcalls % RECT_W == 0 && current_piece < path_length)
        current_piece++;
}

void Enemy::Render(void)
{
    if(!visible)
        return;
    Entity::Render();
    SDL_SetRenderDrawColor(GUIManager::getInstance()->GetRenderer(), 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(GUIManager::getInstance()->GetRenderer(), &totalHealthRect);
    SDL_SetRenderDrawColor(GUIManager::getInstance()->GetRenderer(), 0, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(GUIManager::getInstance()->GetRenderer(), &currentHealthRect);
}