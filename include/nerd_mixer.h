#pragma once

#include "nerd.h"
#include "nerd_file.h"
#include "nerd_math.h"
#include "nerd_memory.h"

#include "stb_vorbis.h"

#define MIXER_BUFFER_SIZE (512)
#define MIXER_BUFFER_MASK (MIXER_BUFFER_SIZE - 1)

struct mixer_event
{
    int type;
    void *udata;
    const char *msg;
    int16 *buffer;
    int length;
};

typedef void (*mixer_event_handler)(struct mixer_event *e);

struct mixer_source_info
{
    mixer_event_handler handler;
    void *udata;
    int samplerate;
    int length;
};

struct mixer_source
{
    struct mixer_source *next;       /* Next source in list */
    int16 buffer[MIXER_BUFFER_SIZE]; /* Internal buffer with raw stereo PCM */
    mixer_event_handler handler;     /* Event handler */
    void *udata;                     /* Stream's udata (from Mixer_SourceInfo) */
    int samplerate;                  /* Stream's native samplerate */
    int length;                      /* Stream's length in frames */
    int end;                         /* End index for the current play-through */
    int state;                       /* Current state (playing|paused|stopped) */
    int64 position;                  /* Current playhead position (fixed point) */
    int lgain, rgain;                /* Left and right gain (fixed point) */
    int rate;                        /* Playback rate (fixed point) */
    int nextfill;                    /* Next frame idx where the buffer needs to be filled */
    int loop;                        /* Whether the source will loop when `end` is reached */
    int rewind;                      /* Whether the source will rewind before playing */
    int active;                      /* Whether the source is part of `sources` list */
    double gain;                     /* Gain set by `mixer_set_gain()` */
    double pan;                      /* Pan set by `mixer_set_pan()` */
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
    const char *lasterror;           /* Last error message */
    mixer_event_handler lock;        /* Event handler for lock/unlock events */
    struct mixer_source *sources;    /* Linked list of active (playing) sources */
    int32 buffer[MIXER_BUFFER_SIZE]; /* Internal master buffer */
    int samplerate;                  /* Master samplerate */
    int gain;                        /* Master gain (fixed point) */
};

struct ogg_stream
{
    stb_vorbis *ogg;
    void *data;
};

const char *mixer_get_error(struct mixer *mixer);
void mixer_init(struct mixer *mixer, int samplerate);
void mixer_set_lock(struct mixer *mixer, mixer_event_handler lock);
void mixer_set_master_gain(struct mixer *mixer, double gain);
void mixer_process(struct mixer *mixer, int16 *destination, int length);

struct mixer_source *mixer_new_source(struct mixer *mixer, const struct mixer_source_info *info);
struct mixer_source *mixer_new_source_from_file(struct mixer *mixer, const char *filename);
struct mixer_source *mixer_new_source_from_mem(struct mixer *mixer, void *data, int size);
void mixer_destroy_source(struct mixer *mixer, struct mixer_source *source);
double mixer_get_length(struct mixer_source *source);
double mixer_get_position(struct mixer_source *source);
int mixer_get_state(struct mixer_source *source);
void mixer_set_gain(struct mixer_source *source, double gain);
void mixer_set_pan(struct mixer_source *source, double pan);
void mixer_set_pitch(struct mixer *mixer, struct mixer_source *source, double pitch);
void mixer_set_loop(struct mixer_source *source, int loop);
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