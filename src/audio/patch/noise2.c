
#include "noise2.h"
#include "../../base/log.h"
#include "../../base/math.h"
#include <stddef.h>

void renderNoise2(Node *node, NodeState *state, Voice *voice)
{
  state->outputs[NOISE2_OUTPUT_OUT] = (2.f * rand_float()) - 1.f;
}
