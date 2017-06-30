#ifndef ENTRIES_H
#define ENTRIES_H

#include <dirent.h>
#include <cstring>

#include <string>
#include <map>
#include <set>
#include <list>

struct file_entry {
  file_entry();
  file_entry(bool _deleted);

  std::string name;
  std::string full_path;

  std::map<std::string, std::string> attributes;
  bool deleted;
};

struct path_component {
  path_component(): children(), files() {
  }
  path_component(const path_component &)=delete;
  path_component &operator=(const path_component &)=delete;
  ~path_component() {
    for(auto child : children) {
      delete child.second;
    }
  }

  std::map<size_t, path_component *> children;
  std::list<file_entry> files;
};

enum pattern_component_type {
  METADATA, LITERAL, PATH_SEPARATOR
};

struct pattern_component {
  std::string name;
  pattern_component_type type;
};

class Entries {
  public:
    typedef uint64_t nodeid;

    Entries();
    Entries(const Entries &)=delete;
    Entries &operator=(const Entries &)=delete;
    ~Entries();

    void config_update(void);

    path_component *get_store() const {
      return this->store;
    }
    size_t get_string_index(const std::string &key);

    bool path_exists(const std::list<std::string> &path_parts) const;
    std::set<std::string> get_children(const std::list<std::string> &path_parts) const;
    std::list<file_entry> get_files(const std::list<std::string> &path_parts) const;
    const file_entry &get_file(const std::list<std::string> &path_parts, const std::string &name) const;
    void delete_file(const std::list<std::string> &path_parts, const std::string &name);
  protected:
    size_t get_string_index(const std::string &key) const;
    path_component *walk_tree(const std::list<std::string> &path_parts) const;
    //void add_file(const file_entry &file, const std::list<pattern_component> &pattern_parts);
    //void scan_path(const std::string &path, const std::list<pattern_component> &pattern_parts);
    void dump(const file_entry &file, int indent) const;
    void dump(const path_component &store, int indent) const;

    static Entries *_instance;
    std::map<std::string, size_t> string_index;
    std::map<size_t, std::string> string_reverse_index;
    path_component *store;
    size_t num_files;
};

#endif//ENTRIES_H
