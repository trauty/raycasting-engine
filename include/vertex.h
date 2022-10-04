#pragma once

typedef struct Vertex
{
    float x;
    float y;
    float colR;
    float colG;
    float colB;
}Vertex;

void newVertex(Vertex* currentVertex, float x, float y, float colR, float colG, float colB);