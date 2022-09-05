#include <stdio.h>

#include "glad.h"
#include "glfw3.h"

#include "shader.h"
#include "shaderCode.h"

#define MAP_WIDTH 8
#define MAP_HEIGHT 8

int screenWidth = 1024, screenHeight = 576;

float playerX = 300, playerY = 300;

GLfloat vertices[] = 
{
    1.0f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f,
    0.8f, -0.2f, 0.0f
};

int map[MAP_WIDTH][MAP_HEIGHT] = 
{
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 2, 1, 2, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
};

void drawPlayer()
{
    
}

void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    screenWidth = width;
    screenHeight = height;
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =  glfwCreateWindow(screenWidth, screenHeight, "raycasting-engine", NULL, NULL);
    if (window == NULL)
    {
        printf("Error while trying to create window!");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(window, windowSizeCallback);
    
    gladLoadGL();

    glViewport(0, 0, screenWidth, screenHeight);

    glEnable(GL_PROGRAM_POINT_SIZE);

    GLuint shaderID = createShader(vertexShaderSource, fragmentShaderSource);

    GLuint vao, vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderID);
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, 3);

        drawPlayer();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}