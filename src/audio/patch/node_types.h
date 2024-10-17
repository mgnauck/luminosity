
#ifndef __NODE_TYPES_H
#define __NODE_TYPES_H

#include "node.h"
#include <stdint.h>

#define NODE_TYPE_OSC       1
#define NODE_TYPE_NOISE     2
#define NODE_TYPE_AHDSR     3
#define NODE_TYPE_ADSR      4
#define NODE_TYPE_FILTER    5
#define NODE_TYPE_DELAY     6
#define NODE_TYPE_MUL       7
#define NODE_TYPE_SCALE     8
#define NODE_TYPE_OFFSET    9
#define NODE_TYPE_INVERT    10
#define NODE_TYPE_MIX       11
#define NODE_TYPE_FREQ_NORM 12
#define NODE_TYPE_DIST      13
#define NODE_TYPE_TRANSPOSE 14
#define NODE_TYPE_REVERB    15
#define NODE_TYPE_CLAMP     16
#define NODE_TYPE_NOISE2    17
#define NODE_TYPE_NOTE      98
#define NODE_TYPE_OUT       99

typedef struct NodeType {
  uint8_t type;
  NODE_STATE_INIT_FUNC initFunc;
  NODE_RENDER_FUNC renderFunc;
  NODE_STATE_FREE_FUNC freeFunc;
} NodeType;

NodeType *getNodeType(uint8_t type);

#endif
