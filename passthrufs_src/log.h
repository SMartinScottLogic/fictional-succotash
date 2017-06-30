#ifndef TAGFS_LOG_H
#define TAGFS_LOG_H

namespace tagfs
{
class log {
  public:
     log();
     ~log();
     void printf(const char*, ...) const;   
};

extern log g_log;
};

#define LOG(msg, ...) do {tagfs::g_log.printf("%s(%d): " msg, __FILE__, __LINE__, ##__VA_ARGS__);} while(false)
#endif//TAGFS_LOG_H
