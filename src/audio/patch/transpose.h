
#ifndef __TRANSPOSE_H
#define __TRANSPOSE_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define TRANSPOSE_INPUT_IN   0
#define TRANSPOSE_PARAM_OCT  0
#define TRANSPOSE_PARAM_SEMI 1
#define TRANSPOSE_OUTPUT_OUT 0

void renderTranspose(Node *node, NodeState *state, Voice *voice);

#endif
