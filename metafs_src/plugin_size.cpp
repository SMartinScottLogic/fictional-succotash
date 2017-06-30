#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <sys/stat.h>

#include "log.h"
#include "plugin.h"

class plugin_size: public plugin {
  public:
  plugin_size() {
  }

  ~plugin_size() {
  }

  void process(file_entry &file, struct stat &sb) const {
    off_t size = sb.st_size;
    file.attributes["human_size"] = get_human_size(size);
    file.attributes["size"] = get_human_size(size, 0);
    LOG(boost::log::trivial::severity_level::debug, "======== size (%s) = %s (%s)", file.full_path.c_str(), file.attributes["size"].c_str(), file.attributes["human_size"].c_str() );
  }

  void init() {
  }
  protected:
    static std::string get_human_size(off_t _size, int8_t size_code=-1)
    {
      double size = _size;
      std::string name = "";

      if( size_code == -1) {
        size_code=0;
        while(size >= 1024) {
          size_code ++;
          size /= 1024;
        }

        switch(size_code) {
          case 0:
            name = "b";
            break;
          case 1:
            name = "K";
            break;
          case 2:
            name = "M";
            break;
          case 3:
            name = "G";
            break;
          case 4:
            name = "T";
            break;
          default:
            size = _size;
            name = "b";
            break;
        }
      }

      char s[1024];
      if(size_code == 0) {
        snprintf(s, 1024, "%0.0lf%s", size, name.c_str() );
      } else {
        snprintf(s, 1024, "%0.1lf%s", size, name.c_str());
      }
      return s;
    }

};
plugin_size g_plugin_size;

