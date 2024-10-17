
#include "patch.h"
#include "../../base/log.h"
#include "../../base/math.h"
#include "../../base/walloc.h"
#include "node_types.h"
#include "out.h"
#include <float.h>
#include <stdbool.h>
#include <stddef.h>

Patch *allocatePatch(uint8_t index, double sampleRate)
{
  Patch *patch = (Patch *)malloc(sizeof(Patch));

#ifndef PLAYER_ONLY
  patch->index = index;
#endif

  patch->sampleRate = sampleRate;
  patch->oneOverSampleRate = 1.0 / sampleRate;
  patch->out = NULL;

  patch->nodes = (Node **)malloc(MAX_PATCH_NODES * sizeof(Node));
  for(uint8_t i = 0; i < MAX_PATCH_NODES; i++) {
    patch->nodes[i] = NULL;
  }

  patch->voices = (Voice *)malloc(MAX_PATCH_VOICES * sizeof(Voice));
  for(uint8_t i = 0; i < MAX_PATCH_VOICES; i++) {
    Voice *voice = &patch->voices[i];
    // voice->index = i;
    voice->active = false;

    allocateVoice(voice);
  }

#ifndef PLAYER_ONLY
  patch->polyphony = 0;
#endif

  return patch;
}

void deletePatch(Patch *patch)
{
  deletePatchNodes(patch);
  free(patch->nodes);

  for(uint8_t i = 0; i < MAX_PATCH_VOICES; i++) {
    Voice *voice = &patch->voices[i];
    // logd("Deleting voice at offset %d", i);

    deleteVoice(voice);
  }
  free(patch->voices);
  free(patch);
}

void deletePatchNodes(Patch *patch)
{
  stopAllVoicesInPatch(patch);

  for(uint8_t i = 0; i < MAX_PATCH_NODES; i++) {
    Node *node = patch->nodes[i];
    if(node != NULL) {
      // logd("Deleting node at offset %d", i);

      removeNodeFromPatch(patch, node);
      deleteNode(node);
      patch->nodes[i] =
          NULL; // TODO remove me (happens in removeNodeFromPatch already)
    }
  }

  // NOOO free(patch->nodes);
}

void addNodeToPatch(Patch *patch, Node *node)
{
  for(uint8_t i = 0; i < MAX_PATCH_NODES; i++) {
    if(!patch->nodes[i]) {
      // logd("Adding node at offset %d", i);

      patch->nodes[i] = node;

      // in case of the output we remember it for later
      if(node->type == NODE_TYPE_OUT) {
        patch->out = node;
      }

      break;
    }
  }

  // initialize custom memory in node state
  for(uint8_t i = 0; i < MAX_PATCH_VOICES; i++) {
    Voice *voice = &patch->voices[i];
    NodeState *state = &voice->nodeStates[node->index];

    // inject sample rate
    state->sampleRate = patch->sampleRate;

    if(node->initFunc) {
      // logd("Allocating custom node state memory on node: %d, voice: %d",
      // node->index, i);
      node->initFunc(node, state);
    }
  }
}

#ifndef PLAYER_ONLY
void removeNodeFromPatch(Patch *patch, Node *node)
{
  if(node->initFunc) {
    // free custom memory in node state
    for(uint8_t i = 0; i < MAX_PATCH_VOICES; i++) {
      Voice *voice = &patch->voices[i];
      NodeState *state = &voice->nodeStates[node->index];

      if(state->custom != NULL) {
        // logd("Freeing custom node state memory on node: %d, voice: %d",
        // node->index, i);
        if(node->freeFunc != NULL) {
          node->freeFunc(node, state);
        }
        free(state->custom);
        state->custom = NULL;
      }
    }
  }

  for(uint8_t i = 0; i < MAX_PATCH_NODES; i++) {
    if(patch->nodes[i] == node) {
      // logd("Removing node at offset %d", i);

      patch->nodes[i] = NULL;

      // in case of the output we reset it
      if(node->type == NODE_TYPE_OUT) {
        // logd("Resetting patch output");

        patch->out = NULL;
      }
      return;
    }
  }
}
#endif

