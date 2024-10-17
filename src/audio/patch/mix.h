
#ifndef __MIX_H
#define __MIX_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define MIX_INPUT_IN_A 0
#define MIX_INPUT_IN_B 1
#define MIX_PARAM_MIX  0
#define MIX_OUTPUT_OUT 0

void renderMix(Node *node, NodeState *state, Voice *voice);

#endif
