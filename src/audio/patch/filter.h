
#ifndef __SVF_H
#define __SVF_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define FILTER_INPUT_IN            0
#define FILTER_INPUT_CUTOFF        1
#define FILTER_INPUT_RESONANCE     2
#define FILTER_PARAM_TYPE          0
#define FILTER_PARAM_TYPE_LOWPASS  0
#define FILTER_PARAM_TYPE_BANDPASS 1
#define FILTER_PARAM_TYPE_HIGHPASS 2
#define FILTER_PARAM_TYPE_NOTCH    3
#define FILTER_PARAM_CUTOFF        1
#define FILTER_PARAM_RESONANCE     2
#define FILTER_OUTPUT_OUT          0

void initFilter(Node *node, NodeState *state);
void renderFilter(Node *node, NodeState *state, Voice *voice);

#endif
