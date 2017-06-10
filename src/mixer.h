#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "common.h"
#include "stb_vorbis.c"

typedef struct MixerSource MixerSource;

typedef struct
{
  int type;
  void* udata;
  const char* msg;
  int16* buffer;
  int length;
} MixerEvent;

typedef void (*MixerEventHandler)(MixerEvent* e);

typedef struct
{
  MixerEventHandler handler;
  void* udata;
  int samplerate;
  int length;
} MixerSourceInfo;

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


const char* MixerGetError(void);
void MixerInit(int samplerate);
void MixersetLock(MixerEventHandler lock);
void MixerSetMasterGain(double gain);
void MixerProcess(int16* destination, int length);

MixerSource* MixerNewSource(const MixerSourceInfo* info);
MixerSource* MixerNewSourceFromFile(const char* filename);
MixerSource* MixerNewSourceFromMem(void* data, int size);
void MixerDestroySource(MixerSource* source);
double MixerGetLength(MixerSource* source);
double MixerGetPosition(MixerSource* source);
int MixerGetState(MixerSource* source);
void MixerSetGain(MixerSource* source, double gain);
void MixerSetPan(MixerSource* source, double pan);
void MixerSetPitch(MixerSource* source, double pitch);
void MixerSetLoop(MixerSource* source, int loop);
void MixerPlay(MixerSource* source);
void MixerPause(MixerSource* source);
void MixerStop(MixerSource* source);

static const char* Error(const char* msg);
static void DummyHandler(MixerEvent* event);
static void Lock(void);
static void Unlock(void);

static void RewindSource(MixerSource* source);
static void FillSourceBuffer(MixerSource* source, int offset, int length);
static void ProcessSource(MixerSource* source, int length);
static int CheckHeader(void* data, int size, const char* str, int offset);
static MixerSource* NewSourceFromMem(void* data, int size, int ownsdata);
static void* LoadFile(const char* filename, int *size);
static void RecalculateSourceGain(MixerSource *source);
static const char* OggInit(MixerSourceInfo* info, void* data, int length, int ownsdata);
static void OggHandler(MixerEvent* e);


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

struct MixerSource
{
  MixerSource* next;                /* Next source in list */
  int16 buffer[MIXER_BUFFER_SIZE];  /* Internal buffer with raw stereo PCM */
  MixerEventHandler handler;        /* Event handler */
  void* udata;                      /* Stream's udata (from MixerSourceInfo) */
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
  double gain;                      /* Gain set by `MixerSetGain()` */
  double pan;                       /* Pan set by `MixerSetPan()` */
};


/* Global Mixer State */
static struct
{
  const char* lasterror;            /* Last error message */
  MixerEventHandler lock;           /* Event handler for lock/unlock events */
  MixerSource* sources;             /* Linked list of active (playing) sources */
  int32 buffer[MIXER_BUFFER_SIZE];  /* Internal master buffer */
  int samplerate;                   /* Master samplerate */
  int gain;                         /* Master gain (fixed point) */
} Mixer;


static void DummyHandler(MixerEvent* event)
{
  UNUSED(event);
}


static void Lock(void)
{
  MixerEvent event;
  event.type = MIXER_EVENT_LOCK;
  Mixer.lock(&event);
}


static void Unlock(void)
{
  MixerEvent event;
  event.type = MIXER_EVENT_UNLOCK;
  Mixer.lock(&event);
}


const char* MixerGetError(void)
{
  const char* res = Mixer.lasterror;
  Mixer.lasterror = NULL;
  return res;
}


static const char* Error(const char* msg)
{
  Mixer.lasterror = msg;
  return msg;
}


void MixerInit(int samplerate)
{
  Mixer.samplerate = samplerate;
  Mixer.lock = DummyHandler;
  Mixer.sources = NULL;
  Mixer.gain = FX_UNIT;
}


void MixerSetLock(MixerEventHandler lock)
{
  Mixer.lock = lock;
}


void MixerSetMasterGain(double gain)
{
  Mixer.gain = FX_FROM_FLOAT(gain);
}


static void RewindSource(MixerSource* source)
{
  MixerEvent event;
  event.type = MIXER_EVENT_REWIND;
  event.udata = source->udata;
  source->handler(&event);
  source->position = 0;
  source->rewind = 0;
  source->end = source->length;
  source->nextfill = 0;
}


static void FillSourceBuffer(MixerSource* source, int offset, int length)
{
  MixerEvent event;
  event.type = MIXER_EVENT_SAMPLES;
  event.udata = source->udata;
  event.buffer = source->buffer + offset;
  event.length = length;
  source->handler(&event);
}


