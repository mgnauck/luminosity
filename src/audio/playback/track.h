
#ifndef __TRACK_H
#define __TRACK_H

#include "../../base/base.h"
#include <stdint.h>

typedef struct {
  // TODO could be removed in player mode, should be handled by the song
  // exported to discard inactive tracks
  bool active;
  uint8_t note;
  uint8_t patch;
  uint8_t volume;
  uint8_t fxType[2];
  uint8_t fxValue[2];
} Track;

#endif
