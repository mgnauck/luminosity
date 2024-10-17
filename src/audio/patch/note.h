
#ifndef __NOTE_H
#define __NOTE_H

#include "node.h"
#include <stdint.h>

#define NOTE_OUTPUT_PITCH     0
#define NOTE_OUTPUT_VELOCITY  1
#define NOTE_OUTPUT_FX0_VALUE 2
#define NOTE_OUTPUT_FX1_VALUE 3

void renderNote(Node *node, NodeState *state, Voice *voice);

#endif
