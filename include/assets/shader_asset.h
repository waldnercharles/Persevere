#ifndef SHADER_ASSET_H
#define SHADER_ASSET_H

#include "std.h"

void *asset_load_fragment_shader(const char *filename, void *udata);
void *asset_load_vertex_shader(const char *filename, void *udata);

#endif