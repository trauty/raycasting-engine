#include <stdio.h>
#include <math.h>

#include "glad.h"
#include "glfw3.h"

#include "shader.h"
#include "vertex.h"
#include "vector.h"
#include "shaderCode.h"
#include "bufferUtil.h"

#define MAP_WIDTH 16
#define MAP_HEIGHT 16
#define MAP_SIZE 128

GLFWwindow* window = NULL;

int screenWidth = 1280, screenHeight = 720;
int minimapOffset = 0;

float playerPosX = 2.0, playerPosY = 3.0;
float dirX = -1.0, dirY = 0.0;
float planeX = 0.0, planeY = 0.66;
float rotationSpeed = 4.0;

float playerDeltaX = 0.0f;
float playerDeltaY = 0.0f;

float movementSpeed = 4.0f;

Vertex playerVertex;

Vertex playerDirVertices[2];

Vertex minimapVertices[2048];

Vertex raycastVertices[4096];

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

int map[MAP_WIDTH][MAP_HEIGHT] =
{
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
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
    float deltaRot = rotationSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        float oldDirX = dirX;
        dirX = dirX * cos(-deltaRot) - dirY * sin(-deltaRot);
        dirY = oldDirX * sin(-deltaRot) + dirY * cos(-deltaRot);
        
        float oldPlaneX = planeX;
        planeX = planeX * cos(-deltaRot) - planeY * sin(-deltaRot);
        planeY = oldPlaneX * sin(-deltaRot) + planeY * cos(-deltaRot);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        float oldDirX = dirX;
        dirX = dirX * cos(deltaRot) - dirY * sin(deltaRot);
        dirY = oldDirX * sin(deltaRot) + dirY * cos(deltaRot);
        
        float oldPlaneX = planeX;
        planeX = planeX * cos(deltaRot) - planeY * sin(deltaRot);
        planeY = oldPlaneX * sin(deltaRot) + planeY * cos(deltaRot);
    }

    playerDeltaX = dirX * movementSpeed * deltaTime;
    playerDeltaY = dirY * movementSpeed * deltaTime;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (map[(int)(playerPosX + dirX)][(int)playerPosY] == 0) { playerPosX += playerDeltaX; }
        if (map[(int)playerPosX][(int)(playerPosY + dirY)] == 0) { playerPosY += playerDeltaY; }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (map[(int)(playerPosX - dirX)][(int)playerPosY] == 0) { playerPosX -= playerDeltaX; }
        if (map[(int)playerPosX][(int)(playerPosY - dirY)] == 0) { playerPosY -= playerDeltaY; }
    }
}

void initMinimap()
{
    int xOffset = 0, yOffset = 0, minimapArrOffset = 0;
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            GLfloat color = 0.0f;
            if (map[x][y] == 1)
            {
                color = 1.0f;
            }

            minimapOffset = screenHeight - MAP_SIZE;

            xOffset = x * MAP_SIZE / MAP_WIDTH;
            yOffset = y * MAP_SIZE / MAP_HEIGHT + minimapOffset;

            newVertex(&minimapVertices[minimapArrOffset], xOffset, yOffset, color, color, color);    
            newVertex(&minimapVertices[minimapArrOffset + 1], xOffset, yOffset + MAP_SIZE / MAP_HEIGHT, color, color, color);
            newVertex(&minimapVertices[minimapArrOffset + 2], xOffset + MAP_SIZE / MAP_WIDTH, yOffset + MAP_SIZE / MAP_HEIGHT, color, color, color);
            newVertex(&minimapVertices[minimapArrOffset + 3], xOffset, yOffset, color, color, color);
            newVertex(&minimapVertices[minimapArrOffset + 4], xOffset + MAP_SIZE / MAP_WIDTH, yOffset, color, color, color);
            newVertex(&minimapVertices[minimapArrOffset + 5], xOffset + MAP_SIZE / MAP_WIDTH, yOffset + MAP_SIZE / MAP_HEIGHT, color, color, color);
        
            minimapArrOffset += 6;
        }
    }
}

