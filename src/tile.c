#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <GL/gl.h>
#include <GLES3/gl3.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include "tile.h"
#include "util.h"


#define PGWB_TILE_VERTEX_PATH       "/resources/shaders/tile.vert"
#define PGWB_TILE_FRAGMENT_PATH     "/resources/shaders/tile.frag"

#define PGWB_TILE_COLOUR_GRASS          0.0f, 1.0f, 0.0f
#define PGWB_TILE_COLOUR_FOREST         0.0f, 0.2f, 0.0f
#define PGWB_TILE_COLOUR_DESERT         1.0f, 1.0f, 0.0f
#define PGWB_TILE_COLOUR_MOUNTAIN       0.5f, 0.5f, 0.5f
#define PGWB_TILE_COLOUR_HILL           0.0f, 0.8f, 0.0f


static bool _pgwb_tile_draw_grass(GLuint shader_program, GLuint vao, GLuint ebo, pgwb_tile_t* tile, float x, float y, float width, float height);
static bool _pgwb_tile_draw_forest(GLuint shader_program, GLuint vao, GLuint ebo, pgwb_tile_t* tile, float x, float y, float width, float height);
static bool _pgwb_tile_draw_desert(GLuint shader_program, GLuint vao, GLuint ebo, pgwb_tile_t* tile, float x, float y, float width, float height);
static bool _pgwb_tile_draw_mountain(GLuint shader_program, GLuint vao, GLuint ebo, pgwb_tile_t* tile, float x, float y, float width, float height);
static bool _pgwb_tile_draw_hill(GLuint shader_program, GLuint vao, GLuint ebo, pgwb_tile_t* tile, float x, float y, float width, float height);


void pgwb_tile_ctx_init(pgwb_tile_ctx_t* ctx, GLuint vao, GLFWwindow* window)
{
    memset(ctx, 0, sizeof(pgwb_tile_ctx_t));

    ctx->shader_program = pgwb_load_shaders(PGWB_TILE_VERTEX_PATH, PGWB_TILE_FRAGMENT_PATH);
    if (!ctx->shader_program)
    {
        fprintf(stderr, "Fail to load shaders!\n");
        return;
    }
    glUseProgram(ctx->shader_program);
    float size = 30.f;
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float ratio = (float)width / (float)height;
    float Y = size;
    float X = size * ratio;

    float transform[16] = {
        2.0f / X, 0.0f,      0.0f, 0.0f,
        0.0f,     -2.0f / Y, 0.0f, 0.0f,
        0.0f,     0.0f,      1.0f, 0.0f,
        -1.f,    1.0f,      0.0f, 1.0f
    };
    GLint location = glGetUniformLocation(ctx->shader_program, "u_m4_transform");
    glUniformMatrix4fv(location, 1, GL_FALSE, transform);
}


bool pgwb_tile_draw(GLuint shader_program, GLuint vao, GLuint ebo, pgwb_tile_t* tile, float x, float y, float width, float height)
{
    switch (tile->surface)
    {
#define __PGWB_TILE_DRAW_CASE(_surface, _func)                          \
        case PGWB_TILE_SURFACE_ ## _surface:                                 \
            return _pgwb_tile_draw_ ## _func(shader_program, vao, ebo, tile, x, y, width, height)
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


static bool _pgwb_tile_draw_simple(GLuint shader_program, GLuint vao, GLuint ebo, float x, float y, float width, float height, float r, float g, float b)
{
    glUseProgram(shader_program);
    float vertices[] =
    {
        x+width , y+height  , r, g, b,
        x       , y+height  , r, g, b,
        x+width , y         , r, g, b,
        x       , y         , r, g, b,
    };
    
    GLuint vbo;
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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
static bool _pgwb_tile_draw_ ## _func (GLuint shader_program, GLuint vao, GLuint ebo, pgwb_tile_t* tile, float x, float y, float width, float height) \
{                                                                       \
    return _pgwb_tile_draw_simple(shader_program, vao, ebo, x, y, width, height, PGWB_TILE_COLOUR_ ## _surface); \
}
__PGWB_TILE_DRAW_SIMPLE(grass, GRASS)
__PGWB_TILE_DRAW_SIMPLE(forest, FOREST)
__PGWB_TILE_DRAW_SIMPLE(desert, DESERT)
__PGWB_TILE_DRAW_SIMPLE(mountain, MOUNTAIN)
__PGWB_TILE_DRAW_SIMPLE(hill, HILL)

