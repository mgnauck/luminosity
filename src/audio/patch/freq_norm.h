
#ifndef __FREQ_NORM_H
#define __FREQ_NORM_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define FREQ_NORM_INPUT_IN   0
#define FREQ_NORM_OUTPUT_OUT 0

void renderFreqNorm(Node *node, NodeState *state, Voice *voice);

#endif
