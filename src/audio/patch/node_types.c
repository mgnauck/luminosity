
#include "node_types.h"
#include "adsr.h"
#include "ahdsr.h"
#include "clamp.h"
#include "delay.h"
#include "dist.h"
#include "filter.h"
#include "freq_norm.h"
#include "invert.h"
#include "mix.h"
#include "mul.h"
#include "noise.h"
#include "noise2.h"
#include "note.h"
#include "offset.h"
#include "osc.h"
#include "out.h"
#include "reverb.h"
#include "scale.h"
#include "transpose.h"
#include <stddef.h>

NodeType nodeTypes[] = {
#if !PLAYER_ONLY || SUPPORT_OSC
    {.type = NODE_TYPE_OSC, .renderFunc = renderOsc},
#endif
#if !PLAYER_ONLY || SUPPORT_OUT
    {.type = NODE_TYPE_OUT, .renderFunc = renderOut},
#endif
#if !PLAYER_ONLY || SUPPORT_NOTE
    {.type = NODE_TYPE_NOTE, .renderFunc = renderNote},
#endif
#if !PLAYER_ONLY || SUPPORT_NOISE
    {.type = NODE_TYPE_NOISE, .renderFunc = renderNoise},
#endif
#if !PLAYER_ONLY || SUPPORT_NOISE2
    {.type = NODE_TYPE_NOISE2, .renderFunc = renderNoise2},
#endif
#if !PLAYER_ONLY || SUPPORT_AHDSR
    {.type = NODE_TYPE_AHDSR, .renderFunc = renderAHDSR},
#endif
#if !PLAYER_ONLY || SUPPORT_ADSR
    {.type = NODE_TYPE_ADSR, .renderFunc = renderADSR},
#endif
#if !PLAYER_ONLY || SUPPORT_FILTER
    {.type = NODE_TYPE_FILTER, .renderFunc = renderFilter},
#endif
#if !PLAYER_ONLY || SUPPORT_DELAY
    {.type = NODE_TYPE_DELAY,
     .initFunc = initDelay,
#ifndef PLAYER_ONLY
     .freeFunc = freeDelay,
#endif
     .renderFunc = renderDelay},
#endif
#if !PLAYER_ONLY || SUPPORT_MUL
    {.type = NODE_TYPE_MUL, .renderFunc = renderMul},
#endif
#if !PLAYER_ONLY || SUPPORT_SCALE
    {.type = NODE_TYPE_SCALE, .renderFunc = renderScale},
#endif
#if !PLAYER_ONLY || SUPPORT_OFFSET
    {.type = NODE_TYPE_OFFSET, .renderFunc = renderOffset},
#endif
#if !PLAYER_ONLY || SUPPORT_INVERT
    {.type = NODE_TYPE_INVERT, .renderFunc = renderInvert},
#endif
#if !PLAYER_ONLY || SUPPORT_MIX
    {.type = NODE_TYPE_MIX, .renderFunc = renderMix},
#endif
#if !PLAYER_ONLY || SUPPORT_FREQ_NORM
    {.type = NODE_TYPE_FREQ_NORM, .renderFunc = renderFreqNorm},
#endif
#if !PLAYER_ONLY || SUPPORT_DIST
    {.type = NODE_TYPE_DIST, .renderFunc = renderDist},
#endif
#if !PLAYER_ONLY || SUPPORT_TRANSPOSE
    {.type = NODE_TYPE_TRANSPOSE, .renderFunc = renderTranspose},
#endif
#if !PLAYER_ONLY || SUPPORT_CLAMP
    {.type = NODE_TYPE_CLAMP, .renderFunc = renderClamp},
#endif
#if !PLAYER_ONLY || SUPPORT_REVERB
    {.type = NODE_TYPE_REVERB,
     .initFunc = initReverb,
#ifndef PLAYER_ONLY
     .freeFunc = freeReverb,
#endif
     .renderFunc = renderReverb},
#endif
};

NodeType *getNodeType(uint8_t type)
{
  for(uint32_t i = 0; i < sizeof nodeTypes / sizeof nodeTypes[0]; i++) {
    struct NodeType *nodeType = &nodeTypes[i];
    if(nodeType->type == type) {
      return nodeType;
    }
  }

  return NULL;
}
