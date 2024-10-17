
#ifndef __DELAY_H
#define __DELAY_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define MAX_DELAY_SECONDS 5

#define DELAY_INPUT_IN 0

#define DELAY_PARAM_DELAY    0
#define DELAY_PARAM_FEEDBACK 1
#define DELAY_PARAM_MIX      2
#define DELAY_PARAM_COLOR    3

#define DELAY_OUTPUT_OUT 0

void initDelay(Node *node, NodeState *state);
void freeDelay(Node *node, NodeState *state);
void renderDelay(Node *node, NodeState *state, Voice *voice);

#endif
