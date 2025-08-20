#include <stdio.h>

#include <GLES/gl.h>
#include <GLFW/glfw3.h>
#include <emscripten/emscripten.h>

#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES


static const char* _render_vertex_shader_source = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

static const char* _render_fragment_shader_text =
    "precision mediump float;\n"
    "varying vec3 color;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(color, 1.0);\n"
    "}\n";

static const struct
{
    float x, y;
    float r, g, b;
} _render_vertices[3] =
{
    {-0.6f, -0.4f, 1.f, 0.f, 0.f},
    {0.6f , -0.4f, 0.f, 1.f, 0.f},
    {0.f  , 0.6f , 0.f, 0.f, 1.f},
};


EMSCRIPTEN_KEEPALIVE static void _render_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}


EMSCRIPTEN_KEEPALIVE void render(void)
{
    printf("RENDERING\n");
    glfwSetErrorCallback(_render_error_callback);
    if (!glfwInit())
    {
        printf("Initialisation failed");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Render", glfwGetPrimaryMonitor(), NULL);
    if (!window)
    {
        printf("Creating window failed");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof( float ), _render_vertices, GL_STATIC_DRAW);
     
    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
 
    glfwTerminate();
}
