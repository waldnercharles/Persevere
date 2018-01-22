#ifndef MIXER_H
#define MIXER_H
#include "std.h"
#include "allocators/allocator.h"

#define MIXER_BUFFER_SIZE (512)
#define MIXER_BUFFER_MASK (MIXER_BUFFER_SIZE - 1)

struct mixer_event
{
    s32 type;
    void *udata;
    const char *msg;
    s16 *buffer;
    s32 length;
};

struct mixer_source_info
{
    void (*event_handler)(struct mixer_event *event);
    void *udata;
    s32 samplerate;
    s32 length;
};

struct mixer_source
{
    struct mixer_source *next;       /* Next source in list */
    short buffer[MIXER_BUFFER_SIZE]; /* Internal buffer with raw stereo PCM */
    void (*event_handler)(struct mixer_event *event);

    void *udata;      /* Stream's udata (from Mixer_SourceInfo) */
    s32 samplerate;   /* Stream's native samplerate */
    s32 length;       /* Stream's length in frames */
    s32 end;          /* End index for the current play-through */
    s32 state;        /* Current state (playing|paused|stopped) */
    s64 position;     /* Current playhead position (fixed point) */
    s32 lgain, rgain; /* Left and right gain (fixed point) */
    s32 rate;         /* Playback rate (fixed point) */
    s32 nextfill;     /* Next frame idx where the buffer needs to be filled */
    bool loop;        /* Whether the source will loop when `end` is reached */
    bool rewind;      /* Whether the source will rewind before playing */
    bool active;      /* Whether the source is part of `sources` list */
    r64 gain;         /* Gain set by `mixer_set_gain()` */
    r64 pan;          /* Pan set by `mixer_set_pan()` */
};

enum
{
    MIXER_STATE_STOPPED,
    MIXER_STATE_PLAYING,
    MIXER_STATE_PAUSED
};

enum
{
    MIXER_EVENT_LOCK,
    MIXER_EVENT_UNLOCK,
    MIXER_EVENT_DESTROY,
    MIXER_EVENT_SAMPLES,
    MIXER_EVENT_REWIND
};

struct mixer
{
    struct mixer_source *sources;
    u32 audio_device;
    s32 buffer[MIXER_BUFFER_SIZE];
    s32 samplerate;
    s32 gain;
    void (*lock)(u32 audio_device, bool lock);
    void (*handle_event)(struct mixer_event *event);
    const char *err;
};

const char *mixer_get_error(struct mixer *mixer);

void mixer_init(struct mixer *mixer, void (*lock)(u32 audio_device, bool lock));

void mixer_set_event_handler(struct mixer *mixer,
                             void (*handle_event)(struct mixer_event *event));

void mixer_set_master_audio_device(struct mixer *mixer, u32 audio_device);
void mixer_set_master_samplerate(struct mixer *mixer, s32 samplerate);
void mixer_set_master_gain(struct mixer *mixer, r64 gain);
void mixer_process(struct mixer *mixer, short *destination, s32 length);

struct mixer_source *mixer_new_source(struct mixer *mixer,
                                      struct allocator *allocator,
                                      const struct mixer_source_info *info);

struct mixer_source *mixer_new_source_from_file(struct mixer *mixer,
                                                struct allocator *allocator,
                                                const char *filename);

struct mixer_source *mixer_new_source_from_mem(struct mixer *mixer,
                                               struct allocator *allocator,
                                               void *data,
                                               s32 size);

void mixer_destroy_source(struct mixer *mixer, struct mixer_source *source);
r64 mixer_get_length(struct mixer_source *source);
r64 mixer_get_position(struct mixer_source *source);
s32 mixer_get_state(struct mixer_source *source);
void mixer_set_gain(struct mixer_source *source, r64 gain);
void mixer_set_pan(struct mixer_source *source, r64 pan);

void mixer_set_pitch(struct mixer *mixer,
                     struct mixer_source *source,
                     r64 pitch);

void mixer_set_loop(struct mixer_source *source, bool loop);
void mixer_play(struct mixer *mixer, struct mixer_source *source);
void mixer_pause(struct mixer_source *source);
void mixer_stop(struct mixer_source *source);

#define UNUSED(x) ((void)(x))

/* Fixed Point Linear Interpolation */
#define FX_BITS (12)
#define FX_UNIT (1 << FX_BITS)
#define FX_MASK (FX_UNIT - 1)
#define FX_FROM_FLOAT(f) ((f)*FX_UNIT)
#define FX_LERP(a, b, p) ((a) + ((((b) - (a)) * (p)) >> FX_BITS))
#endif