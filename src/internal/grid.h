#pragma once

#include "tile.h"


typedef struct {
    int width;
    int height;
    pgwb_tile_t* tiles;
    GLuint shader_program;
} pgwb_grid_ctx_t;


void pgwb_grid_ctx_init(pgwb_grid_ctx_t* ctx, GLFWwindow* window);
void pgwb_grid_ctx_deinit(pgwb_grid_ctx_t* ctx);
void pgwb_grid_draw(pgwb_grid_ctx_t* ctx, GLFWwindow* window);

