#include "nerd_mixer.h"
#include "stb_vorbis.h"

struct ogg_stream
{
    stb_vorbis *ogg;
    void *data;
};

static void
mixer__dummy_handler(struct mixer_event *event)
{
    (void)event;
}

static void
mixer__lock(struct mixer *mixer)
{
    struct mixer_event event;
    event.type = MIXER_EVENT_LOCK;
    event.udata = &(mixer->audio_device);
    mixer->lock(&event);
}

static void
mixer__unlock(struct mixer *mixer)
{
    struct mixer_event event;
    event.type = MIXER_EVENT_UNLOCK;
    event.udata = &(mixer->audio_device);
    mixer->lock(&event);
}

static const char *
mixer__error(struct mixer *mixer, const char *msg)
{
    mixer->lasterror = msg;
    return msg;
}

static void
mixer__rewind_source(struct mixer_source *source)
{
    struct mixer_event event;
    event.type = MIXER_EVENT_REWIND;
    event.udata = source->udata;
    source->handler(&event);
    source->position = 0;
    source->rewind = 0;
    source->end = source->length;
    source->nextfill = 0;
}

static void
mixer__fill_source_buffer(struct mixer_source *source, s32 offset, u32 length)
{
    struct mixer_event event;
    event.type = MIXER_EVENT_SAMPLES;
    event.udata = source->udata;
    event.buffer = source->buffer + offset;
    event.length = length;
    source->handler(&event);
}

static void
mixer__process_source(struct mixer *mixer, struct mixer_source *s, s32 length)
{
    s32 *dest;
    s32 lerp_frames[2];
    s32 i, pos, offset, lerp;
    s32 num_frames, beg_frame, cur_frame, cur_buf;

    /* Rewind if flag is set */
    if (s->rewind)
    {
        mixer__rewind_source(s);
    }

    /* Don't process if not playing */
    if (s->state != MIXER_STATE_PLAYING)
    {
        return;
    }

    /* Process audio */
    dest = mixer->buffer;
    while (length > 0)
    {
        /* Get current frame */
        beg_frame = s->position >> FX_BITS;

        /* Fill buffer if required */
        if (beg_frame + 3 >= s->nextfill)
        {
            offset = (s->nextfill * 2) & MIXER_BUFFER_MASK;
            mixer__fill_source_buffer(s, offset, MIXER_BUFFER_SIZE / 2);

            s->nextfill += MIXER_BUFFER_SIZE / 4;
        }

        /* Handle reaching the end of the playthrough */
        if (beg_frame >= s->end)
        {
            //  As streams continiously fill the raw buffer in a loop we simply
            //  increment the end idx by one length and continue reading from it
            //  for another play-through
            s->end = beg_frame + s->length;

            /* Set state and stop processing if we're not set to loop */
            if (!s->loop)
            {
                s->state = MIXER_STATE_STOPPED;
                break;
            }
        }

        /* Work out how many frames we should process in the loop */
        num_frames = math_min(s->nextfill - 2, s->end) - beg_frame;

        /* Compensate for fixed precision rate difference */
        num_frames = (num_frames << FX_BITS) / s->rate;

        /* Process at least 1 frame */
        num_frames = math_max(num_frames, 1);

        /* Do not process more frames than we have length left */
        num_frames = math_min(num_frames, length / 2);

        /* Remove frames from length (We process 2 at a time) */
        length -= num_frames * 2;

        /* Add audio to master buffer */
        if (s->rate == FX_UNIT)
        {
            /* Add audio to buffer -- basic */
            cur_frame = beg_frame * 2;
            for (i = 0; i < num_frames; i++)
            {
                cur_buf = s->buffer[cur_frame & MIXER_BUFFER_MASK];
                dest[0] += (cur_buf * s->lgain) >> FX_BITS;

                cur_buf = s->buffer[(cur_frame + 1) & MIXER_BUFFER_MASK];
                dest[1] += (cur_buf * s->rgain) >> FX_BITS;

                cur_frame += 2;
                dest += 2;
            }
            s->position += num_frames * FX_UNIT;
        }
        else
        {
            /* Add audio to buffer -- interpolated */
            for (i = 0; i < num_frames; i++)
            {
                cur_frame = (s->position >> FX_BITS) * 2;
                pos = s->position & FX_MASK;

                lerp_frames[0] = s->buffer[cur_frame & MIXER_BUFFER_MASK];
                lerp_frames[1] = s->buffer[(cur_frame + 2) & MIXER_BUFFER_MASK];

                lerp = FX_LERP(lerp_frames[0], lerp_frames[1], pos);
                dest[0] += (lerp * s->lgain) >> FX_BITS;

                cur_frame++;

                lerp_frames[0] = s->buffer[cur_frame & MIXER_BUFFER_MASK];
                lerp_frames[1] = s->buffer[(cur_frame + 2) & MIXER_BUFFER_MASK];

                lerp = FX_LERP(lerp_frames[0], lerp_frames[1], pos);
                dest[1] += (lerp * s->rgain) >> FX_BITS;

                s->position += s->rate;
                dest += 2;
            }
        }
    }
}

