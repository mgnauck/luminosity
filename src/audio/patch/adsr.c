
#include "adsr.h"
#include "../../base/log.h"
#include "../../base/math.h"
#include <stddef.h>

#define ADSR_STATE_ATTACK  0
#define ADSR_STATE_DECAY   1
#define ADSR_STATE_SUSTAIN 2
#define ADSR_STATE_RELEASE 3
#define ADSR_STATE_IDLE    4

#define STATE_CURRENT state->customDataState
#define STATE_TIME    state->customData1
#define STATE_VALUE   state->customData2

void renderADSR(Node *node, NodeState *state, Voice *voice)
{
  if(!state->renderOffset) {
    // reset
    STATE_CURRENT = ADSR_STATE_ATTACK;

    // TODO not needed here?
    STATE_TIME = 0.0;
    // TODO could alter sound, critical?
    // STATE_VALUE = 0.f;
  }

  // if the voice was released early we need to
  if(voice->released && STATE_CURRENT != ADSR_STATE_RELEASE &&
     STATE_CURRENT != ADSR_STATE_IDLE) {
    STATE_CURRENT = ADSR_STATE_RELEASE;
    STATE_TIME = state->time;
  }

  float stateTime = state->time - STATE_TIME;

  switch(STATE_CURRENT) {
    case ADSR_STATE_ATTACK:
      // ramp from 0..1 (linear/cubic)
      if(stateTime > node->params[ADSR_PARAM_ATTACK] ||
         node->params[ADSR_PARAM_ATTACK] < EPSILON) {
        STATE_CURRENT = ADSR_STATE_DECAY;
        STATE_TIME = state->time;
        STATE_VALUE = 1.f;
      } else {
        float t = stateTime / node->params[ADSR_PARAM_ATTACK];
        STATE_VALUE = 1.f - (1.f - t) * (1.f - t);
      }
      state->outputs[ADSR_OUTPUT_OUT] = STATE_VALUE;
      break;
    case ADSR_STATE_DECAY:
      // ramp from 1..sustainLevel (linear/cubic)
      if(stateTime > node->params[ADSR_PARAM_DECAY] ||
         node->params[ADSR_PARAM_DECAY] < EPSILON) {
        STATE_CURRENT = ADSR_STATE_SUSTAIN;
        STATE_TIME = state->time;
        STATE_VALUE = node->params[ADSR_PARAM_SUSTAIN_LEVEL];
      } else {
        float t = stateTime / node->params[ADSR_PARAM_DECAY];
        STATE_VALUE =
            1.f - (1.f - node->params[ADSR_PARAM_SUSTAIN_LEVEL]) * (t * t);
      }
      state->outputs[ADSR_OUTPUT_OUT] = STATE_VALUE;
      break;
    case ADSR_STATE_SUSTAIN:
      STATE_VALUE = node->params[ADSR_PARAM_SUSTAIN_LEVEL];
      state->outputs[ADSR_OUTPUT_OUT] = STATE_VALUE;
      break;
    case ADSR_STATE_RELEASE:
      // ramp from current/sustainLevel..0 (linear/cubic)
      if(stateTime > node->params[ADSR_PARAM_RELEASE] ||
         node->params[ADSR_PARAM_RELEASE] < EPSILON) {
        STATE_CURRENT = ADSR_STATE_IDLE;
      } else {
        float t = stateTime / node->params[ADSR_PARAM_RELEASE];
        state->outputs[ADSR_OUTPUT_OUT] = STATE_VALUE * ((1.f - t) * (1.f - t));
      }
      break;
  }
}
