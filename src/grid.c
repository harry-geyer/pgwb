#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/gl.h>
#include <GLES3/gl3.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include <emscripten/emscripten.h>

#define GL_GLEXT_PROTOTYPES
#define EGL_GLEXT_PROTOTYPES

#include "grid.h"
#include "util.h"
#include "perlin.h"

#define PGWB_GRID_VERTEX_PATH       "/resources/shaders/tile.vert"
#define PGWB_GRID_FRAGMENT_PATH     "/resources/shaders/tile.frag"


static void _pgwb_grid_generate(pgwb_grid_ctx_t* ctx, float frequency, float amplitude);


void pgwb_grid_ctx_init(pgwb_grid_ctx_t* ctx, GLFWwindow* window, GLuint vao)
{
    ctx->shader_program = pgwb_load_shaders(PGWB_GRID_VERTEX_PATH, PGWB_GRID_FRAGMENT_PATH);
    if (!ctx->shader_program)
    {
        fprintf(stderr, "Failed to load grid shaders!\n");
        return;
    }
    glUseProgram(ctx->shader_program);
    float size = 200.f;
    int window_height, window_width;
    glfwGetWindowSize(window, &window_width, &window_height);

    float widthf = 2.f / size;
    float heightf = (2.f * (float)window_width) / (size * (float)window_height);
    ctx->width = ceil(2.f/widthf);
    ctx->height = ceil(2.f/heightf);

    ctx->tiles = malloc(sizeof(pgwb_tile_t) * ctx->width * ctx->height);
    if (!ctx->tiles)
    {
        fprintf(stderr, "Failed to allocate memory for tiles\n");
        return;
    }
    _pgwb_grid_generate(ctx, 10.f, 1.f);

    float transform[16] =
    {
        widthf  , 0.0f      , 0.0f, 0.0f,
        0.0f    , -heightf  , 0.0f, 0.0f,
        0.0f    , 0.0f      , 1.0f, 0.0f,
        -1.0f   , 1.0f      , 0.0f, 1.0f,
    };
    GLint location = glGetUniformLocation(ctx->shader_program, "u_m4_transform");
    glUniformMatrix4fv(location, 1, GL_FALSE, transform);
    glBindVertexArray(vao);
    const unsigned indices[] = 
    {
        0, 1, 2,
        1, 2, 3,
    };
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    ctx->ebo = ebo;
}


void pgwb_grid_ctx_deinit(pgwb_grid_ctx_t* ctx)
{
    if (ctx->tiles)
    {
        free(ctx->tiles);
        ctx->tiles = 0;
    }
    glDeleteBuffers(1, &ctx->ebo);
    memset(ctx, 0, sizeof(pgwb_grid_ctx_t));
}


void pgwb_grid_draw(pgwb_grid_ctx_t* ctx, GLFWwindow* window, GLuint vao)
{
    for (int y = 0; y < ctx->height; y++)
    {
        for (int x = 0; x < ctx->width; x++)
        {
            pgwb_tile_t* tile = &ctx->tiles[y * ctx->width + x];
            pgwb_tile_draw(ctx->shader_program, vao, ctx->ebo, tile, x, y, 1.f, 1.f); 
        }
    }
}


static void _pgwb_grid_generate(pgwb_grid_ctx_t* ctx, float frequency, float amplitude)
{
    float* height_map = malloc(sizeof(float) * ctx->width * ctx->height);
    if (!height_map)
    {
        fprintf(stderr, "Failed to allocate memory for height map\n");
        return;
    }
    pgwb_perlin_generate(height_map, ctx->width, ctx->height, frequency, amplitude);
    for (unsigned i = 0; i < ctx->width * ctx->height; i++)
    {
        pgwb_tile_t* tile = &ctx->tiles[i];
        if (height_map[i] > 0.5f)
        {
            tile->surface = PGWB_TILE_SURFACE_GRASS;
        }
        else
        {
            tile->surface = PGWB_TILE_SURFACE_MOUNTAIN;
        }
        tile->height = height_map[i];
    }
}

