#ifndef _PATHFINDING_H_
#define _PATHFINDING_H_

#include "structs.h"
#include "enemy.h"
#include "defines.h"

class Pathfinder
{
public:
    // FIXME: change parameters so it only takes the enemy, put startnode and endnode in enemy
    void UpdatePath(int startnode, int endnode, Enemy *enemy);
    static Pathfinder* getInstance();
    static void SetMatrix(int i, int j, TD::TowerType type);
    static Uint8 GetMatrix(int i, int j);
    static void ZeroMatrix();
    ~Pathfinder();
private:
    Pathfinder();
    void Path_to_Directions(Enemy *enemy);
    void Matrix_to_Graph(Uint8 graph[NUM_VERTICES][NUM_VERTICES]);
    static Pathfinder *instance;
};


#endif // _PATHFINDING_H_