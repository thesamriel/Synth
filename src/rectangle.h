#ifndef RECTANGLE_H
#define RECTANGle_H

#include <array>
#include "shader.h"

class Rectangle
{
public:
    Rectangle(int left, int right, int top, int bot, int width, int height);
    void draw();
    void setColor(float* color);

private:
    std::array<float, 3> fill_color = {1.0, 1.0, 1.0};
    bool draw_borders = true;
    std::array<float, 32> vertices;
    std::array<unsigned int, 6> indices;
    unsigned int recVAO, recVBO, recEBO;
    Shader recShader = Shader("../shaders/rec_shader.vshader", "../shaders/rec_shader.fshader");
};

#endif