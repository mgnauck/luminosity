
#ifndef LOG_H
#define LOG_H

#include "base.h"
#include <stddef.h>

#ifndef PLAYER_ONLY_NO_LOG
enum {
  LOG_TRACE = 0x01,
  LOG_DEBUG = 0x02,
  LOG_INFO = 0x03,
  LOG_WARN = 0x04,
  LOG_ERROR = 0x05,
};
#endif

#include "walloc.h"

// TODO handle undef for size
#ifndef NO_LOG
#define logd(...) log_msg(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define logw(...) log_msg(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define loge(...) log_msg(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
// #define logc(...) log_msg(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define logc(...) log_msg_less(LOG_DEBUG, __VA_ARGS__)
#else
#define logd(...)
#define logw(...)
#define loge(...)
#define logc(...)
#endif

void log_msg(int level, const char *file, int line, const char *fmt, ...);
void log_msg_less(int level, const char *fmt, ...);

JS_IMPORT("console", extern void console(int level, char *, size_t len));

#endif
