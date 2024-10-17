
#include "loader.h"
#include "../audio/audio.h"
#include "../base/log.h"
#include "../base/stream.h"
#include "../base/walloc.h"
#include <stddef.h>

#ifdef PLAYER_ONLY

#ifdef PROJECT_DATA
// hard-coded, pull in the actual exported binary project as header
#include "data.h"
#else
unsigned char project_bkpo[] = {0};
#endif

#define NODE_INPUTS MAX_PATCH_NODES *MAX_NODE_INPUTS

void projectLoadSong(Stream *stream)
{
  logd("Loading song...");

  // tracks
  uint8_t trackCount = streamReadUInt8(stream);
  audioSetTrackCount(trackCount);
  for(uint8_t i = 0; i < trackCount; i++) {
    uint8_t active = streamReadUInt8(stream);
    audioSetTrackActive(i, active);
  }

  // sequences
  uint8_t sequenceCount = streamReadUInt8(stream);
  audioSetSequenceCount(sequenceCount);
  for(uint8_t i = 0; i < sequenceCount; i++) {
    uint8_t pattern = streamReadUInt8(stream);
    audioSetSequence(i, pattern);
  }

  // patterns
  uint8_t patternCount = streamReadUInt8(stream);
  logd("Pattern count is %d", patternCount);
  for(uint8_t i = 0; i < patternCount; i++) {
    uint8_t index = streamReadUInt8(stream);
    // uint16_t rowCount = streamReadUInt16(stream);
    uint8_t rowCount = streamReadUInt8(stream);
    uint16_t entryCount = streamReadUInt16(stream);

    logd("Pattern [%d] index is %d with %d row(s) and %d entries", i, index,
         rowCount, entryCount);

    audioSetPatternRowCount(index, rowCount);

    for(uint16_t j = 0; j < entryCount; j++) {
      uint8_t column = streamReadUInt8(stream);
      // uint16_t row = streamReadUInt16(stream);
      uint8_t row = streamReadUInt8(stream);

      uint8_t note = streamReadUInt8(stream);
      uint8_t patch = streamReadUInt8(stream);
      uint8_t volume = streamReadUInt8(stream);
      uint8_t fx1type = streamReadUInt8(stream);
      uint8_t fx1value = streamReadUInt8(stream);
      uint8_t fx2type = streamReadUInt8(stream);
      uint8_t fx2value = streamReadUInt8(stream);

      /*
      logd("Entry [%d,%d] with note:%d, patch:%d, volume:%d, fx1:%d/%d,
      fx2:%d/%d", column, row, note, patch, volume, fx1type, fx1value, fx2type,
      fx2value);
      */

      // set in pattern
      audioSetPattern(index, column, row, note, patch, volume, fx1type,
                      fx1value, fx2type, fx2value);
    }
  }

  // patches
  uint8_t patchCount = streamReadUInt8(stream);
  logd("Patch count is %d", patchCount);

  // TODO/FIXME this needs to be changed if the default in the song.js model
  // changes
  audioSetPatchCount(16); // static for now

  for(uint8_t i = 0; i < patchCount; i++) {
    uint8_t index = streamReadUInt8(stream);
    uint8_t nodeCount = streamReadUInt8(stream);

    // logd("Patch [%d] index is %d and %d node(s)", i, index, nodeCount);

    typedef struct {
      uint8_t nodeId;
      uint8_t input;
      uint8_t target;
      uint8_t output;
    } LoaderNodeInput;

    LoaderNodeInput loaderNodeInputs[NODE_INPUTS];
    uint16_t loaderNodeInputsUsed = 0;

    // first pass create nodes and set params
    for(uint8_t j = 0; j < nodeCount; j++) {
      uint8_t id = streamReadUInt8(stream);
      uint8_t type = streamReadUInt8(stream);

      // logd("Node [%d] id is %d type is %d", i, id, type);

      audioCreatePatchNode(index, id, type);

      // params
      uint8_t paramCount = streamReadUInt8(stream);
      // logd("Params %d total", paramCount);
      for(uint8_t k = 0; k < paramCount; k++) {
        uint8_t paramIndex = streamReadUInt8(stream);
        float value = streamReadFloat32(stream);

        // logd("Params with index %d has value %f", index, value);

        audioSetPatchNodeParam(index, id, paramIndex, value);
      }

      // inputs
      // TODO/FIXME cannot make connections here because the
      // target nodes might not yet exist :x
      uint8_t inputCount = streamReadUInt8(stream);
      logd("Inputs %d total", inputCount);
      for(uint8_t k = 0; k < inputCount; k++) {
        uint8_t index = streamReadUInt8(stream);
        uint8_t target = streamReadUInt8(stream);
        uint8_t output = streamReadUInt8(stream);

        logd("Inputs with index %d has target %d output %d", index, target,
             output);

        LoaderNodeInput loaderNodeInput = {
            .nodeId = id,
            .input = index,
            .target = target,
            .output = output,
        };

        loaderNodeInputs[loaderNodeInputsUsed++] = loaderNodeInput;
      }
    }

    // second pass connect nodes
    for(uint8_t j = 0; j < loaderNodeInputsUsed; j++) {
      LoaderNodeInput loaderNodeInput = loaderNodeInputs[j];

      audioConnectPatchNode(index, loaderNodeInput.nodeId,
                            loaderNodeInput.input, loaderNodeInput.target,
                            loaderNodeInput.output);
    }
  }
}

void projectInit(uint32_t sampleRate, uint8_t sequence, uint8_t *project)
{
  logd("Initializing project...");

  audioSetSampleRate(sampleRate);

  logd("Loading project...");

  if(project != NULL) {
    logd("Loading external project...");
  }

  Stream *stream = streamAllocate(project ? project : project_bkpo);

  audioInit();

  // global project level
// #define BRING_YOUR_VACUUM
#ifndef BRING_YOUR_VACUUM
  uint16_t bpm = streamReadUInt16(stream);
  uint8_t lpb = streamReadUInt8(stream);
  float volume = streamReadFloat32(stream);
  uint32_t durationMs = streamReadUInt32(stream);

  audioSetBPM(bpm);
  audioSetLPB(lpb);
  audioSetVolume(volume);
  audioSetSongDurationMs(durationMs);
#else
  streamRead(stream, (uint8_t *)&audioState.bpm,
             sizeof(uint16_t) + sizeof(uint8_t) + sizeof(float) +
                 sizeof(uint32_t));
#endif

  // song level
  projectLoadSong(stream);

  audioSetActiveSequence(sequence);

  // calculate start time from sequence
  uint32_t rows = 0;
  for(uint8_t i = 0; i < sequence; i++) {
    // get pattern from sequence
    uint8_t pattern = audioState.sequences[sequence];
    Pattern *targetPattern = getPattern(pattern);
    rows += targetPattern->rows;
  }

  double timePerBeat = 60.0 / (double)bpm / (double)lpb;
  audioState.startTime = timePerBeat * rows;

  logd("Start sequence is %d, absolute row is %d, startTime: %.3f", sequence,
       rows, audioState.startTime);

  audioPlay();
}

void projectReset(uint8_t sequence)
{
  logd("Reset project to sequence %d", sequence);

  audioState.sequence = sequence;
  audioState.row = 0;
}

#endif
