#ifndef PLATFORM_H
#define PLATFORM_H
#include "std.h"
#include "memory.h"
#include "mixer.h"
#include "renderer.h"

#include "allocators/allocator.h"

struct audio_spec
{
    u32 audio_device;
    u16 frequency;
    u16 samples;
    s8 channels;
};

struct platform_api
{
    // Memory
    void *(*alloc)(struct allocator *allocator, u32 size);

    // Audio

    void (*open_audio_device)(u32 audio_device,
                              struct audio_spec *want,
                              struct audio_spec *have,
                              void (*callback)(void *udata,
                                               u8 *stream,
                                               s32 len),
                              void *udata);

    void (*close_audio_device)(u32 audio_device);

    void (*pause_audio_device)(u32 audio_device, bool pause);
    void (*lock_audio_device)(u32 audio_device, bool lock);
    void (*unlock_audio_device)(u32 audio_device);

    // Graphics
    void (*render)(struct renderer *renderer);
};

struct platform
{
    void *window;
    void *gl;

    struct memory *memory;
    struct platform_api *api;
};

#endif