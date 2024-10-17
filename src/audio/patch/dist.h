
#ifndef __DIST_H
#define __DIST_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define DIST_INPUT_IN    0
#define DIST_PARAM_LEVEL 0
#define DIST_OUTPUT_OUT  0

void renderDist(Node *node, NodeState *state, Voice *voice);

#endif
