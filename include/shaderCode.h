#pragma once

const char* vertexShaderSource =
{
"#version 330 core\n"

"layout (location = 0) in vec2 aPos;\n"

"void main()\n"
"{\n"
"   gl_PointSize = 5.0;\n"
"   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"}\n\0"
};

const char* fragmentShaderSource = 
{
"#version 330 core\n"

"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"   FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
"}\n\0"
};