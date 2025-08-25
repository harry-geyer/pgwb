#include <stdbool.h>
#include <stdio.h>

#include <GL/gl.h>
#include <GLES3/gl3.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include <emscripten/emscripten.h>

#define GL_GLEXT_PROTOTYPES
#define EGL_GLEXT_PROTOTYPES

#include "tile.h"


#define PGWB_TILE_COLOUR_GRASS          0.0f, 1.0f, 0.0f
#define PGWB_TILE_COLOUR_FOREST         0.0f, 0.2f, 0.0f
#define PGWB_TILE_COLOUR_DESERT         1.0f, 1.0f, 0.0f
#define PGWB_TILE_COLOUR_MOUNTAIN       0.5f, 0.5f, 0.5f
#define PGWB_TILE_COLOUR_HILL           0.0f, 0.8f, 0.0f


static bool _pgwb_tile_draw_grass(GLuint shader_program, GLuint vao, pgwb_tile_t* tile, float x, float y, float width, float height);
static bool _pgwb_tile_draw_forest(GLuint shader_program, GLuint vao, pgwb_tile_t* tile, float x, float y, float width, float height);
static bool _pgwb_tile_draw_desert(GLuint shader_program, GLuint vao, pgwb_tile_t* tile, float x, float y, float width, float height);
static bool _pgwb_tile_draw_mountain(GLuint shader_program, GLuint vao, pgwb_tile_t* tile, float x, float y, float width, float height);
static bool _pgwb_tile_draw_hill(GLuint shader_program, GLuint vao, pgwb_tile_t* tile, float x, float y, float width, float height);


bool pgwb_tile_draw(GLuint shader_program, GLuint vao, pgwb_tile_t* tile, float x, float y, float width, float height)
{
    switch (tile->surface)
    {
#define __PGWB_TILE_DRAW_CASE(_surface, _func)                          \
        case PGWB_TILE_SURFACE_ ## _surface:                                 \
            return _pgwb_tile_draw_ ## _func(shader_program, vao, tile, x, y, width, height)
        __PGWB_TILE_DRAW_CASE(GRASS, grass);
        __PGWB_TILE_DRAW_CASE(FOREST, forest);
        __PGWB_TILE_DRAW_CASE(DESERT, desert);
        __PGWB_TILE_DRAW_CASE(MOUNTAIN, mountain);
        __PGWB_TILE_DRAW_CASE(HILL, hill);
        default:
            break;
    }
    return false;
}


static bool _pgwb_tile_draw_simple(GLuint shader_program, GLuint vao, float x, float y, float width, float height, float r, float g, float b)
{
    glUseProgram(shader_program);
    float vertices[] =
    {
        x+width , y+height  , r, g, b,
        x       , y+height  , r, g, b,
        x+width , y         , r, g, b,
        x       , y         , r, g, b,
    };
    
    GLuint vbo, ebo;
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    unsigned indices[] =
    {
        0, 1, 2,
        1, 2, 3,
    };
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(vertices[0]), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(vertices[0]), (void*)(2 * sizeof(vertices[0])));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    return true;
}


#define __PGWB_TILE_DRAW_SIMPLE(_func, _surface)                        \
static bool _pgwb_tile_draw_ ## _func (GLuint shader_program, GLuint vao, pgwb_tile_t* tile, float x, float y, float width, float height) \
{                                                                       \
    return _pgwb_tile_draw_simple(shader_program, vao, x, y, width, height, PGWB_TILE_COLOUR_ ## _surface); \
}
__PGWB_TILE_DRAW_SIMPLE(grass, GRASS)
__PGWB_TILE_DRAW_SIMPLE(forest, FOREST)
__PGWB_TILE_DRAW_SIMPLE(desert, DESERT)
__PGWB_TILE_DRAW_SIMPLE(mountain, MOUNTAIN)
__PGWB_TILE_DRAW_SIMPLE(hill, HILL)

