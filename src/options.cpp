/** \file
* \brief Implementation of command-line parameters parser
*/
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <getopt.h>
#include "options.h"

namespace ogs
{
  Options::Options(): _parameters(), _sections(), _name(), _static(), _description(), _short_description(), _examples() {}
  Options::~Options() {}
  int Options::getopt(int argc, char *argv[]) const
  {
    // Create short option string;
    std::string s_options = "Dh?";
    std::map<char, std::set<void (*)(const std::string &, const std::string &)> > functions;
    int num_long = 2; // Extras are for ('help', 'manpage') options
    for(auto section=_parameters.begin(); section!=_parameters.end(); ++section)
    {
      for(auto it=section->second.begin(); it!=section->second.end(); ++it)
      {
	if(it->m_name.length()>0) num_long++;
	
	if(it->m_opt!='\0')
	{
	  s_options.push_back(it->m_opt);
	  if(it->m_arg.length()>0)
	  {
	    s_options.push_back(':');
	  }
	  functions[it->m_opt].insert(it->m_fn);
	}
      }
    }
    
    #ifdef DEBUG
    fprintf(stderr, "GETOPT_SHORT: %s\n", s_options.c_str() );
    fprintf(stderr, "GETOPT_LONG : ");
    #endif
    
    // Create long option struct
    option *longopts = new option[num_long+1];
    do {
      int i = 0;
      longopts[i].name = "help";
      longopts[i].has_arg = no_argument;
      longopts[i].flag = NULL;
      longopts[i].val = 0;
      i++;
      longopts[i].name = "manpage";
      longopts[i].has_arg = no_argument;
      longopts[i].flag = NULL;
      longopts[i].val = 0;
      i++;
      for(auto section=_parameters.begin(); section!=_parameters.end(); ++section)
      {
	for(auto it=section->second.begin(); it!=section->second.end(); ++it)
	{
	  if(it->m_name.length()>0)
	  {
	    #ifdef DEBUG
	    fprintf(stderr, "%s ", it->m_name.c_str() );
	    #endif
	    longopts[i].name = it->m_name.c_str();
	    longopts[i].has_arg = ((it->m_arg.length()>0)?required_argument:no_argument);
	    longopts[i].flag = it->m_flag;
	    longopts[i].val = it->m_val;
	    i++;
	  }
	}
      }
      memset( &longopts[num_long], 0, sizeof(option) );
    }while(false);
    #ifdef DEBUG
    fprintf(stderr, "\n" );
    #endif
    
    int c, index;
    
    while((c=::getopt_long(argc, argv, s_options.c_str(), longopts, &index))!=-1)
    {
      if(c!=0)
      {
	std::string c_name;
	c_name.push_back(c);
	
	if(c=='D')
	{
	  printf( "%s\t%s\n", _name.c_str(), _description.c_str() );
	  exit(0);
	}
	
	auto it=functions.find(c);
	if(it!=functions.end())
	{
	  for(auto fn=it->second.begin(); fn!=it->second.end(); ++fn)
	  {
	    if((*fn)!=NULL) (*fn)( c_name, (optarg==NULL ? std::string() : std::string(optarg)) );
	  }
	  continue;
	}
	else
	{
	}
	if(c=='h' || c=='?')
	{
	  usage(stdout);
	  exit(0);
	}
      } 
      std::string name = longopts[index].name;
      if(name == "help")
      {
	usage(stdout);
	exit(0);
      }
      if(name == "manpage") {
	usage(stdout, true);
	exit(0);
      }
      for(auto section=_parameters.begin(); section!=_parameters.end(); ++section)
      {
	for(auto it=section->second.begin(); it!=section->second.end(); ++it)
	{
	  if(it->m_name == name)
	  {
	    if(it->m_fn!=NULL) 
	    {
	      (it->m_fn)( name, (optarg==NULL ? std::string() : std::string(optarg)) );
	    }
	  }
	}
      }
    }
    delete [] longopts;
    return optind;
  }
  