static bool
mixer__check_header(void *data, s32 size, const char *str, s32 offset)
{
    s32 length = strlen(str);

    if (size < (offset + length))
    {
        return 0;
    }

    return !memcmp((char *)data + offset, str, length);
}

static void
mixer__recalculate_source_gain(struct mixer_source *s)
{
    f64 pan = s->pan;
    f64 left = s->gain * (pan <= 0.0 ? 1.0 : 1.0 - pan);
    f64 right = s->gain * (pan >= 0.0 ? 1.0 : 1.0 + pan);
    s->lgain = FX_FROM_FLOAT(left);
    s->rgain = FX_FROM_FLOAT(right);
}

static void
mixer__ogg_handler(struct mixer_event *event)
{
    s16 *buf;
    s32 len, num_sample;
    struct ogg_stream *stream = (struct ogg_stream *)event->udata;

    switch (event->type)
    {
        case MIXER_EVENT_DESTROY:
        {
            stb_vorbis_close(stream->ogg);
            free(stream->data);
            free(stream);
            break;
        }
        case MIXER_EVENT_SAMPLES:
        {
            len = event->length;
            buf = event->buffer;
        fill:
            num_sample = stb_vorbis_get_samples_short_interleaved(stream->ogg,
                                                                  2,
                                                                  buf,
                                                                  len);
            num_sample *= 2;

            // rewind and fill remaining buffer if we reached the end of the ogg
            // before filling it
            if (len != num_sample)
            {
                stb_vorbis_seek_start(stream->ogg);
                buf += num_sample;
                len -= num_sample;
                goto fill;
            }
            break;
        }
        case MIXER_EVENT_REWIND:
        {
            stb_vorbis_seek_start(stream->ogg);
            break;
        }
    }
}

