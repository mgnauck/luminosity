
#include "scale.h"
#include "../../base/log.h"
#include <stddef.h>

void renderScale(Node *node, NodeState *state, Voice *voice)
{
  float in = getNodeInput(node, state, SCALE_INPUT_IN);
  state->outputs[SCALE_OUTPUT_OUT] = in * node->params[SCALE_PARAM_SCALE];
}
