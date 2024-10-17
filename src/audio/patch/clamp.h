
#ifndef __CLAMP_H
#define __CLAMP_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define CLAMP_INPUT_IN   0
#define CLAMP_PARAM_MIN  0
#define CLAMP_PARAM_MAX  1
#define CLAMP_OUTPUT_OUT 0

void renderClamp(Node *node, NodeState *state, Voice *voice);

#endif
