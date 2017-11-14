#include "assets/texture_asset.h"
#include "renderer.h"
#include "stb_image.h"

void *
asset_load_texture(const char *filename, void *udata)
{
    struct renderer_texture *texture;

    u8 *texture_data;
    u32 format;

    texture = malloc(sizeof(struct renderer_texture));

    unused(udata);

    texture->filename = filename;

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(true);
    texture_data = stbi_load(filename,
                             &texture->width,
                             &texture->height,
                             &texture->num_channels,
                             0);
    if (texture_data)
    {
        format = texture->num_channels == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     format,
                     texture->width,
                     texture->height,
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     texture_data);

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        log_warning("Failed to load texture %s\n", filename);
    }

    stbi_image_free(texture_data);

    return texture;
}