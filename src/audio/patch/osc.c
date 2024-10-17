
#include "osc.h"
#include "../../base/log.h"
#include "../../base/math.h"
#include <stddef.h>

#define STATE_PHASE state->customData1

float detuneFrequency(float baseFrequency, float detune)
{
  return baseFrequency * exp2f(detune / 1200.0f);
}

void renderOsc(Node *node, NodeState *state, Voice *voice)
{
  // freq input needs to follow the 1v/octave standard
  float freq = node->params[OSC_PARAM_FREQ];
  if(hasNodeInput(node, OSC_INPUT_FREQ)) {
    float freqIn = getNodeInput(node, state, OSC_INPUT_FREQ);
    freq = noteToFreq(freqIn * 127.f);
  }

  float pm = getNodeInputOrDefault(node, state, OSC_INPUT_PM, 0.f);
  float fm = getNodeInputOrDefault(node, state, OSC_INPUT_FM, 0.f);

  // TODO
  // sync?
  // unison?

  freq = freq * exp2f(node->params[OSC_PARAM_DETUNE] / 1200.0f);

#ifndef SKIP_RESET_STATE_ON_VOICE_START
  if(!state->renderOffset) {
    STATE_PHASE = 0.0;
    // logd("render osc, detune: %f, freq: %.2f, pw: %f", detune, freq, pw);
  }
#endif

  // float modulatedFreq = freq + fm * fmAmount * freq;
  float modulatedFreq =
      freq + freq * ((1.f - fm) * node->params[OSC_PARAM_FM_AMOUNT]);

  modulatedFreq = clamp(modulatedFreq, 0.f, state->sampleRate / 2.f);

  STATE_PHASE += modulatedFreq / state->sampleRate;

  float modulatedPhase = STATE_PHASE + pm * node->params[OSC_PARAM_PM_AMOUNT] +
                         node->params[OSC_PARAM_PHASE];
  modulatedPhase = fmodf(modulatedPhase, 1.0f);
  float t = modulatedPhase;

  float out = 0.f;
  switch((uint8_t)node->params[OSC_PARAM_MODE]) {
    case OSC_PARAM_MODE_SIN:
      out = sinf_approx(t * TAU);
      break;
    case OSC_PARAM_MODE_TRI:
      out = 2.f * fabsf(2.f * t - 1.f) - 1.f;
      break;
    case OSC_PARAM_MODE_SAW:
      out = 2.f * t - 1.0f;
      // adjust for loudness
      // out *= 0.577f; // 1/sqrt(3)
      break;
    case OSC_PARAM_MODE_SQR:
      out = t < node->params[OSC_PARAM_PW] ? 1.f : -1.f;
      // adjust for loudness
      // out *= 0.707f; // 1/sqrt(2)
      break;
  }

  // store output in state
  int a = out;
  state->outputs[OSC_OUTPUT_OUT] = clamp(out, -1.f, 1.f);
}
