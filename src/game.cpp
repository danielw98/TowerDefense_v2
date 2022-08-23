#include "game.h"

Game::Game(const char *title, int xpos, int ypos, int width, int height, bool fullscreen)
{
    Uint32 flags = 0;
    if(fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;
    
    // init SDL API
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        running = false;
        return;
    }
    
    // init window and renderer
    SDL_Window *window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    GUIManager::getInstance()->SetWindow(window);
    GUIManager::getInstance()->SetRenderer(renderer);

    // init stats
    frames = 0;
    running = true;
    paused = true;
    level = 1;
    lives = 20;
    money = 10000;
    score = 0;

    // init other classes
    Pathfinder::ZeroMatrix();
    InitTowers();
    InitEnemies();
    InitMap();
}

void Game::InitMap(void)
{
    int xPos = BOARD_OFFSET_X;
    int yPos = BOARD_OFFSET_Y;
    for(int i = 0; i < BOARD_SIZE_X/RECT_W; i++)
    {
        for(int j = 0; j < BOARD_SIZE_Y/RECT_H; j++)
        {
            SDL_Rect towerRect;
            towerRect.w = RECT_W;
            towerRect.h = RECT_H;
            towerRect.x = xPos + i*RECT_W;
            towerRect.y = yPos + j*RECT_H;
            towerMap[i][j] = Tower(NULL, TD::TOWER_NONE, towerRect);
            insideTower[i][j] = false;
            projectileDstR[i][j].w = 16;
            projectileDstR[i][j].h = 16;
        }
    }
    mapDestR.x = BOARD_OFFSET_X;
    mapDestR.y = BOARD_OFFSET_Y;
    mapDestR.w = BOARD_SIZE_X;
    mapDestR.h = BOARD_SIZE_Y;
}
void Game::InitEnemies(void)
{
    for(int i = 0; i < NUM_ENEMIES; i++)
    {
        Enemy *enemy =  new Enemy(300, 100, 50);
        Pathfinder::getInstance()->UpdatePath(50, 59, enemy);
        enemies.push_back(enemy);
    }
}
void Game::InitTowers(void)
{
    int xPos = BOARD_OFFSET_X + RECT_W * 4;
    int yPos = BOARD_OFFSET_Y + BOARD_SIZE_Y;
    for(int i = 0; i < NUM_TOWERS; i++)
    {
        SDL_Rect towerDstR = {.x = xPos, .y = yPos, .w = RECT_W, .h = RECT_H};
        towerShop[i] = Tower(TextureManager::getInstance()->GetTower(i), (TD::TowerType)(i+1), towerDstR);
        insideTowerShop[i] = false;
        xPos += RECT_W;
    }
    towerTempTexture = NULL;
    towerTempDestR.w = RECT_W;
    towerTempDestR.h = RECT_H;
    currentTower = TD::TOWER_NONE;
    mouseOverTower = -1;
}

bool Game::InsideRect(int pointX, int pointY, SDL_Rect *rect)
{
    if (pointX >= rect->x && pointX <= (rect->x + rect->w))
        if (pointY >= rect->y && pointY <= (rect->y + rect->h))
            return true;
    return false;
}

void Game::DisplayShop(void)
{
    int xPos = BOARD_OFFSET_X + RECT_W * 4;
    int yPos = BOARD_OFFSET_Y + BOARD_SIZE_Y + RECT_H;
    for (int i = 0; i < NUM_TOWERS; i++)
    {
        char text[16];
        sprintf(text, "$%d", towerShop[i].GetCost());
        FontRenderer::getInstance()->DisplayText(text, xPos, yPos);
        if (insideTowerShop[i])
           towerShop[i].SetTexture(TextureManager::getInstance()->GetSelectedTower(i));
        else
           towerShop[i].SetTexture(TextureManager::getInstance()->GetTower(i));
        towerShop[i].Render();
        xPos += RECT_W;
    }
}
void Game::DisplayScore(void)
{
    int xPos = BOARD_OFFSET_X + RECT_W;
    int yPos = BOARD_OFFSET_Y/2;
    char text[16];
    sprintf(text, "Score: %d", score);
    FontRenderer::getInstance()->DisplayText(text, xPos, yPos);
}

