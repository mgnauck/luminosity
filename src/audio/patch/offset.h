
#ifndef __OFFSET_H
#define __OFFSET_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define OFFSET_INPUT_IN     0
#define OFFSET_PARAM_OFFSET 0
#define OFFSET_OUTPUT_OUT   0

void renderOffset(Node *node, NodeState *state, Voice *voice);

#endif
