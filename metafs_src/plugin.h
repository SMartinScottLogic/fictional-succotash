#ifndef PLUGIN_H
#define PLUGIN_H

#include <set>
#include <map>
#include <string>

#include <sys/stat.h>

#include "entries.h"

class plugin {
  public:
    virtual void init();
    virtual void process(file_entry &file, struct stat &sb) const=0;
     
    virtual ~plugin();      
    plugin();
      
    static void handle(file_entry &file);
    static void initialize();
  protected:
    std::string clean(const std::string &src) const;
    static std::multimap<int, plugin *> *m_factories;
};

#endif//PLUGIN_H
