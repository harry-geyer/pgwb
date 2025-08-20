#include <stdio.h>

#include <GL/gl.h>
#include <GLES3/gl3.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <emscripten/emscripten.h>

#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES


typedef struct {
    GLFWwindow* window;
    GLuint shader_program_id;
    GLuint vao_id;
} pgwb_render_ctx_t;


static const char* _pgwb_render_vertex_shader_source = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v2_pos;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(v2_pos, 0.0, 1.0);\n"
    "}\n";

static const char* _pgwb_render_fragment_shader_source = 
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
} _pgwb_render_vertices[3] =
{
    {-0.6f, -0.4f},
    { 0.6f, -0.4f},
    { 0.f ,  0.6f},
};

static pgwb_render_ctx_t _pgwb_render_ctx =
{
    .window = NULL,
    .shader_program_id = 0,
    .vao_id = 0,
};


static void _pgwb_render_error_callback(int error, const char* description);


int pgwb_render_init(void** ctx)
{
    if (!ctx || *ctx) 
    {
        fprintf(stderr, "Context pointer NULL or already assigned\n");
        return 1;
    }
    *ctx = calloc(1, sizeof(pgwb_render_ctx_t));
    if (!*ctx)
    {
        fprintf(stderr, "Failed allocating memory for rendering context\n");
        return 1;
    }
    pgwb_render_ctx_t* r_ctx = (pgwb_render_ctx_t*)*ctx;

    glfwSetErrorCallback(_pgwb_render_error_callback);
    if (!glfwInit())
    {
        fprintf(stderr, "Initialisation failed\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Render", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Creating window failed");
        glfwTerminate();
        return 1;
    }
    r_ctx->window = window;
    glfwMakeContextCurrent(window);

    printf("Renderer: %s.\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s.\n", glGetString(GL_VERSION));

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), _pgwb_render_vertices, GL_STATIC_DRAW);
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    r_ctx->vao_id = vao;

    glEnableVertexAttribArray(0);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &_pgwb_render_vertex_shader_source, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &_pgwb_render_fragment_shader_source, NULL);
    glCompileShader(fs);

    GLuint shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, fs);
    glAttachShader(shader_program_id, vs);
    glLinkProgram(shader_program_id);
    r_ctx->shader_program_id = shader_program_id;

    return 0;
}


int pgwb_render_post_init(void* ctx)
{
    glfwSwapInterval(1);
    return 0;
}


int pgwb_render_deinit(void* ctx)
{
    if (!ctx)
    {
        fprintf(stderr, "Already deinited render context\n");
        return 1;
    }
    free(ctx);
    return 0;
}


void pgwb_render_iterate(void* ctx)
{
    if (!ctx)
    {
        fprintf(stderr, "Render context has not been initialised\n");
        return;
    }
    pgwb_render_ctx_t* r_ctx = (pgwb_render_ctx_t*)ctx;

    if (!glfwWindowShouldClose(r_ctx->window))
    {
        int width, height;
        glfwGetFramebufferSize(r_ctx->window, &width, &height);
        glViewport(0, 0, width, height);

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(r_ctx->shader_program_id);
        glBindVertexArray(r_ctx->vao_id);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(r_ctx->window);
    }
    else 
    {
        glfwDestroyWindow(r_ctx->window);
        glfwTerminate();
    }
}


static void _pgwb_render_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

