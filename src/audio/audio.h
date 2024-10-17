
#ifndef __AUDIO_H
#define __AUDIO_H

#include "../base/base.h"
#include "patch/patch.h"
#include "playback/pattern.h"
#include "playback/track.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_SEQUENCES 128
#define MAX_PATTERNS  64
#define MAX_PATCHES   32
#define MAX_TRACKS    MAX_PATTERN_COLUMNS

// #pragma pack(push, 16)
typedef struct {
  // state
  uint16_t bpm;
  uint8_t lpb;
  float volume;
#ifdef PLAYER_ONLY
  uint32_t durationMs;
#endif
  double sampleRate;
  double oneOverSampleRate;
  double time;
  double startTime;
  uint32_t tick;
  uint32_t renderOffset;
  //    uint16_t row;
  uint8_t row;
  uint8_t sequence;
  bool playback;
#ifndef PLAYER_ONLY
  uint8_t lastSequence;
  bool repeatPattern;
  bool clip;
  double clipTime;
  double infoTime;
#endif
  // patches
  uint8_t patchCount;
  Patch **patches;
#ifndef PLAYER_ONLY
  uint16_t activeVoices;
  uint16_t activePatches;
#endif
  // events
  uint8_t sequenceCount;
  uint8_t *sequences;
  uint8_t trackCount;
  Track *tracks;
  Pattern **patterns;
} AudioState;

extern AudioState audioState;

// tracks
JS_EXPORT void audioSetTrackCount(uint8_t trackCount);
JS_EXPORT void audioSetTrackActive(uint8_t track, bool active);

// sequences
JS_EXPORT void audioSetSequenceCount(uint8_t sequenceCount);

JS_EXPORT void audioSetSequence(uint8_t sequence, uint8_t pattern);

// patterns
JS_EXPORT void audioSetPattern(uint8_t pattern, uint8_t column, uint16_t row,
                               uint8_t note, uint8_t patch, uint8_t volume,
                               uint8_t fx1type, uint8_t fx1value,
                               uint8_t fx2type, uint8_t fx2value);

#ifndef PLAYER_ONLY

JS_EXPORT void audioInsertPatternRow(uint8_t pattern, uint8_t column,
                                     uint16_t row);

JS_EXPORT void audioDeletePatternRow(uint8_t pattern, uint8_t column,
                                     uint16_t row);

#endif

Pattern *getPattern(uint8_t pattern);

// patches
JS_EXPORT void audioSetPatchCount(uint8_t patchCount);
JS_EXPORT void audioCreatePatchNode(uint8_t patch, uint8_t node, uint8_t type);
JS_EXPORT void audioSetPatchNodeParam(uint8_t patch, uint8_t node,
                                      uint8_t param, float value);
JS_EXPORT void audioConnectPatchNode(uint8_t patch, uint8_t node, uint8_t input,
                                     uint8_t target, uint8_t output);

#ifndef PLAYER_ONLY

JS_EXPORT void audioDeletePatchNode(uint8_t patch, uint8_t node);
JS_EXPORT void audioDisconnectPatchNode(uint8_t patch, uint8_t node,
                                        uint8_t input);
JS_EXPORT void audioDeletePatchNodes(uint8_t patch);

#endif

Patch *getPatch(uint8_t patch);

JS_EXPORT void audioSetPatternRowCount(uint8_t pattern, uint16_t rowCount);

// other
JS_EXPORT void audioSetBPM(uint16_t bpm);
JS_EXPORT void audioSetLPB(uint8_t lpb);
JS_EXPORT void audioSetVolume(float volume);

#ifndef PLAYER_ONLY

JS_EXPORT void audioSetActiveSequence(uint8_t sequence);
JS_EXPORT void audioSetRepeatPattern(bool repeatPattern);

// notes
JS_EXPORT void audioPlayNote(uint8_t patch, uint8_t note, float velocity);
JS_EXPORT void audioStopNote(uint8_t patch, uint8_t note);

// control
JS_EXPORT void audioPlay();

#else

void audioSetActiveSequence(uint8_t sequence);
void audioPlay();

#endif

#ifdef PLAYER_ONLY

void audioSetSongDurationMs(uint32_t durationMs);
JS_EXPORT uint32_t audioGetSongDurationMs();
JS_EXPORT double audioGetStartTime();

#endif

#ifndef PLAYER_ONLY

// control
JS_EXPORT void audioStop();
JS_EXPORT void audioPanic();
JS_EXPORT void audioStopAllVoices();

JS_IMPORT("audioRowChangeEvent", extern void audioRowChangeEvent(uint16_t row));
JS_IMPORT("audioActiveSequenceChangeEvent",
          extern void audioActiveSequenceChangeEvent(uint8_t sequence));
JS_IMPORT("audioStopChangeEvent", extern void audioStopChangeEvent());
JS_IMPORT("audioClipEvent", extern void audioClipEvent(bool clipped));
JS_IMPORT("audioInfoEvent", extern void audioInfoEvent(uint16_t activeVoices,
                                                       uint16_t activePatches));

#endif

JS_EXPORT void audioSetSampleRate(uint32_t sampleRate);
JS_EXPORT void audioInit();

#ifndef PLAYER_ONLY

JS_EXPORT void audioKill();

#endif

JS_EXPORT void audioRender(float *left, float *right, uint32_t samples);

#endif
