#include "fbo.h"

#include "std.h"
#include "log.h"
#include "vec.h"

void
fbo_resize(struct fbo *fbo, u32 width, u32 height)
{
    glBindTexture(GL_TEXTURE_2D, fbo->texture);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 width,
                 height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void
fbo_init(struct fbo *fbo, u32 width, u32 height)
{
    GLenum status;
    if (fbo == NULL)
    {
        log_error("Cannot initialize NULL framebuffer");
    }

    // Create framebuffer
    glGenFramebuffers(1, &fbo->handle);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);

    // Create depthbuffer
    glGenRenderbuffers(1, &fbo->depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, fbo->depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER,
                              fbo->depthbuffer);

    // Create texture
    glGenTextures(1, &fbo->texture);
    glBindTexture(GL_TEXTURE_2D, fbo->texture);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 width,
                 height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 NULL);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           fbo->texture,
                           0);

    // Error checking
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        if (status == GL_FRAMEBUFFER_UNSUPPORTED)
        {
            log_error("Framebuffers are unsupported");
        }
        else
        {
            log_error("Could not create framebuffer");
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create screen quad
    static const v2 quad[] = {
        { .x = -1.0f, .y = +1.0f }, { .u = 0.0f, .v = 1.0f },
        { .x = -1.0f, .y = -1.0f }, { .u = 0.0f, .v = 0.0f },
        { .x = +1.0f, .y = -1.0f }, { .u = 1.0f, .v = 0.0f },

        { .x = -1.0f, .y = +1.0f }, { .u = 0.0f, .v = 1.0f },
        { .x = +1.0f, .y = -1.0f }, { .u = 1.0f, .v = 0.0f },
        { .x = +1.0f, .y = +1.0f }, { .u = 1.0f, .v = 1.0f },
    };

    glGenVertexArrays(1, &fbo->vao);
    glGenBuffers(1, &fbo->vbo);

    glBindVertexArray(fbo->vao);
    glBindBuffer(GL_ARRAY_BUFFER, fbo->vao);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(v2), (void *)0);

    // uv
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          false,
                          2 * sizeof(v2),
                          (void *)sizeof(v2));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void
fbo_delete(struct fbo *fbo)
{
    if (fbo == NULL)
    {
        log_warning("Attempted to delete NULL framebuffer");
        return;
    }

    if (fbo->handle)
    {
        glDeleteFramebuffers(1, &fbo->handle);
    }

    if (fbo->depthbuffer)
    {
        glDeleteRenderbuffers(1, &fbo->depthbuffer);
    }

    if (fbo->texture)
    {
        glDeleteTextures(1, &fbo->texture);
    }
}

b32
fbo_is_valid(struct fbo *fbo)
{
    return fbo != NULL && fbo->handle && fbo->depthbuffer && fbo->texture;
}

void
fbo_enable(struct fbo *fbo)
{
    if (!fbo_is_valid(fbo))
    {
        log_error("Cannot enable an invalid framebuffer");
    }

    if (fbo->enabled)
    {
        log_warning("Attempted to enable an already enabled framebuffer");
        return;
    }

    fbo->enabled = true;

    glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
fbo_disable(struct fbo *fbo)
{
    if (!fbo_is_valid(fbo))
    {
        log_error("Cannot disabled an invalid framebuffer");
    }

    if (!fbo->enabled)
    {
        log_warning("Attempted to disable an already disabled framebuffer");
        return;
    }

    // This will actually disable any framebuffer which was attached
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    fbo->enabled = false;
}

void
fbo_render(struct fbo *fbo)
{
    glUseProgram(fbo->shader);
    glBindVertexArray(fbo->vao);
    glBindTexture(GL_TEXTURE_2D, fbo->texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
