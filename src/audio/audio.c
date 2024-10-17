
#include "audio.h"
#include "../base/log.h"
#include "../base/math.h"
#include "patch/node_types.h"

AudioState audioState;

void audioSetTrackCount(uint8_t trackCount)
{
  logd("Setting track count to %d", trackCount);

  audioState.trackCount = trackCount;
}

void audioSetTrackActive(uint8_t track, bool active)
{
  logd("Setting track %d to %s", track, (active ? "on" : "off"));

  audioState.tracks[track].active = active;
}

void audioSetSequenceCount(uint8_t sequenceCount)
{
  logd("Setting sequence count to %d", sequenceCount);

  audioState.sequenceCount = sequenceCount;
}

void audioSetSequence(uint8_t sequence, uint8_t pattern)
{
  logd("Setting sequence %d to pattern %d", sequence, pattern);

  audioState.sequences[sequence] = pattern;
}

void audioSetPattern(uint8_t pattern, uint8_t column, uint16_t row,
                     uint8_t note, uint8_t patch, uint8_t volume,
                     uint8_t fx1type, uint8_t fx1value, uint8_t fx2type,
                     uint8_t fx2value)
{
  logd("Setting pattern %d at [%d, %d] to note: %d, patch: %d, volume: %d, "
       "fx1:%x%02x, fx2:%x%02x",
       pattern, column, row, note, patch, volume, fx1type, fx1value, fx2type,
       fx2value);

  Pattern *targetPattern = getPattern(pattern);
  PatternEntry *targetEntry = getEntryFromPattern(targetPattern, column, row);

  targetEntry->note = note;
  targetEntry->patch = patch;
  targetEntry->volume = volume;
  targetEntry->fxType[0] = fx1type;
  targetEntry->fxValue[0] = fx1value;
  targetEntry->fxType[1] = fx2type;
  targetEntry->fxValue[1] = fx2value;
}

// remove me in player only
void audioInsertPatternRow(uint8_t pattern, uint8_t column, uint16_t row)
{
  logd("Insert pattern row, pattern %d, column: %d, row: %d", pattern, column,
       row);

  Pattern *targetPattern = getPattern(pattern);

  for(uint16_t currentRow = targetPattern->rows - 1; currentRow > row;
      currentRow--) {
    PatternEntry *sourceEntry =
        getEntryFromPattern(targetPattern, column, currentRow - 1);
    PatternEntry *targetEntry =
        getEntryFromPattern(targetPattern, column, currentRow);

    // copy
    *targetEntry = *sourceEntry;
  }

  PatternEntry *targetEntry = getEntryFromPattern(targetPattern, column, row);
  *targetEntry = EMPTY_PATTERN_ENTRY;
}

// remove me in player only
void audioDeletePatternRow(uint8_t pattern, uint8_t column, uint16_t row)
{
  logd("Delete pattern row, pattern %d, column: %d, row: %d", pattern, column,
       row);

  Pattern *targetPattern = getPattern(pattern);

  for(uint16_t currentRow = row; currentRow < targetPattern->rows - 1;
      currentRow++) {
    PatternEntry *sourceEntry =
        getEntryFromPattern(targetPattern, column, currentRow + 1);
    PatternEntry *targetEntry =
        getEntryFromPattern(targetPattern, column, currentRow);

    // copy
    *targetEntry = *sourceEntry;
  }

  PatternEntry *targetEntry =
      getEntryFromPattern(targetPattern, column, targetPattern->rows - 1);
  *targetEntry = EMPTY_PATTERN_ENTRY;
}

Pattern *getPattern(uint8_t pattern)
{
  if(audioState.patterns[pattern] == NULL) {
    logd("Allocating new pattern at index %d", pattern);
    audioState.patterns[pattern] = allocatePattern();
  }

  return audioState.patterns[pattern];
}

void audioSetPatchCount(uint8_t patchCount)
{
  logd("Setting patch count to %d", patchCount);

  audioState.patchCount = patchCount;
}

void audioCreatePatchNode(uint8_t patch, uint8_t node, uint8_t type)
{
  logd("Create node, patch %d, node: %d, type: %d", patch, node, type);

  Patch *targetPatch = getPatch(patch);
  NodeType *nodeType = getNodeType(type);

#ifndef PLAYER_ONLY
  if(!nodeType) {
    loge("Unsupported node type: %d", type);
    return;
  }
#endif

  Node *newNode = allocateNode(node, type);
  newNode->initFunc = nodeType->initFunc;
  newNode->renderFunc = nodeType->renderFunc;
#ifndef PLAYER_ONLY
  newNode->freeFunc = nodeType->freeFunc;
#endif

  addNodeToPatch(targetPatch, newNode);
}

