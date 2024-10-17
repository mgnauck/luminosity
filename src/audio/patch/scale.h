
#ifndef __SCALE_H
#define __SCALE_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define SCALE_INPUT_IN    0
#define SCALE_PARAM_SCALE 0
#define SCALE_OUTPUT_OUT  0

void renderScale(Node *node, NodeState *state, Voice *voice);

#endif