static void ProcessSource(MixerSource* source, int length)
{
  /* Rewind if flag is set */
  if (source->rewind)
  {
    RewindSource(source);
  }

  /* Don't process if not playing */
  if (source->state != MIXER_STATE_PLAYING)
  {
    return;
  }

  /* Process audio */
  int32* destination = Mixer.buffer;
  while (length > 0)
  {
    /* Get current frame */
    int startingFrame = source->position >> FX_BITS;

    /* Fill buffer if required */
    if (startingFrame + 3 >= source->nextfill)
    {
      FillSourceBuffer(source, (source->nextfill * 2) & MIXER_BUFFER_MASK, MIXER_BUFFER_SIZE / 2);
      source->nextfill += MIXER_BUFFER_SIZE / 4;
    }

    /* Handle reaching the end of the playthrough */
    if (startingFrame >= source->end)
    {
      /* As streams continiously fill the raw buffer in a loop we simply
      ** increment the end idx by one length and continue reading from it for
      ** another play-through */
      source->end = startingFrame + source->length;
      /* Set state and stop processing if we're not set to loop */
      if (!source->loop)
      {
	source->state = MIXER_STATE_STOPPED;
	break;
      }
    }

    /* Work out how many frames we should process in the loop */
    int framesToProcess;
    framesToProcess = MIN(source->nextfill - 2, source->end) - startingFrame;
    framesToProcess = (framesToProcess << FX_BITS) / source->rate; /* Compensate for fixed precision rate difference */
    framesToProcess = MAX(framesToProcess, 1); /* Process at least 1 frame */
    framesToProcess = MIN(framesToProcess, length / 2); /* Do not process more frames than we have length left */
    length -= framesToProcess * 2; /* Remove frames from length (We process 2 at a time) */

    /* Add audio to master buffer */
    if (source->rate == FX_UNIT)
    {
      /* Add audio to buffer -- basic */
      int currentFrame = startingFrame * 2;
      for (int i = 0; i < framesToProcess; i++)
      {
	destination[0] += (source->buffer[(currentFrame    ) & MIXER_BUFFER_MASK] * source->lgain) >> FX_BITS;
	destination[1] += (source->buffer[(currentFrame + 1) & MIXER_BUFFER_MASK] * source->rgain) >> FX_BITS;
	currentFrame += 2;
	destination += 2;
      }
      source->position += framesToProcess * FX_UNIT;
    }
    else
    {
      /* Add audio to buffer -- interpolated */
      int lerpFrames[2];
      for (int i = 0; i < framesToProcess; i++)
      {
	int currentFrame = (source->position >> FX_BITS) * 2;
	int pos = source->position & FX_MASK;

	lerpFrames[0] = source->buffer[(currentFrame    ) & MIXER_BUFFER_MASK];
	lerpFrames[1] = source->buffer[(currentFrame + 2) & MIXER_BUFFER_MASK];
	destination[0] += (FX_LERP(lerpFrames[0], lerpFrames[1], pos) * source->lgain) >> FX_BITS;
	
	currentFrame++;

	lerpFrames[0] = source->buffer[(currentFrame    ) & MIXER_BUFFER_MASK];
	lerpFrames[1] = source->buffer[(currentFrame + 2) & MIXER_BUFFER_MASK];
	destination[1] += (FX_LERP(lerpFrames[0], lerpFrames[1], pos) * source->rgain) >> FX_BITS;

	source->position += source->rate;
	destination += 2;
      }
    }
  }
}