void audioDeletePatchNode(uint8_t patch, uint8_t node)
{
  logd("Delete node, patch %d, node: %d", patch, node);

  Patch *targetPatch = getPatch(patch);
  Node *targetNode = getNodeFromPatch(targetPatch, node);

  removeNodeFromPatch(targetPatch, targetNode);
  deleteNode(targetNode);
}

void audioConnectPatchNode(uint8_t patch, uint8_t node, uint8_t input,
                           uint8_t target, uint8_t output)
{
  logd("Connect node, patch %d, node: %d, input: %d, target: %d, output: %d",
       patch, node, input, target, output);

  Patch *targetPatch = getPatch(patch);
  Node *targetNode = getNodeFromPatch(targetPatch, node);
  Node *targetOutputNode = getNodeFromPatch(targetPatch, target);

  connectNode(targetNode, input, targetOutputNode, output);
}

void audioDisconnectPatchNode(uint8_t patch, uint8_t node, uint8_t input)
{
  logd("Disconnect node, patch %d, node: %d, input: %d", patch, node, input);

  Patch *targetPatch = getPatch(patch);
  Node *targetNode = getNodeFromPatch(targetPatch, node);

  disconnectNode(targetNode, input);
}

void audioSetPatchNodeParam(uint8_t patch, uint8_t node, uint8_t param,
                            float value)
{
  logd("Set patch node param, patch %d, node: %d, param: %d, value: %.4f",
       patch, node, param, value);

  Patch *targetPatch = getPatch(patch);
  Node *targetNode = getNodeFromPatch(targetPatch, node);

  targetNode->params[param] = value;
}

void audioDeletePatchNodes(uint8_t patch)
{
  logd("Delete nodes in patch %d", patch);

  Patch *targetPatch = getPatch(patch);
  deletePatchNodes(targetPatch);
}

Patch *getPatch(uint8_t patch)
{
  if(audioState.patches[patch] == NULL) {
    logd("Allocating new patch at index %d", patch);
    audioState.patches[patch] = allocatePatch(patch, audioState.sampleRate);
  }

  return audioState.patches[patch];
}

void resetTracks()
{
  for(uint8_t i = 0; i < MAX_TRACKS; i++) {
    Track *track = &audioState.tracks[i];
    track->note = PATTERN_NOTE_NONE;
    track->patch = PATTERN_PATCH_NONE;
    track->volume = PATTERN_VOLUME_NONE;
    track->fxType[0] = PATTERN_EFFECT_TYPE_NONE;
    track->fxValue[0] = PATTERN_EFFECT_VALUE_NONE;
    track->fxType[1] = PATTERN_EFFECT_TYPE_NONE;
    track->fxValue[1] = PATTERN_EFFECT_VALUE_NONE;
  }
}

#ifndef PLAYER_ONLY
void resetPatches()
{
  // TODO
  logw("TODO implement reset patches");
}
#endif

// TODO rowCount = 8 bit
void audioSetPatternRowCount(uint8_t pattern, uint16_t rowCount)
{
  Pattern *targetPattern = getPattern(pattern);

  logd("Setting pattern to %d row(s)", rowCount);

  targetPattern->rows = rowCount;
}

void audioSetBPM(uint16_t bpm)
{
  logd("Setting %d bpm", bpm);

  audioState.bpm = bpm;
}

void audioSetLPB(uint8_t lpb)
{
  logd("Setting %d lpb", lpb);

  audioState.lpb = lpb;
}

void audioSetVolume(float volume)
{
  logd("Setting volume %.3f ", volume);

  audioState.volume = volume;
}

void audioSetActiveSequence(uint8_t sequence)
{
  logd("Setting active sequence to %d", sequence);

  audioState.sequence = sequence;
}

#ifndef PLAYER_ONLY

void audioSetRepeatPattern(bool repeatPattern)
{
  logd("Setting repeat pattern to %s", repeatPattern ? "true" : "false");

  audioState.repeatPattern = repeatPattern;
}

#endif

