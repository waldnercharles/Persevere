#include "assets/shader_asset.h"
#include "shader.h"

void *
asset_load_fragment_shader(const char *filename, void *udata)
{
    (void)udata;
    u32 *shader = malloc(sizeof(u32));
    *shader = shader_load(GL_FRAGMENT_SHADER, filename);

    return shader;
}

void *
asset_load_vertex_shader(const char *filename, void *udata)
{
    (void)udata;
    u32 *shader = malloc(sizeof(u32));
    *shader = shader_load(GL_VERTEX_SHADER, filename);

    return shader;
}