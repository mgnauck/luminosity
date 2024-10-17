
#include "dist.h"
#include "../../base/log.h"
#include "../../base/math.h"
#include <stddef.h>

void renderDist(Node *node, NodeState *state, Voice *voice)
{
  float in = getNodeInput(node, state, DIST_INPUT_IN);

  // TODO/FIXME make sure amount of zero does not influence the output
  float out = (2.f / PI) *
              atanf_approx(in * (1.0 + 10.0 * node->params[DIST_PARAM_LEVEL]));

  // store output in state
  state->outputs[DIST_OUTPUT_OUT] = clamp(out, -1.f, 1.f);
}
