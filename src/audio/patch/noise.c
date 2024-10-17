
#include "noise.h"
#include "../../base/log.h"
#include "../../base/math.h"
#include <stddef.h>

void renderNoise(Node *node, NodeState *state, Voice *voice)
{
  // TODO/FIXME
  // OH OH shouldn't this be in the range of -1..1?
  // CORRECT: float sample = (2.f * rand_float()) - 1.f;
  float sample = rand_float();

  state->outputs[NOISE_OUTPUT_OUT] = sample;
}