void Game::DisplayLevel(void)
{
    int xPos = BOARD_OFFSET_X + RECT_W*3;
    int yPos = BOARD_OFFSET_Y/2;
    char text[16];
    sprintf(text, "Level: %d", level);
    FontRenderer::getInstance()->DisplayText(text, xPos, yPos);
}
void Game::DisplayMoney(void)
{
    int xPos = BOARD_OFFSET_X + RECT_W*5;
    int yPos = BOARD_OFFSET_Y/2;
    char text[16];
    sprintf(text, "Money: $%d", money);
    FontRenderer::getInstance()->DisplayText(text, xPos, yPos);
}
void Game::DisplayLives(void)
{
    int xPos = BOARD_OFFSET_X + RECT_W*8;
    int yPos = BOARD_OFFSET_Y/2;
    char text[16];
    sprintf(text, "Lives: %d", lives);
    FontRenderer::getInstance()->DisplayText(text, xPos, yPos);
}
void Game::DisplayMovingTower(void)
{
    if (!towerTempTexture)
        return;
    if (InsideRect(mouseX, mouseY, &mapDestR))
        towerTempTexture = TextureManager::getInstance()->GetTower(currentTower-1);
    else
        towerTempTexture = TextureManager::getInstance()->GetRedTower(currentTower-1);
    SDL_RenderCopy(GUIManager::getInstance()->GetRenderer(), towerTempTexture, NULL, &towerTempDestR);
}
void Game::DisplayTowers(void)
{
    // render the towers
    for (int i = 0; i < BOARD_SIZE_X/RECT_W; i++)
        for (int j = 0; j < BOARD_SIZE_Y/RECT_H; j++)
            if (towerMap[i][j].GetTexture() != NULL)
            {
                towerMap[i][j].Render();
            }

    // render the circle if mouse is over tower
    if (mouseOverTower != -1)
    {
        int i = mouseOverTower / (BOARD_SIZE_X / RECT_W);
        int j = mouseOverTower % (BOARD_SIZE_X / RECT_W);

        SDL_Rect rect;
        rect.w = towerMap[i][j].GetRadius();
        rect.h = towerMap[i][j].GetRadius();
        rect.x = towerMap[i][j].GetDestRect().x - rect.w/2 + RECT_W/2;
        rect.y = towerMap[i][j].GetDestRect().y - rect.h/2 + RECT_H/2;
        SDL_RenderCopy(GUIManager::getInstance()->GetRenderer(), TextureManager::getInstance()->GetTowerRadius(), NULL, &rect);
    }
}
void Game::DisplayEnemies(void)
{
    static int numcalls = 0;
    for(int i = 0; i < enemies.size(); i++)
    {
        if(numcalls % 70 == 0 && numcalls / 70 == i)
            enemies[i]->SetVisible(true);
        enemies[i]->Render();
    }
    if(enemies.size() == 0){
        printf("Level completed\n");
        level++;
        InitEnemies();
        numcalls = 0;
    }
    if(!paused)
        numcalls++;
}

void Game::DisplayProjectiles(void)
{
    double angle = 0;
    static float t[BOARD_SIZE_X/RECT_W][BOARD_SIZE_Y/RECT_H] = {0};
    for (int i = 0; i < BOARD_SIZE_X/RECT_W; i++)
        for (int j = 0; j < BOARD_SIZE_Y/RECT_H; j++)
            for(int k = 0; k < enemies.size(); k++)
            {
                
                if(insideTower[i][j] && enemies[k]->IsVisible())
                { 
                    SDL_Rect enemyRect = enemies[k]->GetDestRect();
                    SDL_Rect towerRect = towerMap[i][j].GetDestRect();
                    projectileDstR[i][j].x = projectileStartPoint[i][j].x + (projectileEndPoint[i][j].x - projectileStartPoint[i][j].x) * t[i][j];
                    projectileDstR[i][j].y = projectileStartPoint[i][j].y + (projectileEndPoint[i][j].y - projectileStartPoint[i][j].y) * t[i][j];
                    SDL_RenderCopyExF(GUIManager::getInstance()->GetRenderer(), 
                                      TextureManager::getInstance()->GetProjectile(),
                                      NULL, &projectileDstR[i][j], angle, NULL, SDL_FLIP_NONE);
                    t[i][j] += 1/60.0; // set as property of tower
                    // printf("startpoint.x = %f\tstartpoint.y = %f\n", projectileStartPoint.x, projectileStartPoint.y);
                    if(t[i][j] > 1)
                    {
                        // reset projectile to start shooting again
                        t[i][j] = 0;
                        projectileEndPoint[i][j].x = enemyRect.x + enemyRect.w/2;
                        projectileEndPoint[i][j].y = enemyRect.y + enemyRect.h/2;

                        enemies[k]->DecreaseHealth(towerMap[i][j].GetDamage());

                        if(enemies[k]->ShouldDestroy())
                        {
                            Enemy *enemy = enemies[k];
                            enemies.erase(enemies.begin() + k);
                            money += enemy->GetGold();
                            score += enemy->GetScore();
                            delete enemy;
                        }
                    }

                }
            }
}
Game::~Game()
{
    cout << "~Game()\n";
    Pathfinder::getInstance()->~Pathfinder();    
    FontRenderer::getInstance()->~FontRenderer();
    TextureManager::getInstance()->~TextureManager();
    GUIManager::getInstance()->~GUIManager();
    SDL_Quit();
}

