#include <stdio.h>
#include <math.h>

#include "glad.h"
#include "glfw3.h"

#include "shader.h"
#include "vertex.h"
#include "shaderCode.h"
#include "bufferUtil.h"

#define PI 3.14159265359
#define DR 0.0174533

#define MAP_WIDTH 8
#define MAP_HEIGHT 8
#define MAP_SIZE 64

GLFWwindow* window = NULL;

int screenWidth = 1280, screenHeight = 720;

float playerPosX = 200.0f, playerPosY = 200.0f;
float playerDeltaX = 0.0f, playerDeltaY = 0.0f;
float playerAngle = 0.0f;
float rotationSpeed = 4.0f;

int maxRays = 180;

float movementSpeed = 250.0f;

Vertex playerVertex;

Vertex minimapVertices[2048];

Vertex raycastVertices[2048];

Vertex worldVertices[2048];

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
    1, 0, 1, 0, 0, 0, 0, 1,
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
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        playerAngle += rotationSpeed * deltaTime;
        
        if (playerAngle > 2 * PI)
        {
            playerAngle -= 2 * PI;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        playerAngle -= rotationSpeed * deltaTime;
        
        if (playerAngle < 0)
        {
            playerAngle += 2 * PI;
        }
    }

    playerDeltaX = cos(playerAngle) * movementSpeed;
    playerDeltaY = sin(playerAngle) * movementSpeed;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        playerPosX += playerDeltaX * deltaTime;
        playerPosY += playerDeltaY * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        playerPosX -= playerDeltaX * deltaTime;
        playerPosY -= playerDeltaY * deltaTime;
    }
}

void initMinimap()
{
    int xOffset = 0, yOffset = 0, minimapArrOffset = 0;
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        for (int y = 0; y < MAP_WIDTH; y++)
        {
            GLfloat color = 0.0f;
            if (map[y * MAP_WIDTH + x] == 1)
            {
                color = 1.0f;
            }

            xOffset = x * MAP_SIZE / 4;
            yOffset = y * MAP_SIZE / 4;

            newVertex(&minimapVertices[minimapArrOffset] ,xOffset, yOffset, color, color, color);    
            newVertex(&minimapVertices[minimapArrOffset + 1], xOffset, yOffset + MAP_SIZE / 4, color, color, color);
            newVertex(&minimapVertices[minimapArrOffset + 2], xOffset + MAP_SIZE / 4, yOffset + MAP_SIZE / 4, color, color, color);
            newVertex(&minimapVertices[minimapArrOffset + 3], xOffset, yOffset, color, color, color);
            newVertex(&minimapVertices[minimapArrOffset + 4], xOffset + MAP_SIZE / 4, yOffset, color, color, color);
            newVertex(&minimapVertices[minimapArrOffset + 5], xOffset + MAP_SIZE / 4, yOffset + MAP_SIZE / 4, color, color, color);
        
            minimapArrOffset += 6;
        }
    }
}

float dist(float ax, float ay, float bx, float by, float angle)
{
    return (sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay)));
}

