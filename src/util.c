#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GLES3/gl3.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>


char* pgwb_read_shader_source(const char* file_path)
{
    FILE* file = fopen(file_path, "r");
    if (!file)
    {
        fprintf(stderr, "Could not open shader file: %s\n", file_path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* source = (char*)malloc(length + 1);
    if (!source) 
    {
        fprintf(stderr, "Could not allocate memory for shader source\n");
        fclose(file);
        return NULL;
    }

    fread(source, 1, length, file);
    source[length] = '\0'; // Null-terminate the string
    fclose(file);
    return source;
}


GLuint pgwb_compile_shader(GLenum shader_type, const char* source)
{
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, NULL);
glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader compilation error: %s\n", infoLog);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}


GLuint pgwb_load_shaders(const char* vertex_path, const char* fragment_path)
{
    char* vertex_source = pgwb_read_shader_source(vertex_path);
    char* fragment_source = pgwb_read_shader_source(fragment_path);
    if (!vertex_source || !fragment_source)
    {
        free(vertex_source);
        free(fragment_source);
        return 0;
    }

    GLuint vertex_shader = pgwb_compile_shader(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = pgwb_compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    free(vertex_source);
    free(fragment_source);

    if (!vertex_shader || !fragment_shader)
    {
        return 0;
    }

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader_program;
}

