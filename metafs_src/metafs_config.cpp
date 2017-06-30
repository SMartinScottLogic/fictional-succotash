#include <boost/property_tree/json_parser.hpp>

#include "metafs.h"
#include "metafs_config.h"
#include "metafs_config.hh"
 
Config* Config::_instance = nullptr;
Config* Config::Instance() {
  if(_instance == nullptr) {
    _instance = new Config();
  }
  return _instance;
}

Config::Config()
: m_config()
{}

Config::~Config() {
  _instance = nullptr;
}

const config &Config::get_config() const {
  return m_config;
}

void Config::load_file(const std::string &config_filename) {
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(config_filename, pt);

  m_config.filename_pattern = pt.get<std::string>("filename_pattern");
  m_config.meta_pattern = pt.get<std::string>("meta_pattern");
  fprintf( stderr, "META_PATTERN : '%s'\n", pt.get<std::string>("meta_pattern").c_str() );
  fprintf( stderr, "FILENAME_PATTERN : '%s'\n", pt.get<std::string>("filename_pattern").c_str() );
  for(auto path : pt.get_child("paths")) {
    fprintf( stderr, "PATH : '%s'\n", path.second.data().c_str() );
    m_config.paths.push_back(path.second.data());
  }
}