void MixerProcess(int16* destination, int length)
{
  /* Process in chunks of MIXER_BUFFER_SIZE if `length` is larger than MIXER_BUFFER_SIZE */
  while (length > MIXER_BUFFER_SIZE)
  {
    MixerProcess(destination, MIXER_BUFFER_SIZE);
    destination += MIXER_BUFFER_SIZE;
    length -= MIXER_BUFFER_SIZE;
  }

  /* Zeroset internal buffer */
  memset(Mixer.buffer, 0, length * sizeof(Mixer.buffer[0]));

  /* Process active sources */
  Lock();
  MixerSource** source = &Mixer.sources;
  while (*source)
  {
    ProcessSource(*source, length);
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
  Unlock();

  /* Copy internal buffer to destination and clip */
  for (int i = 0; i < length; i++)
  {
    int withGain = (Mixer.buffer[i] * Mixer.gain) >> FX_BITS;
    destination[i] = CLAMP(withGain, -32768, 32767);
  }
}


MixerSource* MixerNewSource(const MixerSourceInfo* info)
{
  MixerSource *source = (MixerSource*)calloc(1, sizeof(*source));
  if (!source)
  {
    Error("allocation failed");
    return NULL;
  }
  source->handler = info->handler;
  source->length = info->length;
  source->samplerate = info->samplerate;
  source->udata = info->udata;
  MixerSetGain(source, 1);
  MixerSetPan(source, 0);
  MixerSetPitch(source, 1);
  MixerSetLoop(source, 0);
  MixerStop(source);
  return source;
}


static int CheckHeader(void* data, int size, const char* str, int offset)
{
  int length = strlen(str);
  return (size >= offset + length) && !memcmp((char*) data + offset, str, length);
}


static MixerSource* NewSourceFromMem(void* data, int size, int ownsdata)
{
  if (CheckHeader(data, size, "OggS", 0))
  {
    MixerSourceInfo info;
    const char* err = OggInit(&info, data, size, ownsdata);
    if (err)
    {
      return NULL;
    }
    return MixerNewSource(&info);
  }

  Error("unknown format or invalid data");
  return NULL;
}


static void* LoadFile(const char* filename, int *size)
{
  FILE* file = fopen(filename, "rb");
  if (!file)
  {
    return NULL;
  }

  /* Get size */
  fseek(file, 0, SEEK_END);
  *size = ftell(file);
  rewind(file);

  /* Malloc, read and return data */
  void* data = malloc(*size);
  if (!data)
  {
    fclose(file);
    return NULL;
  }
  int readSize = fread(data, 1, *size, file);
  fclose(file);
  if (readSize != *size)
  {
    free(data);
    return NULL;
  }

  return data;
}


MixerSource* MixerNewSourceFromFile(const char* filename)
{
  /* Load file into memory */
  int size;
  void* data = LoadFile(filename, &size);
  if (!data)
  {
    Error("could not load file");
    return NULL;
  }

  /* Try to load and return */
  MixerSource* source = NewSourceFromMem(data, size, 1);
  if (!source)
  {
    free(data);
    return NULL;
  }

  return source;
}


MixerSource* MixerNewSourceFromMem(void* data, int size)
{
  return NewSourceFromMem(data, size, 0);
}


void MixerDestroySource(MixerSource *source)
{
  Lock();
  if (source->active)
  {
    MixerSource **currentSource = &Mixer.sources;
    while (*currentSource)
    {
      if (*currentSource == source)
      {
	*currentSource = source->next;
	break;
      }
    }
  }
  Unlock();
  MixerEvent event;
  event.type = MIXER_EVENT_DESTROY;
  event.udata = source->udata;
  source->handler(&event);
  free(source);
}


double MixerGetLength(MixerSource *source)
{
  return source->length / (double) source->samplerate;
}


double MixerGetPosition(MixerSource *source)
{
  return ((source->position >> FX_BITS) % source->length) / (double) source->samplerate;
}


int MixerGetState(MixerSource *source)
{
  return source->state;
}


static void RecalculateSourceGain(MixerSource *source)
{
  double pan = source->pan;
  double left = source->gain * (pan <= 0.0 ? 1.0 : 1.0 - pan);
  double right = source->gain * (pan >= 0.0 ? 1.0 : 1.0 + pan);
  source->lgain = FX_FROM_FLOAT(left);
  source->rgain = FX_FROM_FLOAT(right);
}


void MixerSetGain(MixerSource *source, double gain)
{
  source->gain = gain;
  RecalculateSourceGain(source);
}


void MixerSetPan(MixerSource *source, double pan)
{
  source->pan = CLAMP(pan, -1.0, 1.0);
  RecalculateSourceGain(source);
}


void MixerSetPitch(MixerSource *source, double pitch)
{
  double rate = pitch > 0.0 ? source->samplerate / (double) Mixer.samplerate * pitch : 0.001;
  source->rate = FX_FROM_FLOAT(rate);
}


void MixerSetLoop(MixerSource *source, int loop)
{
  source->loop = loop;
}


void MixerPlay(MixerSource *source)
{
  Lock();
  source->state = MIXER_STATE_PLAYING;
  if (!source->active)
  {
    source->active = 1;
    source->next = Mixer.sources;
    Mixer.sources = source;
  }
  Unlock();
}


void MixerPause(MixerSource *source)
{
  source->state = MIXER_STATE_PAUSED;
}


void MixerStop(MixerSource *source)
{
  source->state = MIXER_STATE_STOPPED;
  source->rewind = 1;
}


typedef struct
{
  stb_vorbis *ogg;
  void* data;
} OggStream;


static void OggHandler(MixerEvent* event)
{
  OggStream *stream = (OggStream*)event->udata;
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
      int16* buf = event->buffer;
      int sampleCount;
    fill:
      sampleCount = stb_vorbis_get_samples_short_interleaved(stream->ogg, 2, buf, len);
      sampleCount *= 2;
      /* rewind and fill remaining buffer if we reached the end of the ogg
      ** before filling it */
      if (len != sampleCount)
      {
	stb_vorbis_seek_start(stream->ogg);
	buf += sampleCount;
	len -= sampleCount;
	goto fill;
      }
    } break;

    case MIXER_EVENT_REWIND:
    {
      stb_vorbis_seek_start(stream->ogg);
    } break;
  }
}


static const char* OggInit(MixerSourceInfo* info, void* data, int length, int ownsdata)
{
  int err;
  stb_vorbis* ogg = stb_vorbis_open_memory((const unsigned char*)data, length, &err, NULL);
  if (!ogg)
  {
    return Error("invalid ogg data");
  }

  OggStream* stream = (OggStream*)calloc(1, sizeof(*stream));
  if (!stream)
  {
    stb_vorbis_close(ogg);
    return Error("allocation failed");
  }

  stream->ogg = ogg;
  if (ownsdata)
  {
    stream->data = data;
  }

  stb_vorbis_info ogginfo = stb_vorbis_get_info(ogg);

  info->udata = stream;
  info->handler = OggHandler;
  info->samplerate = ogginfo.sample_rate;
  info->length = stb_vorbis_stream_length_in_samples(ogg);

  /* Return NULL (no error) for success */
  return NULL;
}
