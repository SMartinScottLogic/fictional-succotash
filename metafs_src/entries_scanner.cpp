#include "plugin.h"
#include "entries_scanner.h"
#include "log.h"

std::string string_format(const std::string &fmt, ...) {
  int size=100;
  std::string str;
  va_list ap;

  while (1) {
    str.resize(size);
    va_start(ap, fmt);
    int n = vsnprintf(&str[0], size, fmt.c_str(), ap);
    va_end(ap);

    if (n > -1 && n < size)
      return str;
    if (n > -1)
      size = n + 1;
    else
      size *= 2;
  }
}

void add_file(Entries *const entries, const file_entry &file, const std::list<pattern_component> &pattern_parts) {
  LOG(boost::log::trivial::severity_level::debug, "%s: '%s' as '%s'", __PRETTY_FUNCTION__, file.full_path.c_str(), file.name.c_str() );
  
  std::string cur_part = "";
  std::list<std::string> path_parts;
  for(const pattern_component &part : pattern_parts) {
    std::string key = "";
    switch(part.type) {
    case METADATA:
      key = file.attributes.at(part.name);
      break;
    case LITERAL:
      key = part.name;
      break;
    case PATH_SEPARATOR:
      if(cur_part.length() > 0) {
        path_parts.push_back(cur_part);
      }
      cur_part = "";
      break;
    default:
      key = std::string("{") + std::to_string(part.type) + std::string(": ") + part.name + std::string("}");
      break;
    }
    if(key.length() > 0) {
      cur_part.append(key);
    }
  }
  if(cur_part.length() > 0) {
    path_parts.push_back(cur_part);
  }

  auto cur = entries->get_store();
  for(auto key : path_parts) {
    size_t string_index = entries->get_string_index(key);
    if(cur->children.find(string_index) == cur->children.end()) {
      cur->children[string_index] = new path_component();
    }
    cur = cur->children[string_index];
  }
  cur->files.push_back(file);
}

void scan_path(Entries *const entries, const std::string &path, const std::list<pattern_component> &pattern_parts) {
  DIR *dir;
  struct dirent *entry;
  static size_t num_files = 0;

  if( !(dir = opendir(path.c_str())) ) {
    perror((std::string("opendir (")+path+std::string(")")).c_str());
    return;
  }

  std::set<std::string> sub_paths;
  for( entry = readdir(dir); entry; entry = readdir(dir) ) {
    if( entry->d_type == DT_REG) {
      std::string full_path = path + "/" + entry->d_name;

      file_entry file;
      file.full_path = full_path;
      do {
        plugin::handle(file);

        if(file.deleted) {
          break;
        }
        std::string ext;
        if( file.attributes.find("magic_ext")!=file.attributes.end() ) {
          ext = file.attributes["magic_ext"];
        }
        file.name = string_format("%s_%zu.%s", entry->d_name, num_files, ext.c_str());

        if( num_files % 10000 == 0) {
          fprintf(stderr, "\n%zu scan_path ", num_files);
        }
        if( num_files % 100 == 0) {
          fprintf(stderr, ".");
        }
        add_file( entries, file, pattern_parts );
        num_files ++;
      } while(false);
    }
    if( entry->d_type==DT_DIR && entry->d_name[0]!='.' && strcmp( ".", entry->d_name)!=0 && strcmp("..", entry->d_name)!=0 ) {
      sub_paths.insert(path + "/" + entry->d_name);
    }
  }
  closedir( dir);

  for(auto path : sub_paths) {
    scan_path(entries, path, pattern_parts);
  }
}

void scan(Entries *const entries, const std::list<std::string> &paths, const std::list<pattern_component> &pattern_parts) {
  LOG(0, "%s: '%p'", __PRETTY_FUNCTION__, entries->get_store() );
  for(auto path : paths) {
    scan_path(entries, path, pattern_parts);
  }
  boost::this_thread::sleep_for(boost::chrono::seconds(60));
  LOG(0, "%s: done", __PRETTY_FUNCTION__ );
}

