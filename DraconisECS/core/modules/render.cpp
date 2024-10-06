#include "render.h"
#include "GL/glew.h"
#include <iostream>

bool module::Render::init()
{
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error: GLEW failed to initialize" << std::endl;
        return false;
    }
    return true;
}

void module::Render::preUpdate()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void module::Render::update()
{
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
}

void module::Render::shutdown()
{
}
