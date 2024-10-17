
#include "mul.h"
#include <stddef.h>

void renderMul(Node *node, NodeState *state, Voice *voice)
{
  float a = getNodeInput(node, state, MUL_INPUT_IN_A);
  float b = getNodeInput(node, state, MUL_INPUT_IN_B);

  state->outputs[MUL_OUTPUT_OUT] = a * b;
}
