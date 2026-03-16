#ifndef SLACK_CORE_ENGINE_SLACK_ENGINE_H
#define SLACK_CORE_ENGINE_SLACK_ENGINE_H

#include <stdbool.h>
#include <stdint.h>

#define SLACK_PROJECT_TITLE_CAPACITY 64
#define SLACK_PROJECT_ARTIST_CAPACITY 64

typedef struct SlackTimeSignature {
  uint16_t beats_per_bar;
  uint16_t beat_unit;
} SlackTimeSignature;

typedef struct SlackProject {
  char title[SLACK_PROJECT_TITLE_CAPACITY];
  char artist[SLACK_PROJECT_ARTIST_CAPACITY];
  double tempo_bpm;
  SlackTimeSignature time_signature;
} SlackProject;

typedef struct SlackTransport {
  double sample_rate;
  uint64_t frame_position;
  bool playing;
} SlackTransport;

typedef struct SlackEngine {
  SlackProject project;
  SlackTransport transport;
} SlackEngine;

void slack_engine_init(SlackEngine *engine, double sample_rate);
bool slack_engine_set_title(SlackEngine *engine, const char *title);
bool slack_engine_set_artist(SlackEngine *engine, const char *artist);
bool slack_engine_set_tempo(SlackEngine *engine, double tempo_bpm);
void slack_engine_start(SlackEngine *engine);
void slack_engine_stop(SlackEngine *engine);
void slack_engine_advance(SlackEngine *engine, uint32_t frames);
double slack_engine_position_beats(const SlackEngine *engine);

#endif
