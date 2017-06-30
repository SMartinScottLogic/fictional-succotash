#ifndef TAGFS_LOG_H
#define TAGFS_LOG_H

#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/record_ordering.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

namespace tagfs
{
class log {
  public:
     log();
     ~log();
     void printf(boost::log::trivial::severity_level, const char*, ...) const;
     void printf(int, const char*, ...) const;
};

extern log g_log;
};

#define LOG(level, msg, ...) do {tagfs::g_log.printf(level, "%s(%d): " msg, __FILE__, __LINE__, ##__VA_ARGS__);} while(false)
//Narrow-char thread-safe logger.
typedef src::severity_logger_mt<logging::trivial::severity_level> logger_t;

//declares a global logger with a custom initialization
BOOST_LOG_GLOBAL_LOGGER(my_logger, logger_t)

#endif//TAGFS_LOG_H
