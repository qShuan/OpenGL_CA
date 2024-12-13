#shader vertex
#version 430

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

out vec2 fragTexCoord;

void main() {
    fragTexCoord = inTexCoord;
    gl_Position = vec4(inPosition, 0.0, 1.0);
}

#shader fragment
#version 430

in vec2 fragTexCoord;
out vec4 fragColor;

layout(std430, binding = 0) buffer CellData {
    float cells[];
};

uniform int width;
uniform int height;

#define EMPTY 0.0
#define SAND 1.0

void main() {

    int x = int(gl_FragCoord.x);
    int y = int(gl_FragCoord.y);

    int index = y * width + x;
    float currentCell = cells[index];

    if (currentCell == SAND) {
        int belowIndex = (y - 1) * width + x;
        if (y > 0 && cells[belowIndex] == EMPTY) {
            cells[belowIndex] = SAND;
            cells[index] = EMPTY;
        }
    }
    if (currentCell == EMPTY) {
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else if (currentCell == SAND) {
        fragColor = vec4(1.0, 1.0, 0.0, 1.0);
    }
}