Node *getNodeFromPatch(Patch *patch, uint8_t index)
{
  for(uint8_t i = 0; i < MAX_PATCH_NODES; i++) {
    if(patch->nodes[i] && patch->nodes[i]->index == index) {
      return patch->nodes[i];
    }
  }

  return NULL;
}

void setPolyphonyOnPatch(Patch *patch, uint8_t polyphony)
{
#ifndef PLAYER_ONLY
  // clamp/sanitize polyphony setting but ignore in player modeF
  polyphony = clamp(polyphony, 1, MAX_PATCH_VOICES);

  // TODO/FIXME:
  // below code is probably not necessary in PLAYER_ONLY mode!??!!

  // decreasing below current amount of active voices
  // means we have to stop and free up voices, the simplest
  int16_t delta = patch->polyphony - polyphony;
  if(delta > 0) {
    // stop voices that are beyond the polyphony count
    for(uint32_t i = polyphony; i < patch->polyphony; i++) {
      // logd("Stopping voice: %d", i);

      Voice *voice = &patch->voices[i];
      if(voice->active) {
        voice->active = false;
      }
    }
  }
#endif

  patch->polyphony = polyphony;
}

Voice *findFreeVoice(Patch *patch)
{
  uint8_t usedVoices = 0;

  // find a free voice
  for(uint8_t i = 0; i < MAX_PATCH_VOICES; i++) {
    Voice *voice = &patch->voices[i];
    if(!voice->active) {
      // found inactive voice
      // logd("Found inactive voice at offset: %d", i);
      return voice;
    }

    if(++usedVoices >= patch->polyphony) {
      // logd("Exceeding polyphony count of %d", patch->polyphony);
      break;
    }
  }

  // reaching this point means no free voice was found we have
  // to steal one
  // TODO criteria 1 - find a voice that is released
  // TODO criteria 2 - find the voice with the lowest output volume
  // criteria 3 - find the oldest voice playing
  double oldestTime = DBL_MAX;
  Voice *oldestVoice = NULL;
  for(uint8_t i = 0; i < patch->polyphony; i++) {
    Voice *voice = &patch->voices[i];
    if(voice->startTime < oldestTime) {
      oldestTime = voice->startTime;
      oldestVoice = voice;
    }
  }

  return oldestVoice;
}

void stopAllVoicesInPatch(Patch *patch)
{
  // logd("Stopping all voices in patch %d", patch->index);

  for(uint8_t i = 0; i < MAX_PATCH_VOICES; i++) {
    Voice *voice = &patch->voices[i];
    if(voice->active) {
      voice->active = false;
    }
  }
}

void renderPatch(Patch *patch)
{
  // reset output
  patch->left = 0.f;
  patch->right = 0.f;

  // get polyphony from out node and update if required
  // uint8_t polyphony = getNodeParam(patch->out, OUT_PARAM_POLYPHONY);
  uint8_t polyphony = patch->out->params[OUT_PARAM_POLYPHONY];
  if(patch->polyphony != polyphony) {
    setPolyphonyOnPatch(patch, polyphony);
  }

#ifndef PLAYER_ONLY
  patch->activeVoices = 0;
#endif
  // TODO/FIXME:
  // maybe we need to reverse the processing here
  // and render each node for all voices instead
  // of for all voices each node???

  // main patch rendering
  for(uint8_t i = 0; i < patch->polyphony; i++) {
    Voice *voice = &patch->voices[i];
    if(voice->active) {
      // render output node
      renderNode(patch->out, voice);

      // get output node state
      NodeState *outputState = &voice->nodeStates[patch->out->index];

      float left = outputState->outputs[OUT_OUTPUT_LEFT];
      float right = outputState->outputs[OUT_OUTPUT_RIGHT];

      // TODO/FIXME: first simple approach to detecting whether or
      // not the voice is still producing output
      float mono = (left + right) * 0.5f;
      // TODO find a propoer value for smooth
      double smooth = 0.0001;
      double energy = mono * mono;

      voice->energy = smooth * energy + (1.0 - smooth) * voice->energy;

      patch->left += left;
      patch->right += right;

#ifndef PLAYER_ONLY
      patch->activeVoices++;
#endif

      voice->renderOffset++;
      voice->time += patch->oneOverSampleRate;

      // simple voice stop detection
      // only if released and at least playing for
      // a while (to avoid cutting of delayed voices that will produce output
      // later)
      // TODO/FIXME: this won't work in case of a delay that only outputs
      // something every XX seconds :x
      if(voice->released && voice->time > 0.25f) {
        // TODO find a meaningful threshold
        // TODO a more sophisticated approach would be to kick off
        // a timer when energy is low and if it stays this way for
        // some duration we can better decide if we want to really stop
        if(voice->energy < 0.000001) {
          voice->active = false;
          // logd("voice %d stopped, energy: %f", i, energy);
        }
      }
    }
  }
}

