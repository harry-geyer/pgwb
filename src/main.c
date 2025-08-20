#include <stdio.h>
#include <emscripten/emscripten.h>

#include "render.h"


EMSCRIPTEN_KEEPALIVE void button_pressed(char* string, int number);


int main(int argc, char* argv[])
{
    printf("Hello World\n");
    pgwb_render();
    return 0;
}


EMSCRIPTEN_KEEPALIVE void button_pressed(char* string, int number)
{
    printf("number = %d\n", number);
    printf("string = %s\n", string);
    printf("Pressed\n");
}

