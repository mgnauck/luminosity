
#ifndef __NODE_STATE_H
#define __NODE_STATE_H

#include "../../base/base.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_NODE_INPUTS  4
#define MAX_NODE_OUTPUTS 4

typedef struct {
  uint32_t renderOffset;
  double time;
  double sampleRate;
  // float *inputs;
  // float *outputs;
  float inputs[MAX_NODE_INPUTS];
  float outputs[MAX_NODE_OUTPUTS];
  void *custom;
  // to save some of the simpler custom states we introduce some
  // additional states here so nodes can utilize them to store data
  double customData1;
  double customData2;
  double customData3;
  double customData4;
  // TODO remove the state below and use a double for it (should work)
  uint8_t customDataState;
} NodeState;

#endif
