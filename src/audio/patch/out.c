
#include "out.h"
#include "../../base/log.h"
#include <stddef.h>

void renderOut(Node *node, NodeState *state, Voice *voice)
{
  float leftIn = getNodeInput(node, state, OUT_INPUT_LEFT);
  float rightIn = getNodeInput(node, state, OUT_INPUT_RIGHT);

  // store output in state
  state->outputs[OUT_OUTPUT_LEFT] = leftIn;
  state->outputs[OUT_OUTPUT_RIGHT] = rightIn;
}
