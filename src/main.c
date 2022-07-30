#include <stdio.h>

#include "glad.h"
#include "glfw3.h"

#define MAP_WIDTH 8
#define MAP_HEIGHT 8

int screenWidth = 1024, screenHeight = 576;

int map[MAP_WIDTH][MAP_HEIGHT] = 
{
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
};

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
    
    gladLoadGL();

    glViewport(0, 0, screenWidth, screenHeight);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}