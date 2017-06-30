#include "log.h"
#include <cstdarg>
#include <syslog.h>

namespace tagfs {
log g_log;

log::log()  {
  openlog("tagfs", 0, LOG_USER);
}

log::~log() {
  closelog();
}

void log::printf(const char *format, ...) const {
  va_list vl;
  va_start(vl,format);
  vsyslog(LOG_USER | LOG_INFO, format, vl);
  va_end(vl);
}
};

