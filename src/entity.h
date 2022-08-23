#ifndef _ENTITY_H_
#define _ENTITY_H_
#include "singletons.h"
class Entity
{
public:
    Entity();
    ~Entity();
    void Render();
    void SetTexture(SDL_Texture *texture)
    {
        this->texture = texture;
    }
    SDL_Texture* GetTexture(void)
    {
        return texture;
    }
    SDL_Rect GetDestRect(void)
    {
        return dstRect;
    }
    void SetDestRect(SDL_Rect rect)
    {
        dstRect = rect;
    }
protected:
    SDL_Texture *texture;
    SDL_Rect srcRect, dstRect;
};
#endif // _ENTITY_H_