void audioPlayNote(uint8_t patch, uint8_t note, float velocity)
{
  logd("Play note %d velocity %.2f on patch %d", note, velocity, patch);

  Patch *targetPatch = getPatch(patch);
  playNoteOnPatch(targetPatch, note, velocity, audioState.time);
}

void audioStopNote(uint8_t patch, uint8_t note)
{
  logd("Stop note %d on patch %d", note, patch);

  Patch *targetPatch = getPatch(patch);
  stopNoteOnPatch(targetPatch, note);
}

double audioGetStartTime()
{
  return audioState.startTime;
}

void audioPlay()
{
  logd("Play");

#ifndef PLAYER_ONLY
  audioStopAllVoices();
#endif

  audioState.renderOffset = 0;
  audioState.tick = UINT32_MAX;
  audioState.time = 0.0;
  audioState.row = 0;
  audioState.playback = true;

#ifndef PLAYER_ONLY
  audioState.lastSequence = 0xff;
  audioState.infoTime = 0;
#endif

  resetTracks();
#ifndef PLAYER_ONLY
  resetPatches();
#endif
}

void audioStop()
{
  logd("Stop");

#ifndef PLAYER_ONLY
  audioStopAllVoices();

  audioState.playback = false;
#endif
}

#ifndef PLAYER_ONLY

void audioPanic()
{
  logd("Panic");

  audioStopAllVoices();
}

void audioStopAllVoices()
{
  // shut down all voices of all patches
  for(uint8_t i = 0; i < audioState.patchCount; i++) {
    Patch *patch = audioState.patches[i];
    if(patch) {
      stopAllVoicesInPatch(patch);
    }
  }
}

#else

void audioSetSongDurationMs(uint32_t durationMs)
{
  logd("Setting song duration to %d ms", durationMs);

  audioState.durationMs = durationMs;
}

JS_EXPORT uint32_t audioGetSongDurationMs()
{
  return audioState.durationMs;
}

#endif

void initTracks()
{
  audioState.tracks = (Track *)malloc(sizeof(Track) * MAX_TRACKS);
  audioState.trackCount = 0;
}

void deleteTracks()
{
  audioState.trackCount = 0;
  free(audioState.tracks);
  audioState.tracks = NULL;
}

void initSequences()
{
  audioState.sequences = (uint8_t *)malloc(sizeof(uint8_t) * MAX_SEQUENCES);
  audioState.sequenceCount = 0;
}

void deleteSequences()
{
  audioState.sequenceCount = 0;
  free(audioState.sequences);
  audioState.sequences = NULL;
}

void initPatterns()
{
  audioState.patterns = (Pattern **)malloc(sizeof(Pattern *) * MAX_PATTERNS);
  for(uint8_t i = 0; i < MAX_PATTERNS; i++) {
    audioState.patterns[i] = NULL;
  }
}

void deletePatterns()
{
  for(uint8_t i = 0; i < MAX_PATTERNS; i++) {
    if(audioState.patterns[i] != NULL) {
      logd("Deleting pattern at offset %d", i);

      deletePattern(audioState.patterns[i]);
      audioState.patterns[i] = NULL;
    }
  }

  free(audioState.patterns);
  audioState.patterns = NULL;
}

void initPatches()
{
  audioState.patches = (Patch **)malloc(sizeof(Patch *) * MAX_PATCHES);
  for(uint8_t i = 0; i < MAX_PATCHES; i++) {
    audioState.patches[i] = NULL;
  }
}

void deletePatches()
{
  for(uint8_t i = 0; i < MAX_PATCHES; i++) {
    if(audioState.patches[i]) {
      logd("Deleting patch at offset %d", i);

      deletePatch(audioState.patches[i]);
      audioState.patches[i] = NULL;
    }
  }

  free(audioState.patches);
  audioState.patches = NULL;
}

void audioSetSampleRate(uint32_t sampleRate)
{
  logd("Setting sample rate to %dhz", sampleRate);

  audioState.sampleRate = sampleRate;
  audioState.oneOverSampleRate = 1.0 / sampleRate;
}