void Game::Play(void)
{
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    Uint32 frameTime;
    while (running)
    {
        frameStart = SDL_GetTicks();

        HandleEvents();
        Update();
        Render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime)
            SDL_Delay(frameDelay - frameTime);
        else
            // cout << "it takes too much time " << (int)frameDelay - (int)frameTime << "\n";
        frames++;
    }
}

void Game::Render(void)
{
    SDL_RenderClear(GUIManager::getInstance()->GetRenderer());
    DisplayBackground();
    DisplayEnemies();
    DisplayShop();
    DisplayScore();
    DisplayLevel();
    DisplayLives();
    DisplayMoney();
    DisplayTowers();
    DisplayMovingTower();
    DisplayProjectiles();
    SDL_RenderPresent(GUIManager::getInstance()->GetRenderer());
}
bool Game::InTowerRange(Enemy *enemy, Tower &tower)
{
    Uint32 towerRadius = tower.GetRadius();
    SDL_Rect enemyRect = enemy->GetDestRect();
    SDL_Rect towerRect = tower.GetDestRect();
    SDL_Point enemyPoint;
    SDL_Point towerPoint;

    enemyPoint.x = enemyRect.x + enemyRect.w/2;
    enemyPoint.y = enemyRect.y + enemyRect.h/2;
    towerPoint.x = towerRect.x + towerRect.w/2;
    towerPoint.y = towerRect.y + towerRect.h/2;

    int dx = towerPoint.x - enemyPoint.x;
    int dy = towerPoint.y - enemyPoint.y;
    double distance = sqrt(dx*dx + dy*dy);
    double towerRadiusD = (double) towerRadius / 2;
    if(distance < towerRadiusD)
        return true;
    return false;
}
void Game::Update(void)
{
    // game is paused - we don't do anything
    if (paused)
        return;
    
    // update all enemies positions
    for (int i = 0; i < enemies.size(); i++)
        enemies[i]->Update();
    
    // start shooting projectiles for visible enemies that are in tower range
    for (int i = 0; i < BOARD_SIZE_X/RECT_W; i++)
        for (int j = 0; j < BOARD_SIZE_Y/RECT_H; j++)
            for(int k = 0; k < enemies.size(); k++)
            {
                SDL_Rect towerRect = towerMap[i][j].GetDestRect();
                SDL_Rect enemyRect = enemies[k]->GetDestRect();

                // for each visible enemy and existing tower we check the following
                if(towerMap[i][j].GetTexture() == NULL  || !enemies[k]->IsVisible())
                    continue;
                // if enemy that wasn't in tower range before has entered tower range
                if(!insideTower[i][j] && InTowerRange(enemies[k], towerMap[i][j])){
                    insideTower[i][j] = true;
                    projectileDstR[i][j].x = towerRect.x - towerRect.w/2;
                    projectileDstR[i][j].y = towerRect.y - towerRect.h/2;
                    projectileStartPoint[i][j].x = towerRect.x + towerRect.w/2;
                    projectileStartPoint[i][j].y = towerRect.y + towerRect.h/2;
                    projectileEndPoint[i][j].x   = enemyRect.x + enemyRect.w/2;
                    projectileEndPoint[i][j].y   = enemyRect.y + enemyRect.h/2;
                }
                // if enemy that was previously in tower range has left tower range
                if(insideTower[i][j] && !InTowerRange(enemies[k], towerMap[i][j])){
                    insideTower[i][j] = false;
                }
            }
}

void Game::HandleEvents(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:{
            running = false;
            break;
        }
        case SDL_KEYDOWN:{
            switch (event.key.keysym.sym)
            {
            case SDLK_SPACE:
            case SDLK_p:
                paused = !paused;
                break;
            }
            break;
        }
        case SDL_MOUSEMOTION:{
            HandleMouseMotion(&event);
            break;
        }
        case SDL_MOUSEBUTTONDOWN:{
            HandleMouseDown(&event);
            break;
        }
        case SDL_MOUSEBUTTONUP:{
            HandleMouseUp(&event);
            break;
        }
        default:{
            //printf("Unhandled event %d\n", event.type);
            break;
        }
        }
    }
}

