#include "nerd_shader.h"

uint
shader_compile(GLenum type, const char *source)
{
    int status, length;
    uint shader;
    char *info;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == false)
    {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        info = calloc(length, sizeof(char));
        glGetShaderInfoLog(shader, length, NULL, info);
        fprintf(stderr, "shader compilation failed:\n%s\n", info);
        free(info);
    }
    return shader;
}

uint
shader_load(GLenum type, const char *path)
{
    char *source = file_load_cstr(path, NULL);
    uint shader = shader_compile(type, source);
    free(source);
    return shader;
}

uint
shader_program_link(uint vert_shader, uint frag_shader)
{
    int status, length;
    uint program;
    char *info;

    program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == false)
    {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        info = calloc(length, sizeof(char));
        glGetProgramInfoLog(program, length, NULL, info);
        fprintf(stderr, "program linking failed:\n%s\n", info);
        free(info);
    }
    glDetachShader(program, vert_shader);
    glDetachShader(program, frag_shader);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    return program;
}

uint
shader_program_load(const char *vert_shader_path, const char *frag_shader_path)
{
    uint vert_shader = shader_load(GL_VERTEX_SHADER, vert_shader_path);
    uint frag_shader = shader_load(GL_FRAGMENT_SHADER, frag_shader_path);
    uint program = shader_program_link(vert_shader, frag_shader);
    return program;
}
