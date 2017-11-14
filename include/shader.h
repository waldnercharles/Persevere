#ifndef SHADER_H
#define SHADER_H
#include "std.h"

u32 shader_compile(u32 type, const char *source);
u32 shader_load(u32 type, const char *path);
u32 shader_program_link(u32 vert_shader, u32 frag_shader);
u32 shader_program_load(const char *vert_path, const char *frag_path);
#endif