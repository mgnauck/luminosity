
#include "ahdsr.h"
#include "../../base/log.h"
#include "../../base/math.h"
#include <stddef.h>

#define AHDSR_STATE_ATTACK  0
#define AHDSR_STATE_HOLD    1
#define AHDSR_STATE_DECAY   2
#define AHDSR_STATE_SUSTAIN 3
#define AHDSR_STATE_RELEASE 4
#define AHDSR_STATE_IDLE    5

#define STATE_CURRENT state->customDataState
#define STATE_TIME    state->customData1
#define STATE_VALUE   state->customData2

void renderAHDSR(Node *node, NodeState *state, Voice *voice)
{
  if(!state->renderOffset) {
    // reset
    STATE_CURRENT = AHDSR_STATE_ATTACK;

    // TODO not needed here?
    STATE_TIME = 0.0;
    // TODO could alter sound, critical?
    // STATE_VALUE = 0.f;
  }

  // if the voice was released early we need to
  if(voice->released && STATE_CURRENT != AHDSR_STATE_RELEASE &&
     STATE_CURRENT != AHDSR_STATE_IDLE) {
    STATE_CURRENT = AHDSR_STATE_RELEASE;
    STATE_TIME = state->time;
  }

  float stateTime = state->time - STATE_TIME;

  switch(STATE_CURRENT) {
    case AHDSR_STATE_ATTACK:
      // ramp from 0..1 (linear/cubic)
      if(stateTime > node->params[AHDSR_PARAM_ATTACK] ||
         node->params[AHDSR_PARAM_ATTACK] < EPSILON) {
        STATE_CURRENT = AHDSR_STATE_HOLD;
        STATE_TIME = state->time;
        STATE_VALUE = 1.f;
      } else {
        float t = stateTime / node->params[AHDSR_PARAM_ATTACK];
        STATE_VALUE = node->params[AHDSR_PARAM_MODE] == AHDSR_PARAM_MODE_LINEAR
                          ? t
                          : 1.f - (1.f - t) * (1.f - t);
      }
      state->outputs[AHDSR_OUTPUT_OUT] = STATE_VALUE;
      break;
    case AHDSR_STATE_HOLD:
      // stay on 1
      STATE_VALUE = 1.f;
      if(stateTime > node->params[AHDSR_PARAM_HOLD] ||
         node->params[AHDSR_PARAM_HOLD] < EPSILON) {
        STATE_CURRENT = AHDSR_STATE_DECAY;
        STATE_TIME = state->time;
      }
      state->outputs[AHDSR_OUTPUT_OUT] = STATE_VALUE;
      break;
    case AHDSR_STATE_DECAY:
      // ramp from 1..sustainLevel (linear/cubic)
      if(stateTime > node->params[AHDSR_PARAM_DECAY] ||
         node->params[AHDSR_PARAM_DECAY] < EPSILON) {
        STATE_CURRENT = AHDSR_STATE_SUSTAIN;
        STATE_TIME = state->time;
        STATE_VALUE = node->params[AHDSR_PARAM_SUSTAIN_LEVEL];
      } else {
        float t = stateTime / node->params[AHDSR_PARAM_DECAY];
        STATE_VALUE =
            1.f - (1.f - node->params[AHDSR_PARAM_SUSTAIN_LEVEL]) *
                      (node->params[AHDSR_PARAM_MODE] == AHDSR_PARAM_MODE_LINEAR
                           ? t
                           : t * t);
      }
      state->outputs[AHDSR_OUTPUT_OUT] = STATE_VALUE;
      break;
    case AHDSR_STATE_SUSTAIN:
      STATE_VALUE = node->params[AHDSR_PARAM_SUSTAIN_LEVEL];
      state->outputs[AHDSR_OUTPUT_OUT] = STATE_VALUE;
      break;
    case AHDSR_STATE_RELEASE:
      // ramp from current/sustainLevel..0 (linear/cubic)
      if(stateTime > node->params[AHDSR_PARAM_RELEASE] ||
         node->params[AHDSR_PARAM_RELEASE] < EPSILON) {
        STATE_CURRENT = AHDSR_STATE_IDLE;
      } else {
        float t = stateTime / node->params[AHDSR_PARAM_RELEASE];
        state->outputs[AHDSR_OUTPUT_OUT] =
            STATE_VALUE *
            (node->params[AHDSR_PARAM_MODE] == AHDSR_PARAM_MODE_LINEAR
                 ? (1.f - t)
                 : (1.f - t) * (1.f - t));
      }
      break;
  }
}
