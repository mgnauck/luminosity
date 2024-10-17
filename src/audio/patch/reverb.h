
#ifndef __REVERB_H
#define __REVERB_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define MAX_REVERB_SECONDS 2

#define REVERB_INPUT_IN        0
#define REVERB_PARAM_ROOM_SIZE 0
#define REVERB_PARAM_DECAY     1
#define REVERB_PARAM_DAMPING   2
#define REVERB_PARAM_MIX       3
#define REVERB_OUTPUT_OUT      0

void initReverb(Node *node, NodeState *state);
void freeReverb(Node *node, NodeState *state);
void renderReverb(Node *node, NodeState *state, Voice *voice);

#endif
