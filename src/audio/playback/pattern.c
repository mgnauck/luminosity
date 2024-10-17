
#include "pattern.h"
#include "../../base/walloc.h"
#include <stddef.h>

PatternEntry EMPTY_PATTERN_ENTRY = {
    .note = PATTERN_NOTE_NONE,
    .patch = PATTERN_PATCH_NONE,
    .volume = PATTERN_VOLUME_NONE,
    .fxType[0] = PATTERN_EFFECT_TYPE_NONE,
    .fxValue[0] = PATTERN_EFFECT_VALUE_NONE,
    .fxType[1] = PATTERN_EFFECT_TYPE_NONE,
    .fxValue[1] = PATTERN_EFFECT_VALUE_NONE,
};

Pattern *allocatePattern()
{
  Pattern *pattern = (Pattern *)malloc(sizeof(Pattern));

  pattern->entries = (PatternEntry *)malloc(
      MAX_PATTERN_ROWS * MAX_PATTERN_COLUMNS * sizeof(PatternEntry));

  for(uint32_t i = 0; i < MAX_PATTERN_ROWS * MAX_PATTERN_COLUMNS; i++) {
    pattern->entries[i] = EMPTY_PATTERN_ENTRY;
  }

  pattern->rows = 0;

  return pattern;
}

void deletePattern(Pattern *pattern)
{
  free(pattern->entries);
  free(pattern);
}

// PatternEntry *getEntryFromPattern(Pattern* pattern, uint8_t column, uint16_t
// row)
PatternEntry *getEntryFromPattern(Pattern *pattern, uint8_t column, uint8_t row)
{
  return &pattern->entries[row * MAX_PATTERN_COLUMNS + column];
}
