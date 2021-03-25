#include "rectangle.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"

Rectangle::Rectangle(int left, int right, int top , int bot, int width, int height)
{
    vertices = {
        (float) left/width*2 - 1,  (float) bot/height*2 -1, 0.0,    1.0, 1.0, 1.0,  0.0, 0.0,
        (float) right/width*2 - 1, (float) bot/height*2 - 1, 0.0,   1.0, 1.0, 1.0,  1.0, 0.0,
        (float) right/width*2 - 1, (float) top/height*2 - 1, 0.0,   1.0, 1.0, 1.0,  1.0, 1.0,
        (float) left/width*2 - 1,  (float) top/height*2 - 1, 0.0,   1.0, 1.0, 1.0,  0.0, 1.0
    };
    indices = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &recVAO);
    glBindVertexArray(recVAO);

    glGenBuffers(1, &recVBO);
    glBindBuffer(GL_ARRAY_BUFFER, recVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &recEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, recEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) (3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) (6*sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Rectangle::setColor(float* color)
{
    for (auto i=0; i<3; i++){
        fill_color[i] = color[i];
    }
}

void Rectangle::draw()
{
    recShader.use();
    recShader.setFloat3("fill_color", fill_color.data());
    glBindVertexArray(recVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}