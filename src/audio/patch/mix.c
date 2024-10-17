
#include "mix.h"
#include "../../base/log.h"
#include <stddef.h>

void renderMix(Node *node, NodeState *state, Voice *voice)
{
  float a = getNodeInput(node, state, MIX_INPUT_IN_A);
  float b = getNodeInput(node, state, MIX_INPUT_IN_B);

  state->outputs[MIX_OUTPUT_OUT] = a * node->params[MIX_PARAM_MIX] +
                                   (b * (1.f - node->params[MIX_PARAM_MIX]));
}
