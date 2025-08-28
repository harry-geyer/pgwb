#include <stdio.h>

#include <GL/gl.h>
#include <GLES3/gl3.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <emscripten/emscripten.h>

#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES

#include "render.h"
#include "grid.h"
#include "tile.h"


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

    pgwb_grid_ctx_init(&r_ctx->grid_ctx, window); 

    r_ctx->last_time = glfwGetTime();
    r_ctx->num_frames = 0;
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

    double now = glfwGetTime();
    r_ctx->num_frames++;
    if (now - r_ctx->last_time >= 1.0)
    {
        printf("%d FPS\n", r_ctx->num_frames);
        r_ctx->num_frames = 0;
        r_ctx->last_time += 1.0;
    }

    if (!glfwWindowShouldClose(r_ctx->window))
    {
        int width, height;
        glfwGetFramebufferSize(r_ctx->window, &width, &height);
        glViewport(0, 0, width, height);

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        pgwb_grid_draw(&r_ctx->grid_ctx, r_ctx->window);
        glfwSwapBuffers(r_ctx->window);
    }
    else 
    {
        pgwb_grid_ctx_deinit(&r_ctx->grid_ctx);
        glfwDestroyWindow(r_ctx->window);
        glfwTerminate();
    }
}


static void _pgwb_render_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

