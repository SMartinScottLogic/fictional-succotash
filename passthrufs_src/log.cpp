#include <cstdio>
#include <cstdarg>
#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
#include <functional>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/barrier.hpp>
#include "log.h"

//Defines a global logger initialization routine
BOOST_LOG_GLOBAL_LOGGER_INIT(my_logger, logger_t)
{
    logger_t lg;

    logging::add_common_attributes();

    logging::add_file_log(
            boost::log::keywords::file_name = "passthru.log",
            boost::log::keywords::format = (
                    expr::stream << expr::format_date_time<     boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
                    << " [" << expr::attr<     boost::log::trivial::severity_level >("Severity") << "]: "
                    << expr::smessage
            )
    );

    logging::add_console_log(
            std::cout,
            boost::log::keywords::format = (
                    expr::stream << expr::format_date_time<     boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
                    << " [" << expr::attr<     boost::log::trivial::severity_level >("Severity") << "]: "
                    << expr::smessage
            )
    );

    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::info
    );

    return lg;
}

namespace passthrufs {
log g_log;

log::log()  {
}

log::~log() {
}

void log::printf(int, const char *format, ...) const {
  va_list vl;
  char *message = NULL;
  va_start(vl, format);
  //vsyslog(LOG_USER | LOG_INFO, format, vl);
  vasprintf(&message, format, vl);
  va_end(vl);

  BOOST_LOG_SEV(my_logger::get(), boost::log::trivial::severity_level::info) << message;
  free(message);
}

void log::printf(boost::log::trivial::severity_level level, const char *format, ...) const {
  va_list vl;
  char *message = NULL;
  va_start(vl, format);
  //vsyslog(LOG_USER | LOG_INFO, format, vl);
  vasprintf(&message, format, vl);
  va_end(vl);

  BOOST_LOG_SEV(my_logger::get(), level) << message;
  free(message);
}
};