  void Options::add_option(const std::string &name, char opt, int *flag, int val, void (*fn)(const std::string &, const std::string &), const std::string &arg, const char *description, ...)
  {
    va_list ap;
    va_start(ap, description);
    
    char *s = NULL;
    int r = vasprintf(&s, description, ap);
    va_end(ap);
    
    Option o(name, opt, arg, s, flag, val, fn);
    if(r>0) free(s);
    _parameters[""].push_back(o);
    if(_sections.find("")==_sections.end()) {
      add_section("");
    }
  }
  
  void Options::add_option(const std::string &section, const std::string &name, char opt, int *flag, int val, void (*fn)(const std::string &, const std::string &), const std::string &arg, const char *description, ...)
  {
    va_list ap;
    va_start(ap, description);
    
    char *s = NULL;
    int r = vasprintf(&s, description, ap);
    va_end(ap);
    
    Option o(name, opt, arg, s, flag, val, fn);
    if(r>0) free(s);
    _parameters[section].push_back(o);
    
    if(_sections.find(section)==_sections.end()) {
      add_section(section);
    }
  }
  
  void Options::add_section(const std::string &section, double priority, const std::string description) 
  {
    _sections[section] = Section(section, priority, description);
  }
  
  void Options::add_section(const std::string &section, const std::string description, double priority ) 
  {
    _sections[section] = Section(section, priority, description);
  }
  
  void Options::add_example(const std::string &command, const std::string &description)
  {
    _examples.push_back(std::pair<std::string, std::string>(command, description));
  }
  
  void Options::usage(FILE *stream) const
  {
    usage(stream, false);
  }
  
