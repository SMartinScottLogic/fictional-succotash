#include <algorithm>
#include "plugin.h"
#include "log.h"

std::multimap<int, plugin *> *plugin::m_factories=NULL;

plugin::plugin() {
  if(m_factories==NULL) {
    m_factories = new std::multimap<int, plugin*>();
  }
  m_factories->insert(std::pair<int, plugin*>(0, this));
}

plugin::~plugin() {
  if(m_factories!=NULL) {
    delete(m_factories);
    m_factories = NULL;
  }
}

void plugin::handle(file_entry &file) {
  if(m_factories==NULL) return;
 
  struct stat sb;
  if(stat( file.full_path.c_str(), &sb)==-1) {
    LOG(boost::log::trivial::severity_level::error, "ERROR (%s): treating as deleted. Error %s(%d)", file.full_path.c_str(), strerror(errno), errno);
    file.deleted = true;
    return;
  }

  for(auto it=m_factories->begin(); it!=m_factories->end(); ++it) {
    it->second->process(file, sb);
  }
}

std::string plugin::clean(const std::string &unclean) const {
  std::string clean;
  std::transform(unclean.begin(), unclean.end(), std::back_inserter(clean), [](unsigned char ch) {return (ch=='/'?'_':ch);});
  return clean;
}

void plugin::initialize() {
  if(m_factories==NULL) return;

  for(auto it=m_factories->begin(); it!=m_factories->end(); ++it) {
    it->second->init();
  }
}

void plugin::init()
{
}
