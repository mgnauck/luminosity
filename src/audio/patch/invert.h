
#ifndef __INVERT_H
#define __INVERT_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define INVERT_INPUT_IN   0
#define INVERT_OUTPUT_OUT 0

void renderInvert(Node *node, NodeState *state, Voice *voice);

#endif
