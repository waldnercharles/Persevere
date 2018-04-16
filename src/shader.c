#include "shader.h"
#include "file.h"
#include "log.h"

u32
shader_compile(GLenum type, const char *source)
{
    s32 status, length;
    u32 shader;
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
        log_error("Shader compilation failed:\n%s", info);
        free(info);
    }
    return shader;
}

u32
shader_load(struct allocator *allocator, GLenum type, const char *path)
{
    char *source = file_load_cstr(allocator, path, NULL);
    u32 shader = shader_compile(type, source);
    // free(source);
    return shader;
}

u32
shader_program_link(u32 vert_shader, u32 frag_shader)
{
    s32 status, length;
    u32 program;
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
        log_error("Program linking failed:\n%s", info);
        free(info);
    }
    glDetachShader(program, vert_shader);
    glDetachShader(program, frag_shader);
    return program;
}

u32
shader_program_load(struct allocator *allocator,
                    const char *vert_path,
                    const char *frag_path)
{
    u32 vert_shader = shader_load(allocator, GL_VERTEX_SHADER, vert_path);
    u32 frag_shader = shader_load(allocator, GL_FRAGMENT_SHADER, frag_path);
    u32 program = shader_program_link(vert_shader, frag_shader);
    return program;
}
