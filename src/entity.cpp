#include "entity.h"
#include "singletons.h"
#include <iostream>
using namespace std;

Entity::Entity()
{
}

Entity::~Entity()
{
}
void Entity::Render()
{
    SDL_RenderCopy(GUIManager::getInstance()->GetRenderer(), texture, NULL, &dstRect);
}