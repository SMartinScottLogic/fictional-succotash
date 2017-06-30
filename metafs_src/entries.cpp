#include <cstdarg>
#include <cassert>
#include "entries.h"
#include "entries_scanner.h"
#include "log.h"
#include "metafs_config.hh"

#define DEFAULT_META_PATH "/{magic}/{human_size}/"

file_entry DELETED_FILE = file_entry(true);

file_entry::file_entry()
: name(), full_path()
, attributes(), deleted(false)
{}

file_entry::file_entry(bool _deleted)
: name(), full_path()
, attributes(), deleted(_deleted)
{}

std::list<pattern_component> parse_meta_pattern(const std::string &pattern_str) {
  bool in_meta = false;
  std::list<pattern_component> pattern_parts;
  pattern_component cur_part = {"", LITERAL};
  for(auto ch : pattern_str) {
    switch(ch) {
      case '{':
        if(in_meta) {
          cur_part.name.push_back(ch);
          break;
        }
        if(cur_part.name.length() > 0) {
          pattern_parts.push_back(cur_part);
        }
        in_meta = true;
        cur_part.name = "";
        cur_part.type = METADATA;
        break;
      case '}':
        if(in_meta) {
          if(cur_part.name.length() > 0) {
            pattern_parts.push_back(cur_part);
          }
          in_meta = false;
          cur_part.name = "";
          cur_part.type = LITERAL;
          break;
        }
        break;
      case '/':
        if(cur_part.name.length() > 0) {
          pattern_parts.push_back(cur_part);
        }
        pattern_parts.push_back({"", PATH_SEPARATOR});
        in_meta = false;
        cur_part.name = "";
        cur_part.type = LITERAL;
        break;
      default:
        cur_part.name.push_back(ch);
    }
  }
  
  if(cur_part.name.length() > 0) {
    pattern_parts.push_back(cur_part);
  }
  return pattern_parts;
}

Entries::Entries()
: string_index(), string_reverse_index(), store(nullptr), num_files(0)
{
}

void Entries::config_update(void) {
  LOG(0, "%s", __PRETTY_FUNCTION__ );
  auto config = Config::Instance()->get_config();
  LOG(0, "%s: '%s'", __PRETTY_FUNCTION__, config.meta_pattern.c_str() );
  //std::list<pattern_component> pattern_parts = {{"magic",LITERAL},{"magic",METADATA},{"human_size",METADATA}};
  //std::list<pattern_component> pattern_parts = {{"magic",METADATA},{"human_size",METADATA}};
  //std::list<pattern_component> pattern_parts = {{"magic",METADATA}};
  //std::list<pattern_component> pattern_parts = {{"magic",LITERAL}};

  auto pattern_parts = parse_meta_pattern(config.meta_pattern);
  for(auto part : pattern_parts) {
    LOG(0, "%s: (%d, '%s')", __PRETTY_FUNCTION__, part.type, part.name.c_str() );
  }

  if(store == nullptr) {
    store = new path_component();
  }

  boost::thread scanner_thread(scan, this, config.paths, pattern_parts);
  //scanner_thread.join();
}

Entries::~Entries() {
  if(store != nullptr) {
    delete store;
    store = nullptr;
  }
}

void Entries::dump(const file_entry &file, int indent) const {
  std::string indentor = std::string(indent, ' ');
  /*
  for(auto attrib : file.attributes) {
    fprintf(stderr, "%s'%s' : '%s'\n", indentor.c_str(), attrib.first.c_str(), attrib.second.c_str());
  }
  */
  fprintf( stderr, "%s", indentor.c_str() );
  for(auto attrib : file.attributes) {
    fprintf( stderr, "%s(%s) ", attrib.first.c_str(), attrib.second.c_str() );
  }
  fprintf( stderr, "\n" );
}

void Entries::dump(const path_component &store, int indent) const {
  std::string indentor = std::string(indent, ' ');
  for(auto child : store.children) {
    fprintf(stderr, "%s%s/\n", indentor.c_str(), string_reverse_index.at(child.first).c_str());
    dump(*(child.second), indent+2);
  }
  for(auto file : store.files) {
    fprintf(stderr, "%s%s\n", indentor.c_str(), file.name.c_str() );
    dump(file, indent+2);
  }
}

