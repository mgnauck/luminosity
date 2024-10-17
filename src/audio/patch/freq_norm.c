
#include "freq_norm.h"
#include "../../base/log.h"
#include <stddef.h>

void renderFreqNorm(Node *node, NodeState *state, Voice *voice)
{
  float in = getNodeInput(node, state, FREQ_NORM_INPUT_IN);
  float scale = 1.f / (state->sampleRate * 0.5f);
  float out = in * scale;

  state->outputs[FREQ_NORM_OUTPUT_OUT] = out;
}
