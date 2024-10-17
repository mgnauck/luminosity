
#ifndef __ADSR_H
#define __ADSR_H

#include "node.h"
#include <stdint.h>

#define ADSR_PARAM_ATTACK        0
#define ADSR_PARAM_DECAY         1
#define ADSR_PARAM_SUSTAIN_LEVEL 2
#define ADSR_PARAM_RELEASE       3

#define ADSR_OUTPUT_OUT 0

void renderADSR(Node *node, NodeState *state, Voice *voice);

#endif
