
#ifndef __OUT_H
#define __OUT_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define OUT_INPUT_LEFT      0
#define OUT_INPUT_RIGHT     1
#define OUT_PARAM_POLYPHONY 0
#define OUT_OUTPUT_LEFT     0
#define OUT_OUTPUT_RIGHT    1

void renderOut(Node *node, NodeState *state, Voice *voice);

#endif
