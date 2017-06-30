#ifndef METAFS_CONFIG_HH
#define METAFS_CONFIG_HH

struct config {
  config(): filename_pattern(), meta_pattern(), paths() 
  {}

  std::string filename_pattern;
  std::string meta_pattern;
  std::list<std::string> paths;
};

class Config {
  public:
    static Config *Instance();
    void load_file(const std::string &filename);
    const config &get_config() const;
  private:
    Config();
    Config(const Config&)=delete;
    Config &operator=(const Config&)=delete;
    ~Config();

    config m_config;
    static Config *_instance;
};

#endif//METAFS_CONFIG_HH