void drawRays()
{
    int mapX = 0, mapY = 0, mapPos = 0, dof = 0;
    float rayX = 0.0f, rayY = 0.0f, rayAngle = 0.0f, xOffset = 0.0f, yOffset = 0.0f, distT = 0.0f;

    int lineVerticesOffset = 0;
    int worldVerticesOffset = 0;

    rayAngle = playerAngle - DR * 45;
    if (rayAngle < 0)
    {
        rayAngle += 2 * PI;
    }
    else if (rayAngle > 2 * PI)
    {
        rayAngle -= 2 * PI;
    }

    for (int rays = 0; rays < maxRays; rays++)
    {
        float distHor = 100000, horX = playerPosX, horY = playerPosY;
        dof = 0;
        float aTan = -1 / tan(rayAngle);

        if (rayAngle > PI)
        {
            rayY = (((int)playerPosY >> 6) << 6) - 0.0001;
            rayX = (playerPosY - rayY) * aTan + playerPosX;
            yOffset = -64.0f;
            xOffset = -yOffset * aTan;
        }
        else if (rayAngle < PI)
        {
            rayY = (((int)playerPosY >> 6) << 6) + 64;
            rayX = (playerPosY - rayY) * aTan + playerPosX;
            yOffset = 64.0f;
            xOffset = -yOffset * aTan;
        }
        else if (rayAngle == 0 || rayAngle == PI)
        {
            rayX = playerPosX;
            rayY = playerPosY;
            dof = 8;
        }

        while (dof < 8)
        {
            mapX = (int)(rayX) >> 6;
            mapY = (int)(rayY) >> 6;
            mapPos = mapY * MAP_WIDTH + mapX;
            
            if (mapPos > 0 && mapPos < MAP_WIDTH * MAP_HEIGHT && map[mapPos] == 1)
            {
                horX = rayX;
                horY = rayY;
                distHor = dist(playerPosX, playerPosY, horX, horY, rayAngle);
                dof = 8;
            }
            else
            {
                rayX += xOffset;
                rayY += yOffset;
                dof += 1;
            }
        }

        float distVert = 100000, vertX = playerPosX, vertY = playerPosY;
        dof = 0;
        //Vertical check PI2 = PI / 2 , PI3 = 3 * PI / 2 
        float nTan = -tan(rayAngle);

        if (rayAngle > PI / 2 && rayAngle < 3 * PI / 2)
        {
            rayX = (((int)playerPosX >> 6) << 6) - 0.0001;
            rayY = (playerPosX - rayX) * nTan + playerPosY;
            xOffset = -64.0f;
            yOffset = -xOffset * nTan;
        }
        else if (rayAngle < PI / 2 || rayAngle > 3 * PI / 2)
        {
            rayX = (((int)playerPosX >> 6) << 6) + 64;
            rayY = (playerPosX - rayX) * nTan + playerPosY;
            xOffset = 64.0f;
            yOffset = -xOffset * nTan;
        }
        else if (rayAngle == 0 || rayAngle == PI)
        {
            rayX = playerPosX;
            rayY = playerPosY;
            dof = 8;
        }

        while (dof < 8)
        {
            mapX = (int)(rayX) >> 6;
            mapY = (int)(rayY) >> 6;
            mapPos = mapY * MAP_WIDTH + mapX;
        
            if (mapPos > 0 && mapPos < MAP_WIDTH * MAP_HEIGHT && map[mapPos] == 1)
            {
                vertX = rayX;
                vertY = rayY;
                distVert = dist(playerPosX, playerPosY, vertX, vertY, rayAngle);
                dof = 8;
            }
            else
            {
                rayX += xOffset;
                rayY += yOffset;
                dof += 1;
            }
        }

        float color = 0.0f;
        
        if (distVert < distHor)
        {
            rayX = vertX;
            rayY = vertY;
            distT = distVert;
            color = 0.9f;
        }
        else
        {
            rayX = horX;
            rayY = horY;
            distT = distHor;
            color = 0.7f;
        }

        newVertex(&raycastVertices[lineVerticesOffset], playerPosX, playerPosY, 0.1f, 1.0f, 0.1f);
        newVertex(&raycastVertices[lineVerticesOffset + 1], rayX, rayY, 0.1f, 1.0f, 0.1f);

        lineVerticesOffset += 2;

        float cosAngle = playerAngle - rayAngle;
        
        if (cosAngle < 0)
        {
            cosAngle += 2 * PI;
        }
        else if (cosAngle > 2 * PI)
        {
            cosAngle -= 2 * PI;
        }

        distT *= cos(cosAngle);

        float lineHeight = (MAP_SIZE * screenHeight) / distT;
        if (lineHeight > screenHeight)
        {
            lineHeight = screenHeight;
        }

        float lineOffset = screenHeight / 2 - lineHeight / 2;

        float screenScale = screenWidth / maxRays + 1;

        newVertex(&worldVertices[worldVerticesOffset], rays * screenScale, lineOffset, 0.1f, color, 0.1f);
        newVertex(&worldVertices[worldVerticesOffset + 1], rays * screenScale, lineHeight + lineOffset, 0.1f, color, 0.1f);
        newVertex(&worldVertices[worldVerticesOffset + 2], rays * screenScale + screenScale, lineOffset, 0.1f, color, 0.1f);
        newVertex(&worldVertices[worldVerticesOffset + 3], rays * screenScale, lineHeight + lineOffset, 0.1f, color, 0.1f);
        newVertex(&worldVertices[worldVerticesOffset + 4], rays * screenScale + screenScale, lineHeight + lineOffset, 0.1f, color, 0.1f);
        newVertex(&worldVertices[worldVerticesOffset + 5], rays * screenScale + screenScale, lineOffset, 0.1f, color, 0.1f);

        worldVerticesOffset += 6;

        rayAngle += DR / 2;

        if (rayAngle < 0)
        {
            rayAngle += 2 * PI;
        }
        else if (rayAngle > 2 * PI)
        {
            rayAngle -= 2 * PI;
        }
    }
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screenWidth, screenHeight, "raycasting-engine", NULL, NULL);
    if (window == NULL)
    {
        printf("Error while trying to create window!");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(window, windowSizeCallback);

    glfwSwapInterval(1);

    gladLoadGL();

    initMinimap();

    glViewport(0, 0, screenWidth, screenHeight);

    glEnable(GL_PROGRAM_POINT_SIZE);

    playerVertex.colR = 0.1f;
    playerVertex.colG = 0.1f;
    playerVertex.colB = 1.0f;

    GLuint shaderID = createShader(vertexShaderSource, fragmentShaderSource);

    ArrayBuffers* playerBuffers = genArrayBuffers(&playerVertex, sizeof(playerVertex), false);

    ArrayBuffers* minimapBuffers = genArrayBuffers(minimapVertices, sizeof(minimapVertices), false);

    ArrayBuffers* raycastLineBuffers = genArrayBuffers(raycastVertices, sizeof(raycastVertices), true);
    
    ArrayBuffers* worldBuffers = genArrayBuffers(worldVertices, sizeof(worldVertices), true);

    while (!glfwWindowShouldClose(window))
    {
        drawRays();
        playerVertex.x = playerPosX;
        playerVertex.y = playerPosY;

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        updateOrthoMatrix(screenHeight, 0.0f, 0.0f, screenWidth, 0.0f, -1.0f);

        calculateDeltaTime();

        input();

        glUseProgram(shaderID);

        glBindVertexArray(raycastLineBuffers->vao);
        glBindBuffer(GL_ARRAY_BUFFER, raycastLineBuffers->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(raycastVertices), raycastVertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINES, 0, sizeof(raycastVertices) / sizeof(GLfloat));

        glBindVertexArray(worldBuffers->vao);
        glBindBuffer(GL_ARRAY_BUFFER, worldBuffers->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(worldVertices), worldVertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(worldVertices) / sizeof(GLfloat));

        glBindVertexArray(minimapBuffers->vao);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(minimapVertices) / sizeof(GLfloat));

        glBindVertexArray(playerBuffers->vao);
        glBindBuffer(GL_ARRAY_BUFFER, playerBuffers->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertex), &playerVertex, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_POINTS, 0, 1);

        glUniformMatrix4fv(glGetUniformLocation(shaderID, "orthoMat"), 1, GL_FALSE, orthoMat);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}