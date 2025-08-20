#include <stdio.h>

#include <GL/gl.h>
#include <GLES3/gl3.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <emscripten/emscripten.h>

#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES


static const char* _render_vertex_shader_source = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v2_pos;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(v2_pos, 0.0, 1.0);\n"
    "}\n";

static const char* _render_fragment_shader_source = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "out vec3 v3_colour;\n"
    "void main()\n"
    "{\n"
    "    v3_colour = vec3(1, 1, 1);\n"
    "}\n";


static const struct
{
    float x, y;
} _render_vertices[3] =
{
    {-0.6f, -0.4f},
    { 0.6f, -0.4f},
    { 0.f ,  0.6f},
};

static GLFWwindow* _render_window = NULL;
static GLuint _render_shader_program = 0;
static GLuint _render_vao = 0;


static void _render_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}


static void _render_main_loop_iterate(void)
{
    GLFWwindow* window = _render_window;

    if (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(_render_shader_program);
        glBindVertexArray(_render_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
    }
    else 
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}


void render(void)
{
    printf("RENDERING\n");
    glfwSetErrorCallback(_render_error_callback);
    if (!glfwInit())
    {
        printf("Initialisation failed");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Render", NULL, NULL);
    if (!window)
    {
        printf("Creating window failed");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    printf("Renderer: %s.\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s.\n", glGetString(GL_VERSION));

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), _render_vertices, GL_STATIC_DRAW);
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    _render_vao = vao;

    glEnableVertexAttribArray(0);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &_render_vertex_shader_source, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &_render_fragment_shader_source, NULL);
    glCompileShader(fs);

    _render_shader_program = glCreateProgram();
    glAttachShader(_render_shader_program, fs);
    glAttachShader(_render_shader_program, vs);
    glLinkProgram(_render_shader_program);

    _render_window = window;
    emscripten_set_main_loop(_render_main_loop_iterate, 0, 1); 
    glfwSwapInterval(1);
}