void Game::HandleMouseMotion(SDL_Event *event)
{
    SDL_GetMouseState(&mouseX, &mouseY);

    // update the position of tower that we're drag&dropping 
    if (towerTempTexture)
    {
        towerTempDestR.x = mouseX - towerTempDestR.w/2;
        towerTempDestR.y = mouseY - towerTempDestR.h/2;
        goto HandleMouseMotion_nextEvent;
    }
    // query in which tower from shop(if any) the mouse lays - could do this in O(1)
    for (int i = 0; i < NUM_TOWERS; i++)
    {
        SDL_Rect towerShopRect = towerShop[i].GetDestRect();
        if (InsideRect(mouseX, mouseY, &towerShopRect)){
            towerShop[i].SetTexture(TextureManager::getInstance()->GetSelectedTower(i));
            insideTowerShop[i] = true;
        }
        else if(insideTowerShop[i]){
            towerShop[i].SetTexture(TextureManager::getInstance()->GetTower(i));
            insideTowerShop[i] = false;
        }
    }

HandleMouseMotion_nextEvent:
    // FIXME: don't iterate, use a vector to see if sth is selected
    for (int i = 0; i < BOARD_SIZE_X/RECT_W; i++)
    {
        for (int j = 0; j < BOARD_SIZE_Y/RECT_H; j++)
        {
            if (towerMap[i][j].GetTexture() == NULL)
                continue;
            SDL_Rect towerRect = towerMap[i][j].GetDestRect();
            if (mouseOverTower == -1 && InsideRect(mouseX, mouseY, &towerRect))
            {
                mouseOverTower = i * BOARD_SIZE_X/RECT_W + j;
                goto HandleMouseMotion_nextEvent2;
            }
        }
    }

HandleMouseMotion_nextEvent2:
// when mouse should leave tower
    if (mouseOverTower != -1)
    {
        int i = mouseOverTower / (BOARD_SIZE_X/RECT_W);
        int j = mouseOverTower % (BOARD_SIZE_X/RECT_W);
        SDL_Rect towerRect = towerMap[i][j].GetDestRect();
        if (!InsideRect(mouseX, mouseY, &towerRect))
            mouseOverTower = -1;
    }
}
void Game::HandleMouseDown(SDL_Event *event)
{
    SDL_GetMouseState(&mouseX, &mouseY);
    if (event->button.button == SDL_BUTTON_LEFT && event->button.state == SDL_PRESSED)
    {
        // query in which tower from shop (if any) the mouse lays - could do this in O(1)
        for (int i = 0; i < NUM_TOWERS; i++)
        {
            SDL_Rect towerShopRect = towerShop[i].GetDestRect();
            if (!InsideRect(mouseX, mouseY, &towerShopRect))
                continue;
            towerTempTexture = TextureManager::getInstance()->GetTower(i);
            towerTempDestR.x = mouseX - towerTempDestR.w/2;
            towerTempDestR.y = mouseY - towerTempDestR.h/2;
            currentTower = (TD::TowerType) (i+1);
            break;
        }
    }
}

void Game::HandleMouseUp(SDL_Event *event)
{
    SDL_GetMouseState(&mouseX, &mouseY);
    if (event->button.button == SDL_BUTTON_LEFT && event->button.state == SDL_RELEASED)
    {
        // currently dragging a tower and the mouse button is up
        if (!towerTempTexture)
            goto HandleMouseUp_nextEvent;
        if (!InsideRect(mouseX, mouseY, &mapDestR))
            goto HandleMouseUp_cleanup;
        // i could do the check for which rect it is in O(1)
        for (int i = 0; i < BOARD_SIZE_X/RECT_W; i++)
        {
            for (int j = 0; j < BOARD_SIZE_Y/RECT_H; j++)
            {
                SDL_Rect towerRect = towerMap[i][j].GetDestRect();
                if (!InsideRect(mouseX, mouseY, &towerRect))
                    continue;

                // FIXME: dont actually allow dragging a tower if it's not buyable
                if (towerMap[i][j].GetTexture() == NULL && money >= Tower::GetCost(currentTower))
                {
                    // adding a new tower on the map
                    towerMap[i][j].SetTexture(TextureManager::getInstance()->GetTower(currentTower-1));
                    towerMap[i][j].SetCost(Tower::GetCost(currentTower));
                    towerMap[i][j].SetDamage(Tower::GetDamage(currentTower));
                    towerMap[i][j].SetRadius(Tower::GetRadius(currentTower));
                    towerMap[i][j].SetType(currentTower);
                    money -= Tower::GetCost(currentTower);

                    Pathfinder::SetMatrix(j, i, towerMap[i][j].GetType());
                    for (int k = 0; k < enemies.size(); k++)
                        Pathfinder::getInstance()->UpdatePath(50, 59, enemies[k]);
                }
                else
                {
                    // printf("money: %d costs = %d\n", gameState.money, costs[currentTower-1]);
                    // printf("Tower Existent at %d %d\n", gameState.towerMap[i][j].towerRect.x, gameState.towerMap[i][j].towerRect.y);
                }
                goto HandleMouseUp_cleanup;
            }
        }
HandleMouseUp_cleanup:
        towerTempTexture = NULL;
        currentTower = TD::TOWER_NONE;
HandleMouseUp_nextEvent:
        if (event->button.button == SDL_BUTTON_RIGHT && event->button.state == SDL_RELEASED)
        {
        }
    }
}