
#include "filter.h"
#include "../../base/log.h"
#include "../../base/math.h"
#include <stddef.h>

#define STATE_LOW   state->customData1
#define STATE_BAND  state->customData2
#define STATE_HIGH  state->customData3
#define STATE_NOTCH state->customData4

void renderFilter(Node *node, NodeState *state, Voice *voice)
{
  double in = getNodeInput(node, state, FILTER_INPUT_IN);

  // double cutoff = getNodeInputScaledOrParam(node, state, FILTER_INPUT_CUTOFF,
  //                                           8000.f, FILTER_PARAM_CUTOFF);

  double cutoff = node->params[FILTER_PARAM_CUTOFF];
  if(hasNodeInput(node, FILTER_INPUT_CUTOFF)) {
    float cutoffIn = getNodeInput(node, state, FILTER_INPUT_CUTOFF);
    cutoff = noteToFreq(cutoffIn * 127.f);
  }

  double resonance = getNodeInputOrParam(node, state, FILTER_INPUT_RESONANCE,
                                         FILTER_PARAM_RESONANCE);

#ifndef SKIP_RESET_STATE_ON_VOICE_START
  if(!state->renderOffset) {
    STATE_LOW = 0.0;
    STATE_BAND = 0.0;
    STATE_HIGH = 0.0;
    STATE_NOTCH = 0.0;
  }
#endif

  // https://www.musicdsp.org/en/latest/Filters/23-state-variable.html
  // TODO/FIXME: only need to recalculate this if resonance changes
  double q =
      1.f - resonance; // sqrtf(1.f - atanf(sqrtf(resonanceParam)) * 2.f / PI);
  double scale = sqrtf(q);

  // double f = cutoff / state->sampleRate * 2.f;
  // corrected for double sampling
  double f =
      2.0 * sinf_approx(PI * min(1.0, cutoff / (state->sampleRate * 2.0)));
  float s = scale * in;

  // first pass
  STATE_LOW += f * STATE_BAND;
  STATE_HIGH = s - STATE_LOW - q * STATE_BAND;
  STATE_BAND += f * STATE_HIGH;
  STATE_NOTCH = STATE_HIGH + STATE_LOW;

  // second pass
  STATE_LOW += f * STATE_BAND;
  STATE_HIGH = s - STATE_LOW - q * STATE_BAND;
  STATE_BAND += f * STATE_HIGH;
  STATE_NOTCH = STATE_HIGH + STATE_LOW;

  float out = 0.f;
  switch((uint8_t)node->params[FILTER_PARAM_TYPE]) {
    case FILTER_PARAM_TYPE_LOWPASS:
      out = STATE_LOW;
      break;
    case FILTER_PARAM_TYPE_BANDPASS:
      out = STATE_BAND;
      break;
    case FILTER_PARAM_TYPE_HIGHPASS:
      out = STATE_HIGH;
      break;
    case FILTER_PARAM_TYPE_NOTCH:
      out = STATE_NOTCH;
      break;
  }

  state->outputs[FILTER_OUTPUT_OUT] = clamp(out, -1.f, 1.f);
}
