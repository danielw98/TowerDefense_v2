#include <SDL2/SDL.h>
#include "pathfinding.h"
#include <cstring>
#include <iostream>
using namespace std;

#define INFINITY_NODE 255

Uint8 g_matrix[NUM_NODES][NUM_NODES];
Pathfinder* Pathfinder::instance = NULL;
Pathfinder* Pathfinder::getInstance()
{
    if(!instance)
        instance = new Pathfinder;
    return instance;
}
Pathfinder::Pathfinder()
{
    cout << "Pathfinder()\n";
}
Pathfinder::~Pathfinder()
{
    cout << "~Pathfinder()\n";
    instance = NULL;
}

void Pathfinder::ZeroMatrix()
{
    memset(g_matrix, 0, NUM_NODES*NUM_NODES*sizeof(Uint8));
}
void Pathfinder::SetMatrix(int i, int j, TD::TowerType type)
{
    g_matrix[i][j] = (int) type;
}
Uint8 Pathfinder::GetMatrix(int i, int j)
{
    return g_matrix[i][j];
}

void Pathfinder::Path_to_Directions(Enemy *enemy)
{
    int path_length = enemy->GetPathLength();
    enemy->SetDirection(0, TD::EAST); // start going right
    for (int i = 0; i < path_length-1; i++)
    {
        int i1 = enemy->GetPath(i)   / NUM_NODES;
        int i2 = enemy->GetPath(i+1) / NUM_NODES;
        int j1 = enemy->GetPath(i)   % NUM_NODES;
        int j2 = enemy->GetPath(i+1) % NUM_NODES;

        if (j1 > j2)
            enemy->SetDirection(i+1, TD::WEST);
        else if (j1 < j2)
            enemy->SetDirection(i+1, TD::EAST);
        else if (i1 > i2)
            enemy->SetDirection(i+1, TD::NORTH);
        else if (i1 < i2)
            enemy->SetDirection(i+1, TD::SOUTH);
    }
    enemy->SetDirection(enemy->GetPathLength(), TD::EAST); // after last vertex turn right
}

void Pathfinder::UpdatePath(int startnode, int endnode, Enemy *enemy)
{
    Uint8 graph[NUM_VERTICES][NUM_VERTICES];
    int cost[NUM_VERTICES][NUM_VERTICES], distance[NUM_VERTICES], pred[NUM_VERTICES];
    int visited[NUM_VERTICES], count, mindistance, nextnode, i, j, pos;
    int n = NUM_VERTICES;
    
    // swap startnode and endnode because algorithm returns path from end to start, and we need from start to end
    startnode ^= endnode; endnode ^= startnode; startnode ^= endnode;
    // transform the towers placed matrix in an adjacency matrix
    Matrix_to_Graph(graph);
    
    // create cost matrix
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            if (graph[i][j] == 0)
                cost[i][j] = INFINITY_NODE;
            else
                cost[i][j] = graph[i][j];
    // init auxiliary arrays used in the algorithm
    for (i = 0; i < n; i++)
    {
        distance[i] = cost[startnode][i];
        pred[i] = startnode;
        visited[i] = 0;
    }
    // apply Dijkstra's algorithm
    distance[startnode] = 0;
    visited[startnode] = 1;
    count = 1;
    while (count < n - 1)
    {
        mindistance = INFINITY_NODE;
        for (i = 0; i < n; i++)
            if (distance[i] < mindistance && !visited[i])
            {
                mindistance = distance[i];
                nextnode = i;
            }

        visited[nextnode] = 1;
        for (i = 0; i < n; i++)
            if (!visited[i])
                if (mindistance + cost[nextnode][i] < distance[i])
                {
                    distance[i] = mindistance + cost[nextnode][i];
                    pred[i] = nextnode;
                }
        count++;
    }

    // save the path into the array
    j = endnode;
    pos = 0;
    enemy->SetPath(pos++, j);
    do
    {
        j = pred[j];
        enemy->SetPath(pos++, j);
    } while (j != startnode);

    // save the path length
    enemy->SetPathLength(pos);
    Path_to_Directions(enemy);
}

void Pathfinder::Matrix_to_Graph(Uint8 graph[NUM_VERTICES][NUM_VERTICES])
{
    memset(graph, 0, NUM_VERTICES*NUM_VERTICES*sizeof(Uint8));
    for (int i = 0; i < NUM_NODES; i++)
    {
        for (int j = 0; j < NUM_NODES; j++)
        {
            int elem_i = NUM_NODES * i + j;
            int elem_j;

            if (g_matrix[i][j] != 0)
                continue;

            if (i+1 < NUM_NODES){
                elem_j = (i+1) * NUM_NODES + j;
                if (g_matrix[i+1][j] == 0)
                    graph[elem_i][elem_j] = 1;
            }
            if (i-1 >= 0){
                elem_j = (i-1) * NUM_NODES + j;
                if (g_matrix[i-1][j] == 0)
                    graph[elem_i][elem_j] = 1;
            }
            if (j+1 < NUM_NODES){
                elem_j = i * NUM_NODES + j + 1;
                if (g_matrix[i][j+1] == 0)
                    graph[elem_i][elem_j] = 1;
            }
            if (j-1 >= 0){
                elem_j = i * NUM_NODES + j - 1;
                if (g_matrix[i][j-1] == 0)
                    graph[elem_i][elem_j] = 1;
            }
        }
    }
}