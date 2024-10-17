
#include "note.h"
#include "../../base/log.h"
#include "../../base/math.h"
#include <stddef.h>

void renderNote(Node *node, NodeState *state, Voice *voice)
{
  // float freq = noteToFreq(voice->note);
  float pitch = voice->note / 127.f;

  // TODO changes to the velocity should not be instant but
  // interpolated rapidly reaching the new value
  float fx0 = 0.f;
  float fx1 = 0.f;

  for(uint8_t i = 0; i < 2; i++) {
    // send fx type 0 and 1 to the output
    if(voice->fxType[i] == 0x0) {
      fx0 = voice->fxValue[i];
    }
    if(voice->fxType[i] == 0x1) {
      fx1 = voice->fxValue[i];
    }
  }

  state->outputs[NOTE_OUTPUT_PITCH] = pitch;
  state->outputs[NOTE_OUTPUT_VELOCITY] = voice->velocity;
  state->outputs[NOTE_OUTPUT_FX0_VALUE] = fx0;
  state->outputs[NOTE_OUTPUT_FX1_VALUE] = fx1;
}
