
#ifndef __PATCH_H
#define __PATCH_H

#include "../../base/base.h"
#include "node.h"
#include "voice.h"
#include <stdint.h>

#define MAX_PATCH_NODES  32
#define MAX_PATCH_VOICES 16

typedef struct {
#ifndef PLAYER_ONLY
  uint8_t index;
#endif
  double sampleRate;
  double oneOverSampleRate;
  Node **nodes;
  Voice *voices;
#ifndef PLAYER_ONLY
  uint16_t activeVoices;
#endif
  uint8_t polyphony;
  Node *out;
  float left;
  float right;
} Patch;

Patch *allocatePatch(uint8_t index, double sampleRate);
void deletePatch(Patch *patch);
void deletePatchNodes(Patch *patch);
void addNodeToPatch(Patch *patch, Node *node);
void removeNodeFromPatch(Patch *patch, Node *node);
Node *getNodeFromPatch(Patch *patch, uint8_t index);
void setPolyphonyOnPatch(Patch *patch, uint8_t polyphony);
Voice *findFreeVoice(Patch *patch);
void stopAllVoicesInPatch(Patch *patch);
void renderPatch(Patch *patch);
void playNoteOnPatch(Patch *patch, uint8_t note, float velocity, double time);
// TODO consider using a note instance reference instead of the note number
void stopNoteOnPatch(Patch *patch, uint8_t note);
void setNoteVelocityOnPatch(Patch *patch, uint8_t note, float velocity);
void setNoteEffectOnPatch(Patch *patch, uint8_t note, uint8_t index,
                          uint8_t type, float value);

#endif
