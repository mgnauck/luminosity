
#ifndef __DELAY_LINE_H
#define __DELAY_LINE_H

#include <stdint.h>

typedef struct {
  int16_t *buffer; // TODO short
  uint32_t maxSamples;
  uint32_t samples;
  uint32_t writeOffset;
} DelayLine;

DelayLine *allocateDelayLine(uint32_t maxSamples);
void freeDelayLine(DelayLine *delayLine);
void resetDelayLine(DelayLine *delayLine);
float delayLineWriteAndRead(DelayLine *delayLine, float input);

#endif
