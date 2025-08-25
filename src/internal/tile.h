#pragma once

#include <stdbool.h>


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
} pgwb_tile_t;


bool pgwb_tile_draw(GLuint shader_program, GLuint vao, pgwb_tile_t* tile, float x, float y, float width, float height);

