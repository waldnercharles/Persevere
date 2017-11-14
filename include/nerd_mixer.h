#ifndef NERD_MIXER_H
#define NERD_MIXER_H
#include "nerd_engine.h"

#define MIXER_BUFFER_SIZE (512)
#define MIXER_BUFFER_MASK (MIXER_BUFFER_SIZE - 1)

struct mixer_event
{
    s32 type;
    void *udata;
    const char *msg;
    short *buffer;
    s32 length;
};

typedef void (*mixer_event_handler)(struct mixer_event *e);

struct mixer_source_info
{
    mixer_event_handler handler;
    void *udata;
    s32 samplerate;
    s32 length;
};

struct mixer_source
{
    struct mixer_source *next;       /* Next source in list */
    short buffer[MIXER_BUFFER_SIZE]; /* Internal buffer with raw stereo PCM */
    mixer_event_handler handler;     /* Event handler */

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
    f64 gain;         /* Gain set by `mixer_set_gain()` */
    f64 pan;          /* Pan set by `mixer_set_pan()` */
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
    struct mixer_source *sources;  /* Linked list of active (playing) sources */
    u32 audio_device;              /* Audio device passed to lock/unlock */
    mixer_event_handler lock;      /* Event handler for lock/unlock events */
    s32 buffer[MIXER_BUFFER_SIZE]; /* Internal master buffer */
    const char *lasterror;         /* Last error message */
    s32 samplerate;                /* Master samplerate */
    s32 gain;                      /* Master gain (fixed point) */
};

const char *mixer_get_error(struct mixer *mixer);

struct mixer *mixer_new();
void mixer_init(struct mixer *mixer, s32 samplerate);
void mixer_set_lock(struct mixer *mixer, mixer_event_handler lock);
void mixer_set_master_gain(struct mixer *mixer, f64 gain);
void mixer_process(struct mixer *mixer, short *destination, s32 length);

struct mixer_source *mixer_new_source(struct mixer *mixer,
                                      const struct mixer_source_info *info);

struct mixer_source *mixer_new_source_from_file(struct mixer *mixer,
                                                const char *filename);

struct mixer_source *mixer_new_source_from_mem(struct mixer *mixer,
                                               void *data,
                                               s32 size);

void mixer_destroy_source(struct mixer *mixer, struct mixer_source *source);
f64 mixer_get_length(struct mixer_source *source);
f64 mixer_get_position(struct mixer_source *source);
s32 mixer_get_state(struct mixer_source *source);
void mixer_set_gain(struct mixer_source *source, f64 gain);
void mixer_set_pan(struct mixer_source *source, f64 pan);

void mixer_set_pitch(struct mixer *mixer,
                     struct mixer_source *source,
                     f64 pitch);

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