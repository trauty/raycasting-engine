#include <stdlib.h>
#include "vertex.h"

void newVertex(Vertex* currentVertex, float x, float y, float colR, float colG, float colB)
{
    currentVertex->x = x;
    currentVertex->y = y;
    currentVertex->colR = colR;
    currentVertex->colG = colG;
    currentVertex->colB = colB;

    return currentVertex;
}