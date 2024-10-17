
#include "clamp.h"
#include "../../base/log.h"
#include "../../base/math.h"
#include <stddef.h>

void renderClamp(Node *node, NodeState *state, Voice *voice)
{
  // TODO optimize away by using node->params directly
  float in = getNodeInput(node, state, CLAMP_INPUT_IN);
  float out =
      clamp(in, node->params[CLAMP_PARAM_MIN], node->params[CLAMP_PARAM_MAX]);

  state->outputs[CLAMP_OUTPUT_OUT] = out;
}