void playNoteOnPatch(Patch *patch, uint8_t note, float velocity, double time)
{
  // find a free voice
  Voice *voice = findFreeVoice(patch);

#ifndef PLAYER_ONLY
  if(!voice) {
    // should never happen
    logw("No free voice returned for note play on patch: %d, note: %d",
         patch->index, note);
    return;
  }

  if(!patch->out) {
    // should never happen
    logw("No output node on patch: %d, note: %d", patch->index, note);
    return;
  }
#endif

  // mark as active and start voice
  voice->active = true;
  voice->released = false;
  voice->note = note;
  voice->velocity = velocity;
  voice->startTime = time;
  voice->time = 0.0;
  voice->renderOffset = 0;
  voice->fxType[0] = 0xf;
  voice->fxType[1] = 0xf;
  voice->energy = 0.f;

  // initialize node states in voice
  for(uint8_t i = 0; i < MAX_PATCH_NODES; i++) {
    NodeState *state = &voice->nodeStates[i];
    state->renderOffset = UINT32_MAX;
    state->time = 0.0;
  }

  // logd("Starting note play on patch: %d, voice: %d, note: %d, velocity: %f,
  // time: %.3f",
  //          patch->index, voice->index, note, velocity, time);
}

void stopNoteOnPatch(Patch *patch, uint8_t note)
{
  // find the voice that is currently placing the note
  // find a free voice
  for(uint32_t i = 0; i < patch->polyphony; i++) {
    Voice *voice = &patch->voices[i];

    if(voice->active && !voice->released && voice->note == note) {
      // logd("Releasing note on patch: %d, voice: %d, note: %d, startTime:
      // %.3f",
      //          patch->index, voice->index, note, voice->startTime);

      voice->released = true;
      return;
    }
  }

  // reaching this point should be fine as the voice might have gotten stolen in
  // the meantime, so we can simply ignore it
}

void setNoteVelocityOnPatch(Patch *patch, uint8_t note, float velocity)
{
  // find the playing note and update the voice
  for(uint32_t i = 0; i < patch->polyphony; i++) {
    Voice *voice = &patch->voices[i];

    // TODO figure out of voice->released makes sense here, this
    // won't allow us changing velocity of released notes but i don't
    // know if this is a major problem
    if(voice->active && !voice->released && voice->note == note) {
      // logd("Set velocity: %f on patch: %d, note: %d, voice: %d",
      //          velocity, patch->index, note, voice->index);
      voice->velocity = velocity;
      return;
    }
  }
}

void setNoteEffectOnPatch(Patch *patch, uint8_t note, uint8_t index,
                          uint8_t type, float value)
{
  // find the playing note and update the voice
  for(uint32_t i = 0; i < patch->polyphony; i++) {
    Voice *voice = &patch->voices[i];

    // TODO figure out of voice->released makes sense here, this
    // won't allow us changing the effect value of released notes but
    // i don't know if this is a major problem
    if(voice->active && !voice->released && voice->note == note) {
      // logd("Setting effect on patch: %d, index: %d, type: %d, value: %.2f",
      //          patch->index, index, type, value);

      voice->fxType[index] = type;
      voice->fxValue[index] = value;
      return;
    }
  }
}
