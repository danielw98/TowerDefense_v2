#include "singletons.h"
#include <iostream>
using namespace std;

FontRenderer* FontRenderer::instance = NULL;
FontRenderer::FontRenderer()
{
    cout << "FontRenderer()\n";
    if(TTF_Init() == -1){
        fprintf(stderr,"Could not initialize TTF library: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    font = TTF_OpenFont("../Sans.ttf", 24);
    if(font == NULL){
        fprintf(stderr, "Loading font failed: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    } 
}
FontRenderer::~FontRenderer()
{
    cout << "~FontRenderer()\n";
    TTF_CloseFont(font);
    TTF_Quit();
    instance = NULL;
}

FontRenderer* FontRenderer::getInstance()
{
    if(!instance)
        instance = new FontRenderer;
    return instance;
}

void FontRenderer::DisplayText(const char *msg, int x, int y)
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Color color = {.r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE};
    SDL_Rect rect;

    surface = TTF_RenderText_Solid(font, msg, color);
    texture = SDL_CreateTextureFromSurface(GUIManager::getInstance()->GetRenderer(), surface);

    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;
    
    SDL_RenderCopy(GUIManager::getInstance()->GetRenderer(), texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


TextureManager* TextureManager::instance = NULL;
TextureManager::TextureManager()
{
    cout << "TextureManager()\n";
    background  = CreateTextureFromImage("../assets/background.png");
    enemy       = CreateTextureFromImage("../assets/enemy.png");
    towerRadius = CreateTextureFromImage("../assets/towerradius.png");
    projectile  = CreateTextureFromImage("../assets/fire.png");
    for(int i = 0; i < NUM_TOWERS; i++)
    {
        char path[255];
        sprintf(path, "../assets/tower%d.png", i+1);
        tower[i]         = CreateTextureFromImage(path);
        sprintf(path, "../assets/towerselected%d.png", i+1);
        selectedTower[i] = CreateTextureFromImage(path);
        sprintf(path, "../assets/towerred%d.png", i+1);
        redTower[i]      = CreateTextureFromImage(path);
    }
}
TextureManager::~TextureManager()
{
    cout << "~TextureManager()\n";
    for(int i = 0; i < NUM_TOWERS; i++)
    {
        SDL_DestroyTexture(tower[i]);
        SDL_DestroyTexture(redTower[i]);
        SDL_DestroyTexture(selectedTower[i]);
    }
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(enemy);
    SDL_DestroyTexture(towerRadius);
    instance = NULL;
}
TextureManager* TextureManager::getInstance()
{
    if(!instance)
        instance = new TextureManager;
    return instance;
}

SDL_Texture* TextureManager::CreateTextureFromImage(const char *path)
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    surface = IMG_Load(path);
    if(surface == NULL){
        fprintf(stderr, "File %s not found\n", path);
        exit(EXIT_FAILURE);
    }
    texture = SDL_CreateTextureFromSurface(GUIManager::getInstance()->GetRenderer(), surface);
    SDL_FreeSurface(surface);
    return texture;
}

GUIManager* GUIManager::instance = NULL;
GUIManager* GUIManager::getInstance()
{
    if(instance == NULL)
        instance = new GUIManager;
    return instance;
}

GUIManager::GUIManager()
{
    cout << "GUIManager()\n";
    window = NULL;
    renderer = NULL;
}

GUIManager::~GUIManager()
{
    cout << "~GUIManager()\n";
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    instance = NULL;
}