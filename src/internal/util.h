#pragma once


char* pgwb_read_shader_source(const char* file_path);
GLuint pgwb_compile_shader_source(GLenum shader_type, const char* source);
GLuint pgwb_load_shaders(const char* vertex_path, const char* fragment_path);

