
#include "voice.h"
#include "../../base/walloc.h"
#include "patch.h"
#include <stdbool.h>
#include <stddef.h>

void allocateVoice(Voice *voice)
{
  // allocate node states per voice but leave uninitialized as this
  // will happen when the voice begins to play
  voice->nodeStates = (NodeState *)malloc(MAX_PATCH_NODES * sizeof(NodeState));

#ifndef PLAYER_ONLY
  for(uint8_t i = 0; i < MAX_PATCH_NODES; i++) {
    NodeState *state = &voice->nodeStates[i];
    // state->inputs = (float *)malloc(MAX_NODE_INPUTS * sizeof(float));
    // state->outputs = (float *)malloc(MAX_NODE_OUTPUTS * sizeof(float));
    state->custom = NULL;
  }
#endif
}

void deleteVoice(Voice *voice)
{
  for(uint8_t i = 0; i < MAX_PATCH_NODES; i++) {
    NodeState *state = &voice->nodeStates[i];

    // free(state->inputs);
    // free(state->outputs);

    // TODO refactor, this is probably redundant
    if(state->custom) {
      free(state->custom);
      state->custom = NULL;
    }
  }

  free(voice->nodeStates);
}
