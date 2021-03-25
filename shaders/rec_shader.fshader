#version 460 core
in vec3 ourColor;
in vec2 texCoords;

out vec4 FragColor;

void main()
{
    float maxX = 1.0 - 0.04;
    float minX = 0.04;
    float maxY = 1 - minX/4;
    float minY = minX / 4;

    if (texCoords.x < maxX && texCoords.x > minX &&
    texCoords.y < maxY && texCoords.y > minY) {
        FragColor = vec4(ourColor, 1.0);
    }
    else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    
}