void audioInit()
{
  logd("Audio init...");

#ifdef __wasm_simd128__
  logd("Audio has SIMD128 enabled.");
#endif

#ifndef PLAYER_ONLY
  audioState.playback = false;
#endif

  audioState.sequence = 0;
  audioState.startTime = 0;

#ifndef PLAYER_ONLY
  audioState.lastSequence = 0xff;
  audioState.repeatPattern = false;
#endif

  initTracks();
  initSequences();
  initPatterns();
  initPatches();

  // logd("MEM: %d byte(s)", totalAllocatedMemory);
}

// TODO remove in PLAYER_ONLY
void audioKill()
{
  logd("Audio kill...");

  deletePatches();
  deletePatterns();
  deleteSequences();
  deleteTracks();
}

void audioProcessEvents()
{
  // calculate tick
  uint16_t bpm = audioState.bpm;
  uint8_t lpb = audioState.lpb;
  float timePerBeat = 60.0 / (double)bpm / (double)lpb;
  float time = audioState.time;
  uint32_t tick = time / timePerBeat;

  // process if new tick
  if(tick != audioState.tick) {
    // logd("New tick");
#ifndef PLAYER_ONLY
    // logd("Row change event to %d", audioState.row);
    audioRowChangeEvent(audioState.row);

    if(audioState.sequence != audioState.lastSequence) {
      audioActiveSequenceChangeEvent(audioState.sequence);
      // logd("Sending sequence: %d", audioState.sequence);
      audioState.lastSequence = audioState.sequence;
    }
#endif
    uint8_t sequencePattern = audioState.sequences[audioState.sequence];
    Pattern *pattern = getPattern(sequencePattern);

    if(pattern->rows > 0) {
      // logd("Sequence: %d, Pattern: %d, Row: %d/%d",
      //         sequence, sequencePattern, row, pattern->rows - 1);

      // generate events from pattern content
      PatternEntry *entry = &pattern->entries[audioState.row * MAX_TRACKS];
      Track *track = audioState.tracks;
      for(uint8_t i = 0; i < audioState.trackCount; i++) {
        if(track->active) {
          if(entry->note != PATTERN_NOTE_NONE) {
            uint8_t currentNote = track->note;
            if(currentNote != PATTERN_NOTE_NONE) {
              // trigger note event (stop)
              // logd("TODO Stop note: %d", entry->note);
              Patch *targetPatch = getPatch(track->patch);
              stopNoteOnPatch(targetPatch, track->note);

              track->note = PATTERN_NOTE_NONE;
            }

            if(entry->note != PATTERN_NOTE_OFF) {
              // in case no patch was provided by the entry we
              // re-use the last one set if available
              uint8_t patch = entry->patch != PATTERN_PATCH_NONE ? entry->patch
                                                                 : track->patch;

              // trigger note event (start)
              // logd("TODO Start note: %d", entry->note);
              Patch *targetPatch = getPatch(patch);

              // TODO put into function
              uint8_t volume =
                  entry->volume != PATTERN_VOLUME_NONE ? entry->volume : 0x7f;
              float velocity = volume / (float)0x7f;
              velocity = clamp(velocity, 0.f, 1.f);

              playNoteOnPatch(targetPatch, entry->note, velocity, time);

              // remember current note
              track->note = entry->note;
              track->patch = patch;
              track->volume = volume;
            }
          }

          if(entry->volume != PATTERN_VOLUME_NONE) {
            // a new volume entry was set, update the track
            track->volume = entry->volume;

            // emit an event to the playing patch but only if no
            // note was set, a set note has already set the correct
            // velocity
            if(track->patch != PATTERN_PATCH_NONE &&
               entry->note == PATTERN_NOTE_NONE) {
              Patch *targetPatch = getPatch(track->patch);

              // TODO put into function
              float velocity = track->volume / (float)0x7f;
              velocity = clamp(velocity, 0.f, 1.f);

              setNoteVelocityOnPatch(targetPatch, track->note, velocity);
            }
          }

          // effect can only be set if there is a patch available
          if(track->patch != PATTERN_PATCH_NONE) {
            Patch *targetPatch = getPatch(track->patch);

            // not to great to have a loop here but that's less code
            // and supposingly this gets eaten by a compiler optimization
            // which thinking about again probably leads to more code :x
            for(uint8_t n = 0; n < 2; n++) {
              if(entry->fxType[n] != PATTERN_EFFECT_TYPE_NONE) {
                // a new fx type was set, update the track
                track->fxType[n] = entry->fxType[n];
              }

              // emit an event to the playing patch but only if
              // there is an actual effect type set and an effect
              // value present
              if(track->fxType[n] != PATTERN_EFFECT_TYPE_NONE &&
                 entry->fxValue[n] != PATTERN_EFFECT_VALUE_NONE) {
                track->fxValue[n] = entry->fxValue[n];

                float fx = track->fxValue[n] / (float)0x7f;
                fx = clamp(fx, 0.f, 1.f);

                setNoteEffectOnPatch(targetPatch, track->note, n,
                                     track->fxType[n], fx);
              }
            }
          }
        }

        entry++;
        track++;
      }
    }

    // advance in pattern
    audioState.row++;

    // end of pattern? next sequence
    if(audioState.row > pattern->rows - 1) {
      audioState.row = 0;

#ifndef PLAYER_ONLY
      if(!audioState.repeatPattern)
#endif
      {
        audioState.sequence++;
        if(audioState.sequence > audioState.sequenceCount - 1) {
#ifndef PLAYER_ONLY
          bool repeatSong = false;
          if(repeatSong) {
            audioState.sequence = 0;
          } else
#endif
          {
            // #ifndef PLAYER_ONLY
            audioState.playback = false;

#ifdef PLAYER_ONLY
            // TODO need to stop all voices here
#endif
            // #endif
            //  TODO this is probably not needed in player mode either
            audioState.sequence = audioState.sequenceCount - 1;
#ifndef PLAYER_ONLY
            audioStopChangeEvent();
#endif
          }
        }
#ifndef PLAYER_ONLY
        // audioActiveSequenceChangeEvent(audioState.sequence);
#endif
      }
    }

    audioState.tick = tick;
  }
}

