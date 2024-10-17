
#include "offset.h"
#include "../../base/log.h"
#include <stddef.h>

void renderOffset(Node *node, NodeState *state, Voice *voice)
{
  float in = getNodeInput(node, state, OFFSET_INPUT_IN);
  state->outputs[OFFSET_OUTPUT_OUT] = in + node->params[OFFSET_PARAM_OFFSET];
}
