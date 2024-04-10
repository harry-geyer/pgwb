#include <stdio.h>
#include <emscripten/emscripten.h>


int main(int argc, char* argv[])
{
    printf("Hello World\n");
    return 0;
}


EMSCRIPTEN_KEEPALIVE void button_pressed(int argc, char* argv[])
{
    printf("Pressed\n");
}

