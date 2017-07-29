#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "common.h"
#include "stb_vorbis.c"

typedef struct mixer_source mixer_source;

typedef struct
{
    int type;
    void *udata;
    const char *msg;
    int16 *buffer;
    int length;
} mixer_event;

typedef void (*mixer_event_handler)(mixer_event *e);

typedef struct
{
    mixer_event_handler handler;
    void *udata;
    int samplerate;
    int length;
} mixer_source_info;

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

const char *mixer_get_error(void);
void mixer_init(int samplerate);
void mixer_set_lock(mixer_event_handler lock);
void mixer_set_master_gain(double gain);
void mixer_process(int16 *destination, int length);
 
mixer_source *mixer_new_source(const mixer_source_info *info);
mixer_source *mixer_new_source_from_file(const char *filename);
mixer_source *mixer_new_source_from_mem(void *data, int size);
void mixer_destroy_source(mixer_source *source);
double mixer_get_length(mixer_source *source);
double mixer_get_position(mixer_source *source);
int mixer_get_state(mixer_source *source);
void mixer_set_gain(mixer_source *source, double gain);
void mixer_set_pan(mixer_source *source, double pan);
void mixer_set_pitch(mixer_source *source, double pitch);
void mixer_set_loop(mixer_source *source, int loop);
void mixer_play(mixer_source *source);
void mixer_pause(mixer_source *source);
void mixer_stop(mixer_source *source);

static const char *mixer__error(const char *msg);
static void mixer__dummy_handler(mixer_event *event);
static void mixer__lock(void);
static void mixer__unlock(void);

static void mixer__rewind_source(mixer_source *source);
static void mixer__fill_source_buffer(mixer_source *source, int offset, int length);
static void mixer__process_source(mixer_source *source, int length);
static int mixer__check_header(void *data, int size, const char *str, int offset);
static mixer_source *mixer__new_source_from_mem(void *data, int size, int ownsdata);
static void *mixer__load_file(const char *filename, int *size);
static void mixer__recalculate_source_gain(mixer_source *source);
static const char *mixer__ogg_init(mixer_source_info *info, void *data, int length, int ownsdata);
static void mixer__ogg_handler(mixer_event *e);


#define UNUSED(x)         ((void) (x))
#define CLAMP(x, a, b)    ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))
#define MIN(a, b)         ((a) < (b) ? (a) : (b))
#define MAX(a, b)         ((a) > (b) ? (a) : (b))

/* Binary Fixed Point */
#define FX_BITS           (12)
#define FX_UNIT           (1 << FX_BITS)
#define FX_MASK           (FX_UNIT - 1)
#define FX_FROM_FLOAT(f)  ((f) * FX_UNIT)
#define FX_LERP(a, b, p)  ((a) + ((((b) - (a)) * (p)) >> FX_BITS))

#define MIXER_BUFFER_SIZE       (512)
#define MIXER_BUFFER_MASK       (MIXER_BUFFER_SIZE - 1)

struct mixer_source
{
    mixer_source *next;               /* Next source in list */
    int16 buffer[MIXER_BUFFER_SIZE];  /* Internal buffer with raw stereo PCM */
    mixer_event_handler handler;       /* Event handler */
    void *udata;                      /* Stream's udata (from Mixer_SourceInfo) */
    int samplerate;                   /* Stream's native samplerate */
    int length;                       /* Stream's length in frames */
    int end;                          /* End index for the current play-through */
    int state;                        /* Current state (playing|paused|stopped) */
    int64 position;                   /* Current playhead position (fixed point) */
    int lgain, rgain;                 /* Left and right gain (fixed point) */
    int rate;                         /* Playback rate (fixed point) */
    int nextfill;                     /* Next frame idx where the buffer needs to be filled */
    int loop;                         /* Whether the source will loop when `end` is reached */
    int rewind;                       /* Whether the source will rewind before playing */
    int active;                       /* Whether the source is part of `sources` list */
    double gain;                      /* Gain set by `mixer_set_gain()` */
    double pan;                       /* Pan set by `mixer_set_pan()` */
};


/* Global Mixer State */
static struct
{
    const char *lasterror;            /* Last error message */
    mixer_event_handler lock;          /* Event handler for lock/unlock events */
    mixer_source *sources;            /* Linked list of active (playing) sources */
    int32 buffer[MIXER_BUFFER_SIZE];  /* Internal master buffer */
    int samplerate;                   /* Master samplerate */
    int gain;                         /* Master gain (fixed point) */
} mixer;


