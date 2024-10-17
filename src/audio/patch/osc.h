
#ifndef __OSC_H
#define __OSC_H

#include "node.h"
#include "voice.h"
#include <stdint.h>

#define OSC_INPUT_FREQ      0
#define OSC_INPUT_PM        1
#define OSC_INPUT_FM        2
#define OSC_PARAM_MODE      0
#define OSC_PARAM_MODE_SIN  0
#define OSC_PARAM_MODE_TRI  1
#define OSC_PARAM_MODE_SAW  2
#define OSC_PARAM_MODE_SQR  3
#define OSC_PARAM_FREQ      1
#define OSC_PARAM_PM_AMOUNT 2
#define OSC_PARAM_FM_AMOUNT 3
#define OSC_PARAM_DETUNE    4
#define OSC_PARAM_PW        5
#define OSC_PARAM_PHASE     6
#define OSC_OUTPUT_OUT      0

void initOsc(Node *node, NodeState *state);
void renderOsc(Node *node, NodeState *state, Voice *voice);

#endif
