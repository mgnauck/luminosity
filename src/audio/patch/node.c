
#include "node.h"
#include "../../base/log.h"
#include "../../base/walloc.h"
#include <stddef.h>

Node *allocateNode(uint8_t index, uint8_t type)
{
  Node *node = (Node *)malloc(sizeof(Node));

  node->index = index;
  node->type = type;

  // TODO maybe not needed if all params are set when loading a tune???
  // allocate and zero params
  // node->params = (float *)malloc(sizeof(float) * MAX_NODE_PARAMS);
  for(uint8_t i = 0; i < MAX_NODE_PARAMS; i++) {
    node->params[i] = 0.f;
  }

  // allocate and zero inputs
  // node->inputs = (NodeInput *)malloc(sizeof(NodeInput) * MAX_NODE_INPUTS);
  for(uint8_t i = 0; i < MAX_NODE_INPUTS; i++) {
    NodeInput *input = &node->inputs[i];
    input->node = NULL;
    input->output = NODE_OUTPUT_NONE;
  }

  return node;
}

void deleteNode(Node *node)
{
  // free(node->inputs);
  // free(node->params);
  free(node);
}

void connectNode(Node *node, uint8_t input, Node *target, uint8_t output)
{
  NodeInput *nodeInput = &node->inputs[input];
  nodeInput->node = target;
  nodeInput->output = output;
}

void disconnectNode(Node *node, uint8_t input)
{
  NodeInput *nodeInput = &node->inputs[input];
  nodeInput->node = NULL;
  nodeInput->output = NODE_OUTPUT_NONE;
}

void renderNode(Node *node, Voice *voice)
{
  NodeState *state = &voice->nodeStates[node->index];

  // only if the render offset changed, this allows us
  // to keep the output state of nodes when being connected
  // to multiple inputs without having to render them again
  if(state->renderOffset != voice->renderOffset) {
    // update state
    state->renderOffset = voice->renderOffset;
    state->time = voice->time;

    // render all connected inputs first
    // TODO figure out if there is shortcut
    // to quitting the loop earlier!?
    for(uint8_t i = 0; i < MAX_NODE_INPUTS; i++) {
      NodeInput *input = &node->inputs[i];
      if(input->node) {
        renderNode(input->node, voice);

        // grab the output of the input
        NodeState *inputState = &voice->nodeStates[input->node->index];
        state->inputs[i] = inputState->outputs[input->output];
      }
    }

    // now render the actual node
    node->renderFunc(node, state, voice);
  }
}

bool hasNodeInput(Node *node, uint8_t input)
{
  return node->inputs[input].node != NULL;
}

float getNodeInput(Node *node, NodeState *state, uint8_t input)
{
  return getNodeInputOrDefault(node, state, input, 0.f);
}

float getNodeInputOrDefault(Node *node, NodeState *state, uint8_t input,
                            float defaultValue)
{
  NodeInput *nodeInput = &node->inputs[input];
  if(nodeInput->node) {
    // connected, return current value
    return state->inputs[input];
  }

  return defaultValue;
}

float getNodeInputScaledOrParam(Node *node, NodeState *state, uint8_t input,
                                float scale, uint8_t param)
{
  NodeInput *nodeInput = &node->inputs[input];
  if(nodeInput->node) {
    // connected, return current value
    return state->inputs[input] * scale;
  }

  return node->params[param];
}

float getNodeInputOrParam(Node *node, NodeState *state, uint8_t input,
                          uint8_t param)
{
  NodeInput *nodeInput = &node->inputs[input];
  if(nodeInput->node) {
    // connected, return current value
    return state->inputs[input];
  }

  return node->params[param];
}

/*
void setNodeParam(Node *node, uint8_t param, float value)
{
    node->params[param] = value;
}
*/

/*
float getNodeParam(Node *node, uint8_t param)
{
    return node->params[param];
}
*/
