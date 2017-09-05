#include "nerd_shader.h"

uint shader_compile(GLenum type, const char *source)
{
    uint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == false)
    {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *info = calloc(length, sizeof(char));
        glGetShaderInfoLog(shader, length, NULL, info);
        fprintf(stderr, "shader compilation failed:\n%s\n", info);
        free(info);
    }
    return shader;
}

uint shader_load(GLenum type, const char *path)
{
    char *source = file_load_cstr(path, NULL);
    uint shader = shader_compile(type, source);
    free(source);
    return shader;
}

uint shader_program_link(uint vertex_shader, uint fragment_shader)
{
    uint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == false)
    {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char *info = calloc(length, sizeof(char));
        glGetProgramInfoLog(program, length, NULL, info);
        fprintf(stderr, "program linking failed:\n%s\n", info);
        free(info);
    }
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

uint shader_program_load(const char *vertex_shader_path, const char *fragment_shader_path)
{
    uint vertex_shader = shader_load(GL_VERTEX_SHADER, vertex_shader_path);
    uint fragment_shader = shader_load(GL_FRAGMENT_SHADER, fragment_shader_path);
    uint program = shader_program_link(vertex_shader, fragment_shader);
    return program;
}