void raycast()
{
    int lineVerticesOffset = 0;
    int worldVerticesOffset = 0;

    for (int rays = 0; rays < screenWidth; rays++)
    {
        float cameraX = 2.0f * rays / (float)screenWidth - 1.0f;
        float rayDirX = dirX + planeX * cameraX;
        float rayDirY = dirY + planeY * cameraX;

        int mapX = (int)playerPosX;
        int mapY = (int)playerPosY;

        float sideDistX = 0.0f;
        float sideDistY = 0.0f;

        float deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1.0 / rayDirX);
        float deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1.0 / rayDirY);
        float perpWallDist = 0.0f;

        int stepX = 0;
        int stepY = 0;

        int hit = 0;
        int side = 0;

        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (playerPosX - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0f - playerPosX) * deltaDistX;
        }
        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (playerPosY - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0f - playerPosY) * deltaDistY;
        }

        while (hit == 0)
        {
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            
            if (map[mapX][mapY] > 0)
            {
                hit = 1;
            }
        }
        
        if (side == 0)
        {
            perpWallDist = (sideDistX - deltaDistX);
        }
        else
        {
            perpWallDist = (sideDistY - deltaDistY);
        }

        if (cameraX == 0)
        {
            newVertex(&playerDirVertices[0], playerVertex.x, playerVertex.y, 0.0f, 1.0f, 0.0f);
            newVertex(&playerDirVertices[1], playerVertex.x + dirX * perpWallDist * MAP_SIZE / MAP_WIDTH, playerVertex.y + dirY * perpWallDist * MAP_SIZE / MAP_HEIGHT, 0.0f, 1.0f, 0.0f);
        }

        int lineHeight = (int)(screenHeight / perpWallDist);

        int drawStart = -lineHeight / 2 + screenHeight / 2;
        if (drawStart < 0)
        {
            drawStart = 0;
        }
        
        int drawEnd = lineHeight / 2 + screenHeight / 2;
        if (drawEnd >= screenHeight)
        {
            drawEnd = screenHeight - 1;
        }

        float greenColor = 0.0f;
        if (side == 1)
        {
            greenColor = 0.6f;
        }
        else
        {
            greenColor = 1.0f;
        }

        newVertex(&raycastVertices[lineVerticesOffset], rays, drawStart, 0.1f, greenColor, 0.1f);
        newVertex(&raycastVertices[lineVerticesOffset + 1], rays, drawEnd, 0.1f, greenColor, 0.1f);

        lineVerticesOffset += 2;
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

    glfwSwapInterval(0);

    gladLoadGL();

    initMinimap();

    glViewport(0, 0, screenWidth, screenHeight);

    glEnable(GL_PROGRAM_POINT_SIZE);

    playerVertex.colR = 0.1f;
    playerVertex.colG = 0.1f;
    playerVertex.colB = 1.0f;

    GLuint shaderID = createShader(vertexShaderSource, fragmentShaderSource);

    ArrayBuffers* playerBuffers = genArrayBuffers(&playerVertex, sizeof(playerVertex), true);
    
    ArrayBuffers* playerDirBuffers = genArrayBuffers(playerDirVertices, sizeof(playerVertex), true);

    ArrayBuffers* minimapBuffers = genArrayBuffers(minimapVertices, sizeof(minimapVertices), false);

    ArrayBuffers* raycastLineBuffers = genArrayBuffers(raycastVertices, sizeof(raycastVertices), true);
    
    ArrayBuffers* worldBuffers = genArrayBuffers(worldVertices, sizeof(worldVertices), true);

    while (!glfwWindowShouldClose(window))
    {
        raycast();
        
        playerVertex.x = playerPosX * MAP_SIZE / MAP_WIDTH;
        playerVertex.y = playerPosY * MAP_SIZE / MAP_HEIGHT + minimapOffset;

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

        glBindVertexArray(playerDirBuffers->vao);
        glBindBuffer(GL_ARRAY_BUFFER, playerDirBuffers->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(playerDirVertices), playerDirVertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINES, 0, sizeof(playerDirVertices) / sizeof(GLfloat));

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