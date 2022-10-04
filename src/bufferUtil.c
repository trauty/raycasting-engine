#include "bufferUtil.h"

ArrayBuffers* genArrayBuffers(Vertex* vertices, GLsizeiptr size, bool isDynamic)
{
    ArrayBuffers* currentBuffers = malloc(sizeof(ArrayBuffers));

    glGenVertexArrays(1, &currentBuffers->vao);
    glGenBuffers(1, &currentBuffers->vbo);

    glBindVertexArray(currentBuffers->vao);
    glBindBuffer(GL_ARRAY_BUFFER, currentBuffers->vbo);

    if (isDynamic)
    {
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return currentBuffers;
}