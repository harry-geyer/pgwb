#include <stdio.h>
#include <emscripten/emscripten.h>

#include "render.h"


EMSCRIPTEN_KEEPALIVE void button_pressed(char* string, int number);


int main(int argc, char* argv[])
{
    void* render_ctx = NULL;
    pgwb_render_init(&render_ctx);
    emscripten_set_main_loop_arg(pgwb_render_iterate, render_ctx, 0, 1);
    pgwb_render_post_init(render_ctx);
    return 0;
}


EMSCRIPTEN_KEEPALIVE void button_pressed(char* string, int number)
{
    printf("number = %d\n", number);
    printf("string = %s\n", string);
    printf("Pressed\n");
}

