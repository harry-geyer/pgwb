#pragma once

#include "grid.h"


typedef struct {
    GLFWwindow* window;
    pgwb_grid_ctx_t grid_ctx;
    double last_time;
    int num_frames;
} pgwb_render_ctx_t;


int pgwb_render_init(void** ctx);
int pgwb_render_post_init(void* ctx);
int pgwb_render_deinit(void* ctx);
void pgwb_render_iterate(void* ctx);