static const char *
mixer__ogg_init(struct mixer *mixer,
                struct mixer_source_info *info,
                void *data,
                s32 length,
                s32 ownsdata)
{
    s32 err;
    stb_vorbis *ogg;
    struct ogg_stream *stream;

    ogg = stb_vorbis_open_memory(data, length, &err, NULL);

    if (!ogg)
    {
        return mixer__error(mixer, "invalid ogg data");
    }

    stream = calloc(1, sizeof(*stream));
    if (!stream)
    {
        stb_vorbis_close(ogg);
        return mixer__error(mixer, "allocation failed");
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

static struct mixer_source *
mixer__new_source_from_mem(struct mixer *mixer,
                           void *data,
                           s32 size,
                           bool ownsdata)
{
    struct mixer_source_info info;
    const char *err;

    if (mixer__check_header(data, size, "OggS", 0))
    {
        err = mixer__ogg_init(mixer, &info, data, size, ownsdata);
        if (err)
        {
            return NULL;
        }
        return mixer_new_source(mixer, &info);
    }

    mixer__error(mixer, "unknown format or invalid data");
    return NULL;
}

const char *
mixer_get_error(struct mixer *mixer)
{
    const char *res = mixer->lasterror;
    mixer->lasterror = NULL;
    return res;
}

struct mixer *
mixer_new()
{
    struct mixer *m = malloc(sizeof(struct mixer));
    memset(m, 0, sizeof(struct mixer));
    return m;
}

void
mixer_init(struct mixer *mixer, s32 samplerate)
{
    mixer->lasterror = NULL;
    mixer->samplerate = samplerate;
    mixer->lock = mixer__dummy_handler;
    mixer->sources = NULL;
    mixer->gain = FX_UNIT;
}

void
mixer_set_lock(struct mixer *mixer, mixer_event_handler lock)
{
    mixer->lock = lock;
}

void
mixer_set_master_gain(struct mixer *mixer, f64 gain)
{
    mixer->gain = FX_FROM_FLOAT(gain);
}

void
mixer_process(struct mixer *mixer, s16 *dest, s32 length)
{
    struct mixer_source **source;
    s32 i, with_gain;

    // Process in chunks of MIXER_BUFFER_SIZE if `length` is larger than
    // MIXER_BUFFER_SIZE
    while (length > MIXER_BUFFER_SIZE)
    {
        mixer_process(mixer, dest, MIXER_BUFFER_SIZE);
        dest += MIXER_BUFFER_SIZE;
        length -= MIXER_BUFFER_SIZE;
    }

    /* Zeroset internal buffer */
    memset(mixer->buffer, 0, length * sizeof(mixer->buffer[0]));

    /* Process active sources */
    mixer__lock(mixer);
    source = &mixer->sources;
    while (*source)
    {
        mixer__process_source(mixer, *source, length);
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
    mixer__unlock(mixer);

    /* Copy internal buffer to destination and clip */
    for (i = 0; i < length; i++)
    {
        with_gain = (mixer->buffer[i] * mixer->gain) >> FX_BITS;
        dest[i] = math_clamp(with_gain, -32768, 32767);
    }
}

struct mixer_source *
mixer_new_source(struct mixer *mixer, const struct mixer_source_info *info)
{
    struct mixer_source *source = calloc(1, sizeof(*source));
    if (!source)
    {
        mixer__error(mixer, "allocation failed");
        return NULL;
    }

    source->handler = info->handler;
    source->length = info->length;
    source->samplerate = info->samplerate;
    source->udata = info->udata;

    mixer_set_gain(source, 1);
    mixer_set_pan(source, 0);
    mixer_set_pitch(mixer, source, 1);
    mixer_set_loop(source, 0);
    mixer_stop(source);

    return source;
}

struct mixer_source *
mixer_new_source_from_file(struct mixer *mixer, const char *filename)
{
    struct mixer_source *source;
    u32 len;
    void *data = file_load(filename, &len);  // Load file into memory

    if (!data)
    {
        mixer__error(mixer, "could not load file");
        return NULL;
    }

    /* Try to load and return */
    source = mixer__new_source_from_mem(mixer, data, len, 1);
    if (!source)
    {
        free(data);
        return NULL;
    }

    return source;
}

struct mixer_source *
mixer_new_source_from_mem(struct mixer *mixer, void *data, s32 size)
{
    return mixer__new_source_from_mem(mixer, data, size, 0);
}

void
mixer_destroy_source(struct mixer *mixer, struct mixer_source *source)
{
    struct mixer_source **current_source;
    struct mixer_event event;

    mixer__lock(mixer);
    if (source->active)
    {
        current_source = &mixer->sources;
        while (*current_source)
        {
            if (*current_source == source)
            {
                *current_source = source->next;
                break;
            }
        }
    }

    mixer__unlock(mixer);
    event.type = MIXER_EVENT_DESTROY;
    event.udata = source->udata;
    source->handler(&event);
    free(source);
}

f64
mixer_get_length(struct mixer_source *source)
{
    return source->length / (f64)source->samplerate;
}

f64
mixer_get_position(struct mixer_source *s)
{
    return ((s->position >> FX_BITS) % s->length) / (f64)s->samplerate;
}

s32
mixer_get_state(struct mixer_source *source)
{
    return source->state;
}

void
mixer_set_gain(struct mixer_source *source, f64 gain)
{
    source->gain = gain;
    mixer__recalculate_source_gain(source);
}

void
mixer_set_pan(struct mixer_source *source, f64 pan)
{
    source->pan = math_clamp(pan, -1.0, 1.0);
    mixer__recalculate_source_gain(source);
}

void
mixer_set_pitch(struct mixer *mixer, struct mixer_source *source, f64 pitch)
{
    f64 rate = 0.001;
    if (pitch > 0)
    {
        rate = source->samplerate / (f64)mixer->samplerate * pitch;
    }

    source->rate = FX_FROM_FLOAT(rate);
}

void
mixer_set_loop(struct mixer_source *source, bool loop)
{
    source->loop = loop;
}

void
mixer_play(struct mixer *mixer, struct mixer_source *source)
{
    mixer__lock(mixer);
    source->state = MIXER_STATE_PLAYING;
    if (!source->active)
    {
        source->active = 1;
        source->next = mixer->sources;
        mixer->sources = source;
    }
    mixer__unlock(mixer);
}

void
mixer_pause(struct mixer_source *source)
{
    source->state = MIXER_STATE_PAUSED;
}

void
mixer_stop(struct mixer_source *source)
{
    source->state = MIXER_STATE_STOPPED;
    source->rewind = 1;
}