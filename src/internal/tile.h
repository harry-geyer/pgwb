#pragma once

#include <stdbool.h>

#include <GL/gl.h>
#include <GLFW/glfw3.h>


typedef enum
{
    PGWB_TILE_SURFACE_GRASS,
    PGWB_TILE_SURFACE_FOREST,
    PGWB_TILE_SURFACE_DESERT,
    PGWB_TILE_SURFACE_MOUNTAIN,
    PGWB_TILE_SURFACE_HILL,
} pgwb_tile_surface_t;


typedef struct pgwb_tile {
    pgwb_tile_surface_t surface;
    float height;
} pgwb_tile_t;


typedef struct pgwb_tile_ctx {
    GLuint shader_program;
} pgwb_tile_ctx_t;


void pgwb_tile_ctx_init(pgwb_tile_ctx_t* ctx, GLuint vao, GLFWwindow* window);
bool pgwb_tile_draw(GLuint shader_program, GLuint vao, GLuint ebo, pgwb_tile_t* tile, float x, float y, float width, float height);

