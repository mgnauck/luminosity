
#ifndef __AHDSR_H
#define __AHDSR_H

#include "node.h"
#include <stdint.h>

#define AHDSR_SUPPORT_LINEAR
#define AHDSR_SUPPORT_CUBIC

#define AHDSR_PARAM_MODE          0
#define AHDSR_PARAM_MODE_LINEAR   0
#define AHDSR_PARAM_MODE_CUBIC    1
#define AHDSR_PARAM_ATTACK        1
#define AHDSR_PARAM_HOLD          2
#define AHDSR_PARAM_DECAY         3
#define AHDSR_PARAM_SUSTAIN_LEVEL 4
#define AHDSR_PARAM_RELEASE       5

#define AHDSR_OUTPUT_OUT 0

void renderAHDSR(Node *node, NodeState *state, Voice *voice);

#endif
