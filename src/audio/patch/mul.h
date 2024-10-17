
#ifndef __MUL_H
#define __MUL_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define MUL_INPUT_IN_A 0
#define MUL_INPUT_IN_B 1
#define MUL_OUTPUT_OUT 0

void renderMul(Node *node, NodeState *state, Voice *voice);

#endif
