#pragma once

int pgwb_render_init(void** ctx);
int pgwb_render_post_init(void* ctx);
int pgwb_render_deinit(void* ctx);
void pgwb_render_iterate(void* ctx);

