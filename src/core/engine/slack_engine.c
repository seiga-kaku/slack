#include "slack_engine.h"

#include <stddef.h>
#include <string.h>

#define SLACK_DEFAULT_SAMPLE_RATE 44100.0
#define SLACK_DEFAULT_TEMPO_BPM 120.0

static bool slack_copy_string(char *destination, size_t capacity, const char *source) {
  size_t source_length;

  if (destination == NULL || source == NULL || capacity == 0) {
    return false;
  }

  source_length = strlen(source);
  if (source_length >= capacity) {
    return false;
  }

  memcpy(destination, source, source_length + 1);
  return true;
}

void slack_engine_init(SlackEngine *engine, double sample_rate) {
  if (engine == NULL) {
    return;
  }

  memset(engine, 0, sizeof(*engine));

  slack_copy_string(engine->project.title, sizeof(engine->project.title), "Untitled");
  slack_copy_string(engine->project.artist, sizeof(engine->project.artist), "Unknown Artist");
  engine->project.tempo_bpm = SLACK_DEFAULT_TEMPO_BPM;
  engine->project.time_signature.beats_per_bar = 4;
  engine->project.time_signature.beat_unit = 4;

  engine->transport.sample_rate =
      sample_rate > 0.0 ? sample_rate : SLACK_DEFAULT_SAMPLE_RATE;
}

bool slack_engine_set_title(SlackEngine *engine, const char *title) {
  if (engine == NULL) {
    return false;
  }

  return slack_copy_string(engine->project.title, sizeof(engine->project.title), title);
}

bool slack_engine_set_artist(SlackEngine *engine, const char *artist) {
  if (engine == NULL) {
    return false;
  }

  return slack_copy_string(engine->project.artist, sizeof(engine->project.artist), artist);
}

bool slack_engine_set_tempo(SlackEngine *engine, double tempo_bpm) {
  if (engine == NULL || tempo_bpm <= 0.0) {
    return false;
  }

  engine->project.tempo_bpm = tempo_bpm;
  return true;
}

void slack_engine_start(SlackEngine *engine) {
  if (engine == NULL) {
    return;
  }

  engine->transport.playing = true;
}

void slack_engine_stop(SlackEngine *engine) {
  if (engine == NULL) {
    return;
  }

  engine->transport.playing = false;
}

void slack_engine_advance(SlackEngine *engine, uint32_t frames) {
  uint64_t next_frame_position;

  if (engine == NULL || !engine->transport.playing) {
    return;
  }

  next_frame_position = engine->transport.frame_position + frames;
  if (next_frame_position < engine->transport.frame_position) {
    engine->transport.frame_position = UINT64_MAX;
    return;
  }

  engine->transport.frame_position = next_frame_position;
}

double slack_engine_position_beats(const SlackEngine *engine) {
  double samples_per_beat;

  if (engine == NULL || engine->transport.sample_rate <= 0.0 || engine->project.tempo_bpm <= 0.0) {
    return 0.0;
  }

  samples_per_beat = (60.0 * engine->transport.sample_rate) / engine->project.tempo_bpm;
  if (samples_per_beat <= 0.0) {
    return 0.0;
  }

  return (double)engine->transport.frame_position / samples_per_beat;
}
