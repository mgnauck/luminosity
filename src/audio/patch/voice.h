
#ifndef __VOICE_H
#define __VOICE_H

#include "../../base/base.h"
#include "node_state.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  // TODO we should be able to get rid of index here
  // uint8_t index;
  // TODO consolidate with released?
  bool active;
  bool released;
  uint8_t note;
  float velocity;
  uint8_t fxType[2];
  float fxValue[2];
  double startTime;
  double time;
  uint32_t renderOffset;
  NodeState *nodeStates;
  // TODO theoretically this is not required in PLAYER_ONLY mode
  // as it will only increase performance which we only need for
  // interactive playback
  double energy;
} Voice;

void allocateVoice(Voice *voice);
void deleteVoice(Voice *voice);

#endif