void audioRenderPatches(float *left, float *right)
{
  *left = *right = 0.f;

#ifndef PLAYER_ONLY
  audioState.activeVoices = 0;
  audioState.activePatches = 0;
#endif

  // render the next sample for all patches
  for(uint8_t i = 0; i < audioState.patchCount; i++) {
    Patch *patch = audioState.patches[i];
    if(patch && patch->out) {
      renderPatch(patch);

      // grab patch output
      *left += patch->left;
      *right += patch->right;

#ifndef PLAYER_ONLY
      audioState.activeVoices += patch->activeVoices;

      // only mark patch as active if it actually has active voices
      if(patch->activeVoices > 0) {
        audioState.activePatches++;
      }
#endif
    }
  }
}

void audioRender(float *left, float *right, uint32_t samples)
{
#ifndef PLAYER_ONLY
  bool clipped = false;
#endif

  while(samples--) {
    // TODO not needed, done in audioRenderPatches already
    // *left = *right = 0.f;
    // #ifndef PLAYER_ONLY
    if(audioState.playback) {
      // #endif
      //  events
      audioProcessEvents();
      // #ifndef PLAYER_ONLY
    }
    // #endif

    audioRenderPatches(left, right);

    // apply global volume
    *left *= audioState.volume;
    *right *= audioState.volume;

#ifndef PLAYER_ONLY
    // check for clipping
    if(*left < -1.f || *left > 1.f || *right < -1.f || *right > 1.f) {
      // TODO apply clipping
      // send clipping notification but only once and then wait for
      // clipping to stop for at least one render quantum
      if(!audioState.clip) {
        logw("Clipping warning");
        audioClipEvent(true);
        audioState.clipTime = audioState.time;
        audioState.clip = true;
      }

      clipped = true;

      *left = clamp(*left, -1.f, 1.f);
      *right = clamp(*right, -1.f, 1.f);
    }
#else
    // always clamp in player mode to save some size
    *left = clamp(*left, -1.f, 1.f);
    *right = clamp(*right, -1.f, 1.f);
#endif

    // advance time and render offset
    audioState.time += audioState.oneOverSampleRate;
    audioState.renderOffset++;

    left++;
    right++;
  }

#ifndef PLAYER_ONLY
  // reset clip if one render quantum was free of it
  if(!clipped && audioState.clip &&
     audioState.time - audioState.clipTime > 1.0) {
    // logd("Reset clipping");
    audioClipEvent(false);
    audioState.clip = false;
  }

  // report amount of active patches and total active voices
  if(audioState.time - audioState.infoTime > 0.5) {
    // logd("Send info");
    audioInfoEvent(audioState.activeVoices, audioState.activePatches);
    audioState.infoTime = audioState.time;
  }
#endif
}
