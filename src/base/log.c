
#include "log.h"
#include "printf.h"
#include "string.h"
#include <stdarg.h>

/*
static const char *levelStrings[] = {
    "TRC", "DBG", "INF", "WRN", "ERR", "FTL"};
*/

// TODO should better be reduced to a meaningful limit
static char buffer1[65535];
static char buffer2[65535];

void log_msg(int level, const char *file, int line, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  vsnprintf(buffer1, sizeof(buffer1), fmt, ap);
  sprintf(buffer2, "%s:%d: %s", file, line, buffer1);
  console(level, buffer2, strlen(buffer2));

  va_end(ap);
}

void log_msg_less(int level, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  vsnprintf(buffer1, sizeof(buffer1), fmt, ap);
  console(level, buffer1, strlen(buffer1));

  va_end(ap);
}