static void mixer__dummy_handler(mixer_event *event)
{
    UNUSED(event);
}


static void mixer__lock(void)
{
    mixer_event event;
    event.type = MIXER_EVENT_LOCK;
    mixer.lock(&event);
}


static void mixer__unlock(void)
{
    mixer_event event;
    event.type = MIXER_EVENT_UNLOCK;
    mixer.lock(&event);
}


const char *mixer_get_error(void)
{
    const char *res = mixer.lasterror;
    mixer.lasterror = NULL;
    return res;
}


static const char *mixer__error(const char *msg)
{
    mixer.lasterror = msg;
    return msg;
}


void mixer_init(int samplerate)
{
    mixer.samplerate = samplerate;
    mixer.lock = mixer__dummy_handler;
    mixer.sources = NULL;
    mixer.gain = FX_UNIT;
}


void mixer_set_lock(mixer_event_handler lock)
{
    mixer.lock = lock;
}


void mixer_set_master_gain(double gain)
{
    mixer.gain = FX_FROM_FLOAT(gain);
}


static void mixer__rewind_source(mixer_source *source)
{
    mixer_event event;
    event.type = MIXER_EVENT_REWIND;
    event.udata = source->udata;
    source->handler(&event);
    source->position = 0;
    source->rewind = 0;
    source->end = source->length;
    source->nextfill = 0;
}


static void mixer__fill_source_buffer(mixer_source *source, int offset, int length)
{
    mixer_event event;
    event.type = MIXER_EVENT_SAMPLES;
    event.udata = source->udata;
    event.buffer = source->buffer + offset;
    event.length = length;
    source->handler(&event);
}


static void mixer__process_source(mixer_source *source, int length)
{
    /* Rewind if flag is set */
    if (source->rewind)
    {
	mixer__rewind_source(source);
    }

    /* Don't process if not playing */
    if (source->state != MIXER_STATE_PLAYING)
    {
	return;
    }

    /* Process audio */
    int32 *destination = mixer.buffer;
    while (length > 0)
    {
	/* Get current frame */
	int starting_frame = source->position >> FX_BITS;

	/* Fill buffer if required */
	if (starting_frame + 3 >= source->nextfill)
	{
	    mixer__fill_source_buffer(source, (source->nextfill * 2) & MIXER_BUFFER_MASK, MIXER_BUFFER_SIZE / 2);
	    source->nextfill += MIXER_BUFFER_SIZE / 4;
	}

	/* Handle reaching the end of the playthrough */
	if (starting_frame >= source->end)
	{
	    /* As streams continiously fill the raw buffer in a loop we simply
	    ** increment the end idx by one length and continue reading from it for
	    ** another play-through */
	    source->end = starting_frame + source->length;
	    /* Set state and stop processing if we're not set to loop */
	    if (!source->loop)
	    {
		source->state = MIXER_STATE_STOPPED;
		break;
	    }
	}

	/* Work out how many frames we should process in the loop */
	int frames_to_process;
	frames_to_process = MIN(source->nextfill - 2, source->end) - starting_frame;
	frames_to_process = (frames_to_process << FX_BITS) / source->rate; /* Compensate for fixed precision rate difference */
	frames_to_process = MAX(frames_to_process, 1); /* Process at least 1 frame */
	frames_to_process = MIN(frames_to_process, length / 2); /* Do not process more frames than we have length left */
	length -= frames_to_process * 2; /* Remove frames from length (We process 2 at a time) */

	/* Add audio to master buffer */
	if (source->rate == FX_UNIT)
	{
	    /* Add audio to buffer -- basic */
	    int current_frame = starting_frame * 2;
	    for (int i = 0; i < frames_to_process; i++)
	    {
		destination[0] += (source->buffer[(current_frame    ) & MIXER_BUFFER_MASK] * source->lgain) >> FX_BITS;
		destination[1] += (source->buffer[(current_frame + 1) & MIXER_BUFFER_MASK] * source->rgain) >> FX_BITS;
		current_frame += 2;
		destination += 2;
	    }
	    source->position += frames_to_process * FX_UNIT;
	}
	else
	{
	    /* Add audio to buffer -- interpolated */
	    int lerp_frames[2];
	    for (int i = 0; i < frames_to_process; i++)
	    {
		int current_frame = (source->position >> FX_BITS) * 2;
		int pos = source->position & FX_MASK;

		lerp_frames[0] = source->buffer[(current_frame    ) & MIXER_BUFFER_MASK];
		lerp_frames[1] = source->buffer[(current_frame + 2) & MIXER_BUFFER_MASK];
		destination[0] += (FX_LERP(lerp_frames[0], lerp_frames[1], pos) * source->lgain) >> FX_BITS;
	
		current_frame++;

		lerp_frames[0] = source->buffer[(current_frame    ) & MIXER_BUFFER_MASK];
		lerp_frames[1] = source->buffer[(current_frame + 2) & MIXER_BUFFER_MASK];
		destination[1] += (FX_LERP(lerp_frames[0], lerp_frames[1], pos) * source->rgain) >> FX_BITS;

		source->position += source->rate;
		destination += 2;
	    }
	}
    }
}


