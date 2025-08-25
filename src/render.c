#include <stdio.h>

#include <GL/gl.h>
#include <GLES3/gl3.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <emscripten/emscripten.h>

#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES

#include "tile.h"


typedef struct {
    GLFWwindow* window;
    GLuint shader_program_id;
    GLuint vao_id;
} pgwb_render_ctx_t;


static const char* _pgwb_render_vertex_shader_source = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "layout(location = 0) in vec2 in_v2_pos;\n"
    "layout(location = 1) in vec3 in_v3_colour;\n"
    "uniform mat4 u_m4_transform;\n"
    "out vec3 frag_colour;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_m4_transform * vec4(in_v2_pos, 0.0, 1.0);\n"
    "    frag_colour = in_v3_colour;\n"
    "}\n";

static const char* _pgwb_render_fragment_shader_source = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec3 frag_colour;\n"
    "out vec4 out_v4_colour;\n"
    "void main()\n"
    "{\n"
    "    out_v4_colour = vec4(frag_colour, 1.0);\n"
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

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
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
    
    glUseProgram(shader_program_id);
    float size = 30.f;
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float ratio = (float)width / (float)height;
    float Y = size;
    float X = size * ratio;

    float transform[16] = {
        2.0f / X, 0.0f,      0.0f, 0.0f,
        0.0f,     -2.0f / Y, 0.0f, 0.0f,
        0.0f,     0.0f,      1.0f, 0.0f,
        -1.f,    1.0f,      0.0f, 1.0f
    };
    GLint location = glGetUniformLocation(shader_program_id, "u_m4_transform");
    glUniformMatrix4fv(location, 1, GL_FALSE, transform);

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
        pgwb_tile_t tile =
        {
            .surface = PGWB_TILE_SURFACE_GRASS,
        };
        pgwb_tile_draw(r_ctx->shader_program_id, r_ctx->vao_id, &tile, 0, 0, 1, 1);
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

