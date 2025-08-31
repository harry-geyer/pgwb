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


static void _pgwb_grid_generate(unsigned seed, pgwb_grid_ctx_t* ctx, float frequency, float amplitude);


void pgwb_grid_ctx_init(unsigned seed, pgwb_grid_ctx_t* ctx, GLFWwindow* window)
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
    _pgwb_grid_generate(seed, ctx, 10.f, 1.f);

    float transform[16] =
    {
        widthf  , 0.0f      , 0.0f, 0.0f,
        0.0f    , -heightf  , 0.0f, 0.0f,
        0.0f    , 0.0f      , 1.0f, 0.0f,
        -1.0f   , 1.0f      , 0.0f, 1.0f,
    };
    GLint location = glGetUniformLocation(ctx->shader_program, "u_m4_transform");
    glUniformMatrix4fv(location, 1, GL_FALSE, transform);
}


void pgwb_grid_ctx_deinit(pgwb_grid_ctx_t* ctx)
{
    if (ctx->tiles)
    {
        free(ctx->tiles);
        ctx->tiles = 0;
    }
    memset(ctx, 0, sizeof(pgwb_grid_ctx_t));
}


void pgwb_grid_draw(pgwb_grid_ctx_t* ctx, GLFWwindow* window)
{

    const float tile_width = 1.0f;
    const unsigned tile_vertex_count = (2 + 3) * 4;  // 4 vertices per square, 2 spacial dimensions + 3 colour dimensions

    unsigned count = ctx->height * ctx->width;
    unsigned vertices_array_count = count * tile_vertex_count;
    float* vertices = malloc(sizeof(float) * vertices_array_count);     
    unsigned* indices = malloc(sizeof(unsigned) * count * 6);  // to draw 2 triangles
    for (int y = 0; y < ctx->height; y++)
    {
        for (int x = 0; x < ctx->width; x++)
        {
            unsigned i = y * ctx->width + x; 
            pgwb_tile_t* tile = &ctx->tiles[i];

            float r = 0.f, g = 0.f, b = 0.f;
            pgwb_tile_get_colour(tile, &r, &g, &b);
            unsigned tile_offset = i * tile_vertex_count;
            vertices[tile_offset]     = x;
            vertices[tile_offset + 1] = y;
            vertices[tile_offset + 2] = r;
            vertices[tile_offset + 3] = g;
            vertices[tile_offset + 4] = b;

            vertices[tile_offset + 5] = x + tile_width;
            vertices[tile_offset + 6] = y;
            vertices[tile_offset + 7] = r;
            vertices[tile_offset + 8] = g;
            vertices[tile_offset + 9] = b;

            vertices[tile_offset + 10] = x + tile_width;
            vertices[tile_offset + 11] = y + tile_width;
            vertices[tile_offset + 12] = r;
            vertices[tile_offset + 13] = g;
            vertices[tile_offset + 14] = b;

            vertices[tile_offset + 15] = x;
            vertices[tile_offset + 16] = y + tile_width;
            vertices[tile_offset + 17] = r;
            vertices[tile_offset + 18] = g;
            vertices[tile_offset + 19] = b;

            unsigned int base_index = i * 4;
            indices[i * 6]     = base_index;
            indices[i * 6 + 1] = base_index + 1;
            indices[i * 6 + 2] = base_index + 2;
            indices[i * 6 + 3] = base_index + 2;
            indices[i * 6 + 4] = base_index + 3;
            indices[i * 6 + 5] = base_index;
        }
    }
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices_array_count, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * count * 6, indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(vertices[0]), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(vertices[0]), (void*)(sizeof(vertices[0]) * 2));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(ctx->shader_program);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, count * 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    free(vertices);
    free(indices);
}


static void _pgwb_grid_generate(unsigned seed, pgwb_grid_ctx_t* ctx, float frequency, float amplitude)
{
    float* height_map = malloc(sizeof(float) * ctx->width * ctx->height);
    if (!height_map)
    {
        fprintf(stderr, "Failed to allocate memory for height map\n");
        return;
    }
    pgwb_perlin_generate(seed, height_map, ctx->width, ctx->height, frequency, amplitude);
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

