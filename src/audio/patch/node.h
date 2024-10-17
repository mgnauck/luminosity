
#ifndef __NODE_H
#define __NODE_H

#include "../../base/base.h"
#include "node_state.h"
#include "voice.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_NODE_PARAMS  8
#define NODE_OUTPUT_NONE 0xff

struct Node;

typedef void (*NODE_RENDER_FUNC)(struct Node *node, NodeState *state,
                                 Voice *voice);
typedef void (*NODE_STATE_INIT_FUNC)(struct Node *node, NodeState *state);
typedef void (*NODE_STATE_FREE_FUNC)(struct Node *node, NodeState *state);

typedef struct {
  struct Node *node;
  uint8_t output;
} NodeInput;

typedef struct Node {
  // TODO we should be able to get rid of the index somehow
  uint8_t index;
  uint8_t type;
  // float *params;
  float params[MAX_NODE_PARAMS];
  // NodeInput *inputs;
  NodeInput inputs[MAX_NODE_INPUTS];
  NODE_STATE_INIT_FUNC initFunc;
  NODE_RENDER_FUNC renderFunc;
#ifndef PLAYER_ONLY
  NODE_STATE_FREE_FUNC freeFunc;
#endif
} Node;

Node *allocateNode(uint8_t index, uint8_t type);
void deleteNode(Node *node);
void connectNode(Node *node, uint8_t input, Node *target, uint8_t output);
void disconnectNode(Node *node, uint8_t input);
void renderNode(Node *node, Voice *voice);
bool hasNodeInput(Node *node, uint8_t input);
float getNodeInput(Node *node, NodeState *state, uint8_t input);
float getNodeInputOrParam(Node *node, NodeState *state, uint8_t input,
                          uint8_t param);
float getNodeInputScaledOrParam(Node *node, NodeState *state, uint8_t input,
                                float scale, uint8_t param);
float getNodeInputOrDefault(Node *node, NodeState *state, uint8_t input,
                            float defaultValue);
// void setNodeParam(Node *node, uint8_t param, float value);
// float getNodeParam(Node *node, uint8_t param);

#endif
