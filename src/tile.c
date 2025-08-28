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


bool pgwb_tile_get_colour(pgwb_tile_t* tile, float* r, float* g, float* b)
{
    switch(tile->surface) {
#define __PGWB_TILE_CASE_GET_COLOUR(_surface)                           \
        case PGWB_TILE_SURFACE_ ## _surface:                            \
        {                                                               \
            struct {                                                    \
                float r;                                                \
                float g;                                                \
                float b;                                                \
            } colour = {PGWB_TILE_COLOUR_ ## _surface};                 \
            *r = colour.r;                                              \
            *g = colour.g;                                              \
            *b = colour.b;                                              \
            break;                                                      \
        }
        __PGWB_TILE_CASE_GET_COLOUR(GRASS);
        __PGWB_TILE_CASE_GET_COLOUR(FOREST);
        __PGWB_TILE_CASE_GET_COLOUR(DESERT);
        __PGWB_TILE_CASE_GET_COLOUR(MOUNTAIN);
        __PGWB_TILE_CASE_GET_COLOUR(HILL);
        default:
            return false;
    }
    return true;
}