void mixer_process(int16 *destination, int length)
{
    /* Process in chunks of MIXER_BUFFER_SIZE if `length` is larger than MIXER_BUFFER_SIZE */
    while (length > MIXER_BUFFER_SIZE)
    {
	mixer_process(destination, MIXER_BUFFER_SIZE);
	destination += MIXER_BUFFER_SIZE;
	length -= MIXER_BUFFER_SIZE;
    }

    /* Zeroset internal buffer */
    memset(mixer.buffer, 0, length * sizeof(mixer.buffer[0]));

    /* Process active sources */
    mixer__lock();
    mixer_source** source = &mixer.sources;
    while (*source)
    {
	mixer__process_source(*source, length);
	/* Remove source from list if it is no longer playing */
	if ((*source)->state != MIXER_STATE_PLAYING)
	{
	    (*source)->active = 0;
	    *source = (*source)->next;
	}
	else
	{
	    source = &(*source)->next;
	}
    }
    mixer__unlock();

    /* Copy internal buffer to destination and clip */
    for (int i = 0; i < length; i++)
    {
	int with_gain = (mixer.buffer[i] * mixer.gain) >> FX_BITS;
	destination[i] = CLAMP(with_gain, -32768, 32767);
    }
}


mixer_source *mixer_new_source(const mixer_source_info *info)
{
    mixer_source *source = (mixer_source *)calloc(1, sizeof(*source));
    if (!source)
    {
	mixer__error("allocation failed");
	return NULL;
    }
    source->handler = info->handler;
    source->length = info->length;
    source->samplerate = info->samplerate;
    source->udata = info->udata;
    mixer_set_gain(source, 1);
    mixer_set_pan(source, 0);
    mixer_set_pitch(source, 1);
    mixer_set_loop(source, 0);
    mixer_stop(source);
    return source;
}


static int mixer__check_header(void *data, int size, const char *str, int offset)
{
    int length = strlen(str);
    return (size >= offset + length) && !memcmp((char *) data + offset, str, length);
}


static mixer_source *mixer__new_source_from_mem(void *data, int size, int ownsdata)
{
    if (mixer__check_header(data, size, "OggS", 0))
    {
	mixer_source_info info;
	const char *err = mixer__ogg_init(&info, data, size, ownsdata);
	if (err)
	{
	    return NULL;
	}
	return mixer_new_source(&info);
    }

    mixer__error("unknown format or invalid data");
    return NULL;
}


static void *mixer__load_file(const char *filename, int *size)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
	return NULL;
    }

    /* Get size */
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    /* Malloc, read and return data */
    void *data = malloc(*size);
    if (!data)
    {
	fclose(file);
	return NULL;
    }
    int read_size = fread(data, 1, *size, file);
    fclose(file);
    if (read_size != *size)
    {
	free(data);
	return NULL;
    }

    return data;
}


mixer_source *mixer_new_source_from_file(const char *filename)
{
    /* Load file into memory */
    int size;
    void *data = mixer__load_file(filename, &size);
    if (!data)
    {
	mixer__error("could not load file");
	return NULL;
    }

    /* Try to load and return */
    mixer_source *source = mixer__new_source_from_mem(data, size, 1);
    if (!source)
    {
	free(data);
	return NULL;
    }

    return source;
}


mixer_source *mixer_new_source_from_mem(void *data, int size)
{
    return mixer__new_source_from_mem(data, size, 0);
}


