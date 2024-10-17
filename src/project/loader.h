
#ifndef __LOADER_H
#define __LOADER_H

#include "../base/base.h"
#include <stdint.h>

#ifdef PLAYER_ONLY

JS_EXPORT void projectInit(uint32_t sampleRate, uint8_t sequence,
                           uint8_t *project);
JS_EXPORT void projectReset(uint8_t sequence);

#endif

#endif
