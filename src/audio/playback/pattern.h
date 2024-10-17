
#ifndef __PATTERN_H
#define __PATTERN_H

#include "../../base/base.h"
#include <stdint.h>

#define MAX_PATTERN_COLUMNS       32
#define MAX_PATTERN_ROWS          256
#define PATTERN_NOTE_OFF          0x80
#define PATTERN_NOTE_NONE         0x81
#define PATTERN_PATCH_NONE        0xff
#define PATTERN_VOLUME_NONE       0xff
#define PATTERN_EFFECT_TYPE_NONE  0xf
#define PATTERN_EFFECT_VALUE_NONE 0xff

typedef struct {
  uint8_t note;
  uint8_t patch;
  uint8_t volume;
  uint8_t fxType[2];
  uint8_t fxValue[2];
} PatternEntry;

typedef struct {
  // uint16_t rows;
  uint8_t rows;
  PatternEntry *entries;
} Pattern;

extern PatternEntry EMPTY_PATTERN_ENTRY;

Pattern *allocatePattern();
void deletePattern(Pattern *pattern);
// PatternEntry *getEntryFromPattern(Pattern *pattern, uint8_t column, uint16_t
// row);
PatternEntry *getEntryFromPattern(Pattern *pattern, uint8_t column,
                                  uint8_t row);

#endif
