
#include "invert.h"
#include "../../base/log.h"
#include <stddef.h>

void renderInvert(Node *node, NodeState *state, Voice *voice)
{
  float in = getNodeInput(node, state, INVERT_INPUT_IN);

  state->outputs[INVERT_OUTPUT_OUT] = in * -1.f;
}
