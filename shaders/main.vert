"#version 330 core\n"

"layout (location = 0) in vec2 aPos;\n"

"void main()\n"
"{\n"
    "gl_Position = vec4(aPos.xy, 0.0, 1.0);\n"
"}\0"