void mixer_destroy_source(mixer_source *source)
{
    mixer__lock();
    if (source->active)
    {
	mixer_source **current_source = &mixer.sources;
	while (*current_source)
	{
	    if (*current_source == source)
	    {
		*current_source = source->next;
		break;
	    }
	}
    }
    mixer__unlock();
    mixer_event event;
    event.type = MIXER_EVENT_DESTROY;
    event.udata = source->udata;
    source->handler(&event);
    free(source);
}


double mixer_get_length(mixer_source *source)
{
    return source->length / (double) source->samplerate;
}


double mixer_get_position(mixer_source *source)
{
    return ((source->position >> FX_BITS) % source->length) / (double) source->samplerate;
}


int mixer_get_state(mixer_source *source)
{
    return source->state;
}


static void mixer__recalculate_source_gain(mixer_source *source)
{
    double pan = source->pan;
    double left = source->gain * (pan <= 0.0 ? 1.0 : 1.0 - pan);
    double right = source->gain * (pan >= 0.0 ? 1.0 : 1.0 + pan);
    source->lgain = FX_FROM_FLOAT(left);
    source->rgain = FX_FROM_FLOAT(right);
}


void mixer_set_gain(mixer_source *source, double gain)
{
    source->gain = gain;
    mixer__recalculate_source_gain(source);
}


void mixer_set_pan(mixer_source *source, double pan)
{
    source->pan = CLAMP(pan, -1.0, 1.0);
    mixer__recalculate_source_gain(source);
}


void mixer_set_pitch(mixer_source *source, double pitch)
{
    double rate = pitch > 0.0 ? source->samplerate / (double) mixer.samplerate * pitch : 0.001;
    source->rate = FX_FROM_FLOAT(rate);
}


void mixer_set_loop(mixer_source *source, int loop)
{
    source->loop = loop;
}


void mixer_play(mixer_source *source)
{
    mixer__lock();
    source->state = MIXER_STATE_PLAYING;
    if (!source->active)
    {
	source->active = 1;
	source->next = mixer.sources;
	mixer.sources = source;
    }
    mixer__unlock();
}


void mixer_pause(mixer_source *source)
{
    source->state = MIXER_STATE_PAUSED;
}


void mixer_stop(mixer_source *source)
{
    source->state = MIXER_STATE_STOPPED;
    source->rewind = 1;
}


typedef struct
{
    stb_vorbis *ogg;
    void *data;
} ogg_stream;


static void mixer__ogg_handler(mixer_event *event)
{
    ogg_stream *stream = (ogg_stream *)event->udata;
    switch (event->type)
    {
	case MIXER_EVENT_DESTROY:
	{
	    stb_vorbis_close(stream->ogg);
	    free(stream->data);
	    free(stream);
	} break;

	case MIXER_EVENT_SAMPLES:
	{
	    int len = event->length;
	    int16 *buf = event->buffer;
	    int sample_count;
	fill:
	    sample_count = stb_vorbis_get_samples_short_interleaved(stream->ogg, 2, buf, len);
	    sample_count *= 2;
	    /* rewind and fill remaining buffer if we reached the end of the ogg
	    ** before filling it */
	    if (len != sample_count)
	    {
		stb_vorbis_seek_start(stream->ogg);
		buf += sample_count;
		len -= sample_count;
		goto fill;
	    }
	} break;

	case MIXER_EVENT_REWIND:
	{
	    stb_vorbis_seek_start(stream->ogg);
	} break;
    }
}


static const char *mixer__ogg_init(mixer_source_info *info, void *data, int length, int ownsdata)
{
    int err;
    stb_vorbis *ogg = stb_vorbis_open_memory((const unsigned char *)data, length, &err, NULL);
    if (!ogg)
    {
	return mixer__error("invalid ogg data");
    }

    ogg_stream *stream = (ogg_stream *)calloc(1, sizeof(*stream));
    if (!stream)
    {
	stb_vorbis_close(ogg);
	return mixer__error("allocation failed");
    }

    stream->ogg = ogg;
    if (ownsdata)
    {
	stream->data = data;
    }

    stb_vorbis_info ogginfo = stb_vorbis_get_info(ogg);

    info->udata = stream;
    info->handler = mixer__ogg_handler;
    info->samplerate = ogginfo.sample_rate;
    info->length = stb_vorbis_stream_length_in_samples(ogg);

    /* Return NULL (no error) for success */
    return NULL;
}
