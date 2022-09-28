#include <stdio.h>
#include <math.h>

#include "glad.h"
#include "glfw3.h"

#include "shader.h"
#include "shaderCode.h"

#define PI 3.1415926

#define MAP_WIDTH 8
#define MAP_HEIGHT 8
#define MAP_SIZE 64

GLFWwindow* window = NULL;

int screenWidth = 1024, screenHeight = 1024;

float playerX = 512.0f, playerY = 512.0f;

float movementSpeed = 1000.0f;

GLfloat playerVertices[] = 
{
    512.0f, 512.0f, 0.0f
};

GLfloat orthoMat[16] =
{
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

float deltaTime = 0.0f;
float currentFrame = 0.0f;
float previousFrame = 0.0f;

int map[]= 
{
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 1, 1, 1, 0, 1,
    1, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
};

void calculateDeltaTime()
{
    currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - previousFrame;
	previousFrame = currentFrame;
}

void updateOrthoMatrix(GLfloat top, GLfloat bottom, GLfloat left, GLfloat right, GLfloat zFar, GLfloat zNear)
{
    orthoMat[0] = 2.0f / (right - left);
    orthoMat[5] = 2.0f / (top - bottom);
    orthoMat[10] = -2.0f / (zFar - zNear);
    orthoMat[12] = -(right + left) / (right - left);
    orthoMat[13] = -(top + bottom) / (top - bottom);
    orthoMat[14] = -(zFar + zNear) / (zFar - zNear);
    orthoMat[15] = 1.0f;
}

void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    screenWidth = width;
    screenHeight = height;
}

void input()
{   
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        playerVertices[1] += movementSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        playerVertices[1] -= movementSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        playerVertices[0] -= movementSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        playerVertices[0] += movementSpeed * deltaTime;
    }
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window =  glfwCreateWindow(screenWidth, screenHeight, "raycasting-engine", NULL, NULL);
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

    glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertices), playerVertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        updateOrthoMatrix(screenHeight, 0.0f, 0.0f, screenWidth, 0.0f, -1.0f);

        calculateDeltaTime();

        input();

        glUseProgram(shaderID);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertices), playerVertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_POINTS, 0, 1);

        glUniformMatrix4fv(glGetUniformLocation(shaderID, "orthoMat"), 1, GL_FALSE, orthoMat);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}