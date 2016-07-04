/** \file
* \brief Definition of command-line parameters parser
*/
#ifndef OPTIONS_H
#define OPTIONS_H

#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include <string>

#define DEFAULT_PRIORITY 1.0

namespace ogs
{
  /** \brief Parser for command-line parameters*/
  class Options
  {
    public:
      Options();
      ~Options();
      /** \brief Process command-line
      * \param argc Number of command line parameters
      * \param argv Command line parameters
      * \return Index in argv of the first argv-element that is not an option.
      */
      int getopt(int argc, char *argv[]) const;
      
      /** \brief Add a command-line parameter handler
      *
      * \em flag
      * \em val
      * These parameters control how to report or act on the option when it occurs. 
      * If flag is a null pointer, then the val is a value which identifies this option. Often these values are chosen to uniquely identify particular long options.
      * If flag is not a null pointer, it should be the address of an int variable which is the flag for this option. The value in val is the value to store in the flag to indicate that the option was seen.
      *
      * \param name Long name for parameter to handle
      * \param opt Parameter to handle
      * \param flag
      * \param val
      * \param arg Name for argument      
      * \param fn Function to handle this parameter
      * \param description Description of parameter's purpose to provide in usage description
      */
      void add_option(const std::string &name, char opt, int *flag, int val, void (*fn)(const std::string &, const std::string &), const std::string &arg, const char *description, ...);
      /** \brief Add a command-line parameter handler
      *
      * \em flag
      * \em val
      * These parameters control how to report or act on the option when it occurs.
      * If flag is a null pointer, then the val is a value which identifies this option. Often these values are chosen to uniquely identify particular long options.
      * If flag is not a null pointer, it should be the address of an int variable which is the flag for this option. The value in val is the value to store in the flag to indicate that the option was seen.
      *
      * \param name Long name for parameter to handle
      * \param opt Parameter to handle
      * \param flag
      * \param val
      * \param arg Name for argument
      * \param fn Function to handle this parameter
      * \param description Description of parameter's purpose to provide in usage description
      */
      void add_option(const std::string &section, const std::string &name, char opt, int *flag, int val, void (*fn)(const std::string &, const std::string &), const std::string &arg, const char *description, ...);
      /** \brief */
      void add_section(const std::string &section, double priority=DEFAULT_PRIORITY, const std::string description="");
      void add_section(const std::string &section, const std::string description, double priority=1.0 );
      /** \brief Write usage instructions
      * \param stream C-standard FILE to which to write usage instructions
      */
      void usage(FILE *stream) const;
      void usage(FILE *stream, bool manpage) const;
      /** \brief Set program name
      * \param name Name to use for current program
      */
      void set_name(const std::string &name)
      {
	_name = name;
      }
      /** \brief Set static parameter text
      * \param s Display text for static parameters
      */
      void set_static(const std::string &s)
      {
	_static = s;
      }
      /** \brief Set description of the purpose of the current program
      * \param description Description of the purpose of the current program
      */	
      void set_description(const std::string &description)
      {
	_description = description;
      }
      /** \brief Set short description of the purpose of the current program
      * \param description Short description of the purpose of the current program
      */	
      void set_short_description(const std::string &description)
      {
	_short_description = description;
      }
      /** \brief Add a usage example
      * \param command Command line for example
      * \param description Description of expected program behaviour
      */
      void add_example(const std::string &command, const std::string &description);
    protected:
      /** \brief One parameter */
      struct Option
      {
	Option(const std::string &name, char opt, const std::string &arg, const std::string &description, int *flag, int val, void (*fn)(const std::string &, const std::string &))
	  : m_name(name), m_arg(arg), m_description(description), m_opt(opt), m_flag(flag), m_val(val), m_fn(fn)
	{
	}
	
	Option(const Option &other)
	  : m_name(other.m_name), m_arg(other.m_arg), m_description(other.m_description), m_opt(other.m_opt), m_flag(other.m_flag), m_val(other.m_val), m_fn(other.m_fn)
	{
	}
	
	Option &operator=(const Option &other)
	{
	  m_name = other.m_name;
	  m_arg = other.m_arg;
	  m_description = other.m_description;
	  m_opt = other.m_opt;
	  m_flag = other.m_flag;
	  m_val = other.m_val;
	  m_fn = other.m_fn;
	  return (*this);
	}
	~Option() {}
	
	std::string m_name;
	std::string m_arg;
	std::string m_description;
	char m_opt;
	int *m_flag;
	int m_val;
	void (*m_fn)(const std::string &, const std::string &);
      };
      
      struct Section {
	Section()
	: name(), description(), priority(DEFAULT_PRIORITY)
	{}
	
	Section(std::string _name, double _priority, std::string _description)
	: name(_name), description(_description), priority(_priority)
	{
	}
	~Section() {}
	
	bool operator<(const Section &other) const {
	  if(this->priority > other.priority) {
	    return true;
	  } else if(this->priority == other.priority && name < other.name) {
	    return true;
	  }
	  return false;
	}
	std::string name;
	std::string description;
	double priority;
      };
      
      std::map<std::string, std::vector<Option>> _parameters; /*!< \brief Store for options */
      std::map<std::string, Section> _sections; /*!< \brief Store for sections */
      std::string _name; /*!< \brief Program name */
      std::string _static; /*!< \brief Static parameters */
      std::string _description; /*!< \brief Program description */
      std::string _short_description; /*!< \brief Program short description */
      std::vector<std::pair<std::string, std::string>> _examples; /*!< \brief Store for usage examples */
  }; //Options
} // ogs
#endif //OPTIONS_H
