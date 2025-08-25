#pragma once

#include "tile.h"


typedef struct {
    GLFWwindow* window;
    GLuint shader_program;
    GLuint vao;
    pgwb_tile_ctx_t tile_ctx;
} pgwb_render_ctx_t;


int pgwb_render_init(void** ctx);
int pgwb_render_post_init(void* ctx);
int pgwb_render_deinit(void* ctx);
void pgwb_render_iterate(void* ctx);

