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

    void (*pause_audio_device)(u32 audio_device, b32 pause);
    void (*lock_audio_device)(u32 audio_device, b32 lock);
};

struct mouse
{
    s32 pos_x, pos_y;
    s32 rel_x, rel_y;
};

struct platform
{
    void *window;
    void *gl;

    struct memory *memory;
    struct platform_api *api;

    s32 mouse_pos_x, mouse_pos_y;
    s32 window_size_x, window_size_y;
};

#endif