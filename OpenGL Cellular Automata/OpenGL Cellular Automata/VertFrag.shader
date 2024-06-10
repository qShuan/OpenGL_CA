#shader vertex
#version 330 core

layout(location = 0) in vec2 position;

uniform mat4 u_MVP;


void main() {

    gl_Position = u_MVP * vec4(position, 0.0, 1.0);
};

#shader fragment
#version 430 core

layout(std430, binding = 0) buffer Colors {
    vec4 colors[];
};

out vec4 color;

void main() {

    color = colors[gl_PrimitiveID / 2];
};