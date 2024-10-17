
#include "delay.h"
#include "../../base/log.h"
#include "../../base/math.h"
#include "../../base/string.h"
#include "delay_line.h"
#include <stddef.h>

typedef struct {
  DelayLine *delayLine;
  float lastSample;
} DelayState;

void initDelay(Node *node, NodeState *state)
{
  DelayState *delayState = state->custom =
      (DelayState *)malloc(sizeof(DelayState));
  uint32_t maxSamples = state->sampleRate * MAX_DELAY_SECONDS;

  delayState->delayLine = allocateDelayLine(maxSamples);
}

void freeDelay(Node *node, NodeState *state)
{
  DelayState *delayState = (DelayState *)state->custom;

  freeDelayLine(delayState->delayLine);
  free(delayState->delayLine);
}

void renderDelay(Node *node, NodeState *state, Voice *voice)
{
  DelayState *delayState = (DelayState *)state->custom;

  float in = getNodeInput(node, state, DELAY_INPUT_IN);

  // hm hm hm
  // #ifdef RESET_STATE_ON_VOICE_START
  if(!state->renderOffset) {
    resetDelayLine(delayState->delayLine);
    delayState->lastSample = 0.f;
  }
  // #endif

  delayState->delayLine->samples =
      (uint32_t)state->sampleRate * node->params[DELAY_PARAM_DELAY];

  float delayInput =
      in + (delayState->lastSample * node->params[DELAY_PARAM_FEEDBACK]);
  float delayedSample =
      delayLineWriteAndRead(delayState->delayLine, delayInput);
  float filteredDelayedSample =
      delayState->lastSample * node->params[DELAY_PARAM_COLOR] +
      (delayedSample * (1.f - node->params[DELAY_PARAM_COLOR]));

  delayState->lastSample = filteredDelayedSample;

  float out = (in * (1.f - node->params[DELAY_PARAM_MIX])) +
              (delayedSample * node->params[DELAY_PARAM_MIX]);

  // store output in state
  state->outputs[DELAY_OUTPUT_OUT] = clamp(out, -1.f, 1.f);
}