path_component *Entries::walk_tree(const std::list<std::string> &path_parts) const {
  auto cur = store;
  size_t string_index;
  for(auto part : path_parts) {
    LOG(boost::log::trivial::severity_level::debug, "%s: '%s'", __PRETTY_FUNCTION__, part.c_str() );
    if(cur == nullptr) {
      return nullptr;
    }

    try {
      string_index = get_string_index(part);
    } catch (const std::out_of_range& oor) {
      LOG(0, "%s: '%s' on '%s'", __PRETTY_FUNCTION__, oor.what(), part.c_str() );
      return nullptr;
    }
    auto found = cur->children.find(string_index);
    if(found==cur->children.end()) {
      return nullptr;
    }
    cur = found->second;
  }
  return cur;
}

void Entries::delete_file(const std::list<std::string> &path_parts, const std::string &name) {
  LOG(0, "%s", __PRETTY_FUNCTION__ );
  auto cur = walk_tree(path_parts);
  if(cur == nullptr) {
    return;
  }
  
  for(auto &file : cur->files) {
    LOG(boost::log::trivial::severity_level::debug, "%s: '%s' vs '%s'", __PRETTY_FUNCTION__, file.name.c_str(), name.c_str() );
    if(strcmp(file.name.c_str(), name.c_str()) == 0) {
      LOG(boost::log::trivial::severity_level::info, "%s: DELETED '%s' => '%s'", __PRETTY_FUNCTION__, file.name.c_str(), file.full_path.c_str() );
      file.deleted = true;
      return;
    }
  }
}

const file_entry &Entries::get_file(const std::list<std::string> &path_parts, const std::string &name) const {
  LOG(0, "%s", __PRETTY_FUNCTION__ );
  auto cur = walk_tree(path_parts);
  if(cur == nullptr) {
    return DELETED_FILE;
  }
  
  for(auto &file : cur->files) {
    LOG(boost::log::trivial::severity_level::debug, "%s: '%s' vs '%s'", __PRETTY_FUNCTION__, file.name.c_str(), name.c_str() );
    if(strcmp(file.name.c_str(), name.c_str()) == 0) {
      return file;
    }
  }
  return DELETED_FILE;
}

std::set<std::string> Entries::get_children(const std::list<std::string> &path_parts) const {
  LOG(0, "%s", __PRETTY_FUNCTION__ );
  std::set<std::string> values;

  auto cur = walk_tree(path_parts);
  if(cur == nullptr) {
    return values;
  }
  for(auto child : cur->children) {
    values.insert(string_reverse_index.at(child.first));
  }
  return values;
}

std::list<file_entry> Entries::get_files(const std::list<std::string> &path_parts) const {
  LOG(boost::log::trivial::severity_level::debug, "%s", __PRETTY_FUNCTION__ );

  std::list<file_entry> matches;
  auto cur = walk_tree(path_parts);
  if(cur == nullptr) {
    return matches;
  }
  for(auto file : cur->files) {
    if(!file.deleted) {
      matches.push_back(file);
    }
  }
  return matches;
}

bool Entries::path_exists(const std::list<std::string> &path_parts) const {
  auto cur = store;
  size_t string_index;
  for(auto part : path_parts) {
    try {
      string_index = get_string_index(part);
    } catch (const std::out_of_range& oor) {
      LOG(boost::log::trivial::severity_level::info, "%s: '%s' on '%s'", __PRETTY_FUNCTION__, oor.what(), part.c_str() );
      return false;
    }
    auto next = cur->children.find(string_index);
    if(next == cur->children.end()) {
      LOG(boost::log::trivial::severity_level::info, "%s: '%s' path component MISSING", __PRETTY_FUNCTION__, part.c_str() );
      return false;
    }
    LOG(boost::log::trivial::severity_level::debug, "%s: '%s' path component PRESENT", __PRETTY_FUNCTION__, part.c_str() );
    cur = next->second;
  }
  return true;
}

size_t Entries::get_string_index(const std::string &key) {
  auto it = string_index.find(key);
  if(it!=string_index.end()) {
    return it->second;
  }
  size_t index = string_index.size();
  string_index[key] = index;
  string_reverse_index[index] = key;
  return index;
}

size_t Entries::get_string_index(const std::string &key) const {
  return string_index.at(key);
}

