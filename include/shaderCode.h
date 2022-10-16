#pragma once

const char* vertexShaderSource =
{
"#version 330 core\n"

"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"

"out vec3 color;\n"

"uniform mat4 orthoMat;\n"

"void main()\n"
"{\n"
"   gl_PointSize = 3.0;\n"
"   color = aColor;\n"
"   gl_Position = orthoMat * vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"}\n\0"
};

const char* fragmentShaderSource = 
{
"#version 330 core\n"

"in vec3 color;"

"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"   FragColor = vec4(color.x, color.y, color.z, 1.0);\n"
"}\n\0"
};