  void Options::usage(FILE *stream, bool manpage) const
  {
    if(manpage) {
      fprintf( stream, ".TH \"%s\" 1 \"\" Linux \"OGS\"\n", _name.c_str() );
      fprintf( stream, ".SH NAME\n" );
      if(_short_description.length()>0) {
	fprintf( stream, "%s \\- %s\n", _name.c_str(), _short_description.c_str() );
      } else {
	fprintf( stream, "%s\n", _name.c_str() );
	fprintf( stderr, "%s: WARNING: no 'short description' defined.\n", _name.c_str() );
      }
      fprintf( stream, ".SH SYNOPSIS\n" );
      fprintf( stream, ".B %s %s [\\fIOPTION\\fR]...\n", _name.c_str(), _static.c_str() );
      if(_description.length()>0) {
	fprintf( stream, ".SH DESCRIPTION\n" );
	fprintf( stream, ".B %s\n", _name.c_str() );
	fprintf( stream, "%s\n", _description.c_str() );
      }
      if (_sections.size()>0)
      {
	std::multiset<Section> sections;
	for(auto it = _sections.begin(); it!=_sections.end(); ++it) {
	  sections.insert(it->second);
	}
	
	fprintf(stream, ".SH OPTIONS\n");
	for (auto section=sections.begin(); section!=sections.end(); ++section) 
	{
	  if(section->name.length()>0) 
	  {
	    fprintf( stream, ".SS %s\n", section->name.c_str() );
	  }
	  if(section->description.length()>0)
	  {
	    std::string description;
	    for(auto it=section->description.begin(); it!=section->description.end(); ++it) 
	    {
	      switch(*it)
	      {
		case '\n':
		  description += "\n.br\n";
		  break;
		default:
		  description += (*it);
		  break;
	      }
	    }	      
	    fprintf( stream, "%s\n", description.c_str() );
	  }
	  auto p = _parameters.find(section->name);
	  if(p!=_parameters.end()) 
	  {
	    for (auto it=p->second.begin(); it!=p->second.end(); ++it)
	    {
	      if(it->m_opt!='\0' && it->m_name.length()>0) {
		fprintf( stream, ".TP\n" );
		fprintf( stream, ".PD 0\n" );
		fprintf(stream, ".BI \\-%c ", it->m_opt);
		if (it->m_arg.length()>0)
		{
		  fprintf(stream, "\" %s\"", it->m_arg.c_str() );
		}
		fprintf(stream, "\n");
		fprintf( stream, ".TP\n" );
		fprintf( stream, ".PD\n" );
		fprintf( stream, ".BI \\-\\^\\-%s ", it->m_name.c_str() );
		if (it->m_arg.length()>0)
		{
		  fprintf(stream, "\" %s\"", it->m_arg.c_str() );
		}
		fprintf(stream, "\n");
	      }
	      else if(it->m_opt!='\0') {
		fprintf( stream, ".TP\n" );
		fprintf( stream, ".BI \\-%c ", it->m_opt);
		if (it->m_arg.length()>0)
		{
		  fprintf(stream, "\" %s\"", it->m_arg.c_str() );
		}
		fprintf(stream, "\n.br\n");
	      } else if(it->m_name.length()>0) {
		fprintf( stream, ".TP\n" );
		fprintf( stream, ".BI \\-\\^\\-%s ", it->m_name.c_str() );
		if (it->m_arg.length()>0)
		{
		  fprintf(stream, "\" %s\"", it->m_arg.c_str() );
		}
		fprintf(stream, "\n.br\n");
	      }
	      fprintf(stream, "%s\n", it->m_description.c_str() );
	    }
	  }
	}
	fprintf(stream, "\n");
      }
      if (_examples.size()>0) {
	fprintf(stream, ".SH EXAMPLE%s\n.nf\n", (_examples.size()>1)?"S":"");
	for (auto it=_examples.begin(); it!=_examples.end(); ++it) {
	  fprintf( stream, "%s:\n", it->second.c_str() );
	  fprintf( stream, ".ft B\n" );
	  fprintf( stream, "  %s %s\n", _name.c_str(), it->first.c_str() );
	  fprintf( stream, ".ft R\n" );
	  fprintf( stream, ".P\n" );
	}
	fprintf(stream, ".fi\n" );
      }
      return;
    }
    fprintf(stream, "SYNOPSIS\n");
    fprintf(stream, "\t%s [OPTIONS] %s\n\n", _name.c_str(), _static.c_str());
    if (_description.length()>0)
    {
      fprintf(stream, "DESCRIPTION\n");
      fprintf(stream, "\t%s\n\n", _description.c_str() );
    }
    if (_sections.size()>0)
    {
      std::multiset<Section> sections;
      for(auto it = _sections.begin(); it!=_sections.end(); ++it) {
	sections.insert(it->second);
      }
      
      fprintf(stream, "OPTIONS\n");
      for (auto section=sections.begin(); section!=sections.end(); ++section) 
      {
	if(section->name.length()>0) 
	{
	  fprintf( stream, "  %s\n", section->name.c_str() );
	}
	if(section->description.length()>0)
	{
	  fprintf( stream, "%s\n", section->description.c_str() );
	}
	auto p = _parameters.find(section->name);
	if(p!=_parameters.end()) 
	{
	  for (auto it=p->second.begin(); it!=p->second.end(); ++it)
	  {
	    if(it->m_opt!='\0')
	    {
	      fprintf(stream, "\t-%c ", it->m_opt);
	      if (it->m_arg.length()>0)
	      {
		fprintf(stream, "%s", it->m_arg.c_str() );
	      }
	      fprintf(stream, "\n");
	    }
	    if(it->m_name.length()>0)
	    {
	      fprintf(stream, "\t--%s ", it->m_name.c_str() );
	      if (it->m_arg.length()>0)
	      {
		fprintf(stream, "%s", it->m_arg.c_str() );
	      }
	      fprintf(stream, "\n");
	    }
	    fprintf(stream, "\t\t%s\n", it->m_description.c_str() );
	    fprintf(stream, "\n");
	  }
	}
      }
      fprintf(stream, "\n");
    }
    if (_examples.size()>0)
    {
      fprintf(stream, "EXAMPLE%s\n", (_examples.size()>1)?"S":"");
      for (auto it=_examples.begin(); it!=_examples.end(); ++it)
      {
	fprintf(stream, "\t%s %s\n%s\n\n", _name.c_str(), it->first.c_str(), it->second.c_str() );
      }
    }
  }
} //ogs
