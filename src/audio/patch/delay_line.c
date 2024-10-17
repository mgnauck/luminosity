
#include "delay_line.h"
#include "../../base/math.h"
#include "../../base/string.h"
#include "../../base/walloc.h"
#include <stddef.h>

DelayLine *allocateDelayLine(uint32_t maxSamples)
{
  DelayLine *delayLine = (DelayLine *)malloc(sizeof(DelayLine));

  delayLine->maxSamples = maxSamples;
  delayLine->buffer = malloc(sizeof(int16_t) * delayLine->maxSamples);

  // TODO/FIXME shouldn't be required as we should be calling resetDelayLine
  // anyway before using it delayLine->samples = samples; delayLine->writeOffset
  // = 0;

  return delayLine;
}

void freeDelayLine(DelayLine *delayLine)
{
  free(delayLine->buffer);
}

void resetDelayLine(DelayLine *delayLine)
{
  delayLine->writeOffset = 0;
  memset(delayLine->buffer, 0, sizeof(int16_t) * delayLine->maxSamples);
}

float delayLineWriteAndRead(DelayLine *delayLine, float in)
{
  // clamp to avoid overflows for int16_t
  in = clamp(in, -1.f, 1.f);

  int32_t readOffset =
      (delayLine->writeOffset - delayLine->samples + delayLine->maxSamples) %
      delayLine->maxSamples;

  float out = delayLine->buffer[readOffset] / 32767.0f;

  delayLine->buffer[delayLine->writeOffset] = (int16_t)(in * 32767.0f);
  delayLine->writeOffset = (delayLine->writeOffset + 1) % delayLine->maxSamples;

  return out;
}
