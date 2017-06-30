#ifdef HAVE_CONFIG_H
# include "config.h"
# ifdef HAVE_LIBMAGIC
#  include <magic.h>
# endif
#endif

#include <string>
#include "log.h"
#include "plugin.h"

#ifdef HAVE_LIBMAGIC
class plugin_magic: public plugin {
  public:
  plugin_magic(): m_magic(NULL) {
  }
  plugin_magic(const plugin_magic&)=delete;
  void operator=(const plugin_magic&)=delete;

  ~plugin_magic() {
    if(m_magic != NULL) {
      magic_close(m_magic);
    }
  }

  void process(file_entry &file, struct stat &) const {
    std::string magic = magic_file(m_magic, file.full_path.c_str());
    LOG(boost::log::trivial::severity_level::debug, "======== MAGIC (%s) = %s", file.full_path.c_str(), magic.c_str() );
    do {
      auto pos = magic.find_last_of("/");
      if(pos != magic.npos) {
        pos ++;
        file.attributes["magic_ext"] = magic.substr(pos);
      }
    } while(false);
    file.attributes["magic"] = clean(magic);
  }

  void init() {
    m_magic = magic_open(MAGIC_MIME_TYPE);
    if(magic_load(m_magic, NULL)==-1) {
      LOG(boost::log::trivial::severity_level::error, "MAGIC ERROR = %s", magic_error(m_magic) );
    }
  }
  protected:
    magic_t m_magic;
};
plugin_magic g_plugin_magic;
#else
#error no LIBMAGIC
#endif

