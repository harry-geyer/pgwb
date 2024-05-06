#include <stdio.h>

#include <GLFW/glfw3.h>
#include <emscripten/emscripten.h>


EMSCRIPTEN_KEEPALIVE void render(void)
{
    printf("RENDERING\n");
    GLFWwindow* window = glfwCreateWindow(640, 480, "Render", glfwGetPrimaryMonitor(), NULL);
}
