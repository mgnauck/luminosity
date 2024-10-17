
#include "reverb.h"
#include "../../base/log.h"
#include "../../base/math.h"
#include "../../base/string.h"
#include "delay_line.h"
#include <stddef.h>

#define REVERB_MAX_COMBS   4
#define REVERB_MAX_ALLPASS 2

// constant comb and all-pass filter delays in ms
float filterDelaysMs[REVERB_MAX_COMBS + REVERB_MAX_ALLPASS] = {
    35.0f, 37.0f, 41.0f, 43.0f, 6.0f, 17.0f};

typedef struct {
  DelayLine *delayLine;
  float feedback;
  float lastSample;
} ReverbFilterState;

typedef struct {
  ReverbFilterState filters[REVERB_MAX_COMBS + REVERB_MAX_ALLPASS];
} ReverbState;

void initReverb(Node *node, NodeState *state)
{
  ReverbState *reverbState = state->custom =
      (ReverbState *)malloc(sizeof(ReverbState));
  uint32_t maxSamples = state->sampleRate * MAX_REVERB_SECONDS;

  // comb + allpass
  for(uint8_t i = 0; i < REVERB_MAX_COMBS + REVERB_MAX_ALLPASS; i++) {
    reverbState->filters[i].delayLine = allocateDelayLine(maxSamples);
  }
}

void freeReverb(Node *node, NodeState *state)
{
  ReverbState *reverbState = (ReverbState *)state->custom;

  // comb + allpass
  for(uint8_t i = 0; i < REVERB_MAX_COMBS + REVERB_MAX_ALLPASS; i++) {
    freeDelayLine(reverbState->filters[i].delayLine);
    free(reverbState->filters[i].delayLine);
  }
}

void resetReverb(ReverbState *reverbState)
{
  // comb + allpass
  for(uint8_t i = 0; i < REVERB_MAX_COMBS + REVERB_MAX_ALLPASS; i++) {
    resetDelayLine(reverbState->filters[i].delayLine);
    reverbState->filters[i].lastSample = 0.f;
  }
}

void renderReverb(Node *node, NodeState *state, Voice *voice)
{
  ReverbState *reverbState = (ReverbState *)state->custom;

  // input
  float in = getNodeInput(node, state, REVERB_INPUT_IN);

  // input parameters
  // roomSize - controls the comb filter delay lines 0.5 .. 1.5
  // float roomSize = getNodeParam(node, REVERB_PARAM_ROOM_SIZE);
  // decayRT60 - how long the reverb lasts before decaying 0.5 .. 2.5
  // float decayRT60 = getNodeParam(node, REVERB_PARAM_DECAY);
  // damping - how much high frequency is absorbed 0 (bright) .. 0.9 (dark)
  // float damping = 1.f - getNodeParam(node, REVERB_PARAM_DAMPING);
  // dry/wet - how much of reverb is added to the input signal
  // float mix = getNodeParam(node, REVERB_PARAM_MIX);

  // TODO check if it harms to disable this here!
  if(!state->renderOffset) {
    resetReverb(reverbState);
  }

  // process comb filters
  float combSum = 0.f;
  for(uint8_t i = 0; i < REVERB_MAX_COMBS; i++) {
    ReverbFilterState *filter = &reverbState->filters[i];

    float delayMs = filterDelaysMs[i] * node->params[REVERB_PARAM_ROOM_SIZE];
    uint32_t delaySamples = (uint32_t)(delayMs * state->sampleRate / 1000.0f);
    filter->delayLine->samples = delaySamples;
    filter->feedback = exp2f(-3.0f *
                             (delaySamples / (node->params[REVERB_PARAM_DECAY] *
                                              state->sampleRate)) *
                             LOG210);

    float delayLineInput = in + (filter->lastSample * filter->feedback);
    float delayedOut = delayLineWriteAndRead(filter->delayLine, delayLineInput);

    filter->lastSample =
        filter->lastSample + (delayedOut - filter->lastSample) *
                                 (1.f - node->params[REVERB_PARAM_DAMPING]);

    combSum += filter->lastSample;
  }

  // normalize according to comb filter count
  combSum *= (1.0f / REVERB_MAX_COMBS);

  // process allpass filters
  float allpassOut = combSum;

  for(uint8_t i = REVERB_MAX_COMBS; i < REVERB_MAX_COMBS + REVERB_MAX_ALLPASS;
      i++) {
    ReverbFilterState *filter = &reverbState->filters[i];

    uint32_t delaySamples =
        (uint32_t)(filterDelaysMs[i] * state->sampleRate / 1000.0f);
    filter->delayLine->samples = delaySamples;
    filter->feedback = 0.7f;

    float delayLineInput = allpassOut + (filter->feedback * filter->lastSample);
    float delayedOut = delayLineWriteAndRead(filter->delayLine, delayLineInput);
    float out = delayedOut - (filter->feedback * allpassOut);

    filter->lastSample = delayedOut;

    allpassOut = out;
  }

  // assign final output
  state->outputs[REVERB_OUTPUT_OUT] =
      (in * (1.f - node->params[REVERB_PARAM_MIX])) +
      (allpassOut * node->params[REVERB_PARAM_MIX]);
}
