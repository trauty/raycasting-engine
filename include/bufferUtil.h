#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "glad.h"
#include "vertex.h"

typedef struct ArrayBuffers
{
    GLuint vao;
    GLuint vbo;
}ArrayBuffers;

ArrayBuffers* genArrayBuffers(Vertex* vertices, GLsizeiptr size, bool isDynamic);