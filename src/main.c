#include <stdio.h>
#include <math.h>

#include "glad.h"
#include "glfw3.h"

#include "shader.h"
#include "shaderCode.h"

#define PI 3.14159265359
#define DR 0.0174533

#define MAP_WIDTH 8
#define MAP_HEIGHT 8
#define MAP_SIZE 64

GLFWwindow* window = NULL;

int screenWidth = 1024, screenHeight = 1024;

float playerPosX = 512.0f, playerPosY = 512.0f;
float playerDeltaX = 0.0f, playerDeltaY = 0.0f;
float playerAngle = 0.0f;
float rotationSpeed = 4.0f;

float movementSpeed = 250.0f;

GLfloat playerVertices[] = 
{
    512.0f, 512.0f, 0.0f, 1.0f, 0.0f
};

GLfloat minimapVertices[2048];

GLfloat raycastVertices[2048];

GLfloat worldVertices[2048];

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

            xOffset = x * MAP_SIZE;
            yOffset = y * MAP_SIZE;

            minimapVertices[minimapArrOffset]     = xOffset;
            minimapVertices[minimapArrOffset + 1] = yOffset;
            
            minimapVertices[minimapArrOffset + 2] = color;
            minimapVertices[minimapArrOffset + 3] = color;
            minimapVertices[minimapArrOffset + 4] = color;
            
            minimapVertices[minimapArrOffset + 5] = xOffset;
            minimapVertices[minimapArrOffset + 6] = yOffset + MAP_SIZE;
            
            minimapVertices[minimapArrOffset + 7] = color;
            minimapVertices[minimapArrOffset + 8] = color;
            minimapVertices[minimapArrOffset + 9] = color;
            
            minimapVertices[minimapArrOffset + 10] = xOffset + MAP_SIZE;
            minimapVertices[minimapArrOffset + 11] = yOffset + MAP_SIZE;
            
            minimapVertices[minimapArrOffset + 12] = color;
            minimapVertices[minimapArrOffset + 13] = color;
            minimapVertices[minimapArrOffset + 14] = color;

            minimapVertices[minimapArrOffset + 15] = xOffset;
            minimapVertices[minimapArrOffset + 16] = yOffset;
            
            minimapVertices[minimapArrOffset + 17] = color;
            minimapVertices[minimapArrOffset + 18] = color;
            minimapVertices[minimapArrOffset + 19] = color;

            minimapVertices[minimapArrOffset + 20] = xOffset + MAP_SIZE;
            minimapVertices[minimapArrOffset + 21] = yOffset;
            
            minimapVertices[minimapArrOffset + 22] = color;
            minimapVertices[minimapArrOffset + 23] = color;
            minimapVertices[minimapArrOffset + 24] = color;

            minimapVertices[minimapArrOffset + 25] = xOffset + MAP_SIZE;
            minimapVertices[minimapArrOffset + 26] = yOffset + MAP_SIZE;

            minimapVertices[minimapArrOffset + 27] = color;
            minimapVertices[minimapArrOffset + 28] = color;
            minimapVertices[minimapArrOffset + 29] = color;
            
            minimapArrOffset += 30;
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

    rayAngle = playerAngle - DR * 30;
    if (rayAngle < 0)
    {
        rayAngle += 2 * PI;
    }
    else if (rayAngle > 2 * PI)
    {
        rayAngle -= 2 * PI;
    }

    for (int rays = 0; rays < 60; rays++)
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

        raycastVertices[lineVerticesOffset] = playerPosX;
        raycastVertices[lineVerticesOffset + 1] = playerPosY;
    
        raycastVertices[lineVerticesOffset + 2] = 0.1f;
        raycastVertices[lineVerticesOffset + 3] = 1.0f;
        raycastVertices[lineVerticesOffset + 4] = 0.1f;

        raycastVertices[lineVerticesOffset + 5] = rayX;
        raycastVertices[lineVerticesOffset + 6] = rayY;
    
        raycastVertices[lineVerticesOffset + 7] = 0.1f;
        raycastVertices[lineVerticesOffset + 8] = 1.0f;
        raycastVertices[lineVerticesOffset + 9] = 0.1f;

        lineVerticesOffset += 10;

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

        float lineHeight = (MAP_SIZE * 320) / distT;
        if (lineHeight > 320)
        {
            lineHeight = 320;
        }

        float lineOffset = 160 - lineHeight / 2;

        worldVertices[worldVerticesOffset] = rays * 8 + 530;
        worldVertices[worldVerticesOffset + 1] = lineOffset;
        worldVertices[worldVerticesOffset + 2] = 0.1f;
        worldVertices[worldVerticesOffset + 3] = color;
        worldVertices[worldVerticesOffset + 4] = 0.1f;

        worldVertices[worldVerticesOffset + 5] = rays * 8 + 530;
        worldVertices[worldVerticesOffset + 6] = lineHeight + lineOffset;
        worldVertices[worldVerticesOffset + 7] = 0.1f;
        worldVertices[worldVerticesOffset + 8] = color;
        worldVertices[worldVerticesOffset + 9] = 0.1f;

        worldVertices[worldVerticesOffset + 10] = rays * 8 + 538;
        worldVertices[worldVerticesOffset + 11] = lineOffset;
        worldVertices[worldVerticesOffset + 12] = 0.1f;
        worldVertices[worldVerticesOffset + 13] = color;
        worldVertices[worldVerticesOffset + 14] = 0.1f;

        worldVertices[worldVerticesOffset + 15] = rays * 8 + 530;
        worldVertices[worldVerticesOffset + 16] = lineHeight + lineOffset;
        worldVertices[worldVerticesOffset + 17] = 0.1f;
        worldVertices[worldVerticesOffset + 18] = color;
        worldVertices[worldVerticesOffset + 19] = 0.1f;

        worldVertices[worldVerticesOffset + 20] = rays * 8 + 538;
        worldVertices[worldVerticesOffset + 21] = lineHeight + lineOffset;
        worldVertices[worldVerticesOffset + 22] = 0.1f;
        worldVertices[worldVerticesOffset + 23] = color;
        worldVertices[worldVerticesOffset + 24] = 0.1f;

        worldVertices[worldVerticesOffset + 25] = rays * 8 + 538;
        worldVertices[worldVerticesOffset + 26] = lineOffset;
        worldVertices[worldVerticesOffset + 27] = 0.1f;
        worldVertices[worldVerticesOffset + 28] = color;
        worldVertices[worldVerticesOffset + 29] = 0.1f;

        worldVerticesOffset += 30;

        rayAngle += DR;

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

    //glfwSwapInterval(1);

    gladLoadGL();

    initMinimap();

    glViewport(0, 0, screenWidth, screenHeight);

    glEnable(GL_PROGRAM_POINT_SIZE);

    GLuint shaderID = createShader(vertexShaderSource, fragmentShaderSource);

    GLuint playerVAO, playerVBO;

    glGenVertexArrays(1, &playerVAO);
    glGenBuffers(1, &playerVBO);

    glBindVertexArray(playerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, playerVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertices), playerVertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint minimapVAO, minimapVBO;

    glGenVertexArrays(1, &minimapVAO);
    glGenBuffers(1, &minimapVBO);

    glBindVertexArray(minimapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, minimapVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(minimapVertices), minimapVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint raycastVAO, raycastVBO;

    glGenVertexArrays(1, &raycastVAO);
    glGenBuffers(1, &raycastVBO);

    glBindVertexArray(raycastVAO);
    glBindBuffer(GL_ARRAY_BUFFER, raycastVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(raycastVertices), raycastVertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint worldVAO, worldVBO;

    glGenVertexArrays(1, &worldVAO);
    glGenBuffers(1, &worldVBO);

    glBindVertexArray(worldVAO);
    glBindBuffer(GL_ARRAY_BUFFER, worldVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(worldVertices), worldVertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        drawRays();
        playerVertices[0] = playerPosX;
        playerVertices[1] = playerPosY;

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        updateOrthoMatrix(screenHeight, 0.0f, 0.0f, screenWidth, 0.0f, -1.0f);

        calculateDeltaTime();

        input();

        glUseProgram(shaderID);

        glBindVertexArray(minimapVAO);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(minimapVertices) / sizeof(GLfloat));

        glBindVertexArray(playerVAO);
        glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertices), playerVertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_POINTS, 0, 1);

        glBindVertexArray(raycastVAO);
        glBindBuffer(GL_ARRAY_BUFFER, raycastVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(raycastVertices), raycastVertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINES, 0, sizeof(raycastVertices) / sizeof(GLfloat));

        glBindVertexArray(worldVAO);
        glBindBuffer(GL_ARRAY_BUFFER, worldVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(worldVertices), worldVertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(worldVertices) / sizeof(GLfloat));


        glUniformMatrix4fv(glGetUniformLocation(shaderID, "orthoMat"), 1, GL_FALSE, orthoMat);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &playerVAO);
    glDeleteBuffers(1, &playerVBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}