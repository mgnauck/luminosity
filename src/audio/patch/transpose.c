
#include "transpose.h"
#include "../../base/log.h"
#include "../../base/math.h"
#include <stddef.h>

void renderTranspose(Node *node, NodeState *state, Voice *voice)
{
  float in = getNodeInput(node, state, TRANSPOSE_INPUT_IN);

  // convert into actual note
  float note = in * 127.f + node->params[TRANSPOSE_PARAM_OCT] * 12.f +
               node->params[TRANSPOSE_PARAM_SEMI];

  note = clamp(note, 0.f, 127.f);

  // store output in state
  state->outputs[TRANSPOSE_OUTPUT_OUT] = note / 127.f;
}
