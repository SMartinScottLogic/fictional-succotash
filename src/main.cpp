// TODO Error handling

#include "main.h"

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <ctime>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <getopt.h>

#include <algorithm>
#include <string>
#include <vector>
#include <map>

#include "options.h"

#define BUFSIZE (1024*1024)

bool case_insensitive = true;
int perform_actions = 1;
int verbosity = 0;
size_t sha_split_point = 3;

char clean(char ch) {
  if(isalnum(ch)) return ch;
  return '_';
}

constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string hexstring(unsigned char *data, int len)
{
  std::string s(len * 2, ' ');
  for (int i = 0; i < len; ++i) {
    s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
    s[2 * i + 1] = hexmap[data[i] & 0x0F];
  }
  return s;
}

struct file {
  std::string key;
  std::string name;
  std::string path;
  off_t size;
  time_t a_time;
  time_t m_time;
  time_t c_time;

  file(const std::string &_name, const std::string &_path);
  const std::string fullname() const;
  const std::string get_sha1() const;
  bool operator<(const file& b) const;
};

  file::file(const std::string &_name, const std::string &_path)
: key(_name), name(_name), path(_path), size(0)
, a_time(0), m_time(0), c_time(0)
{
  struct stat sb;
  std::string fullname = path + "/" + name;
  if(stat( fullname.c_str(), &sb)==-1) {
    fprintf(stderr, "ERROR (%s): %s(%d).\n", fullname.c_str(), strerror(errno), errno);
    throw -1;
  }
  size = sb.st_size;
  a_time = sb.st_atime;
  m_time = sb.st_mtime;
  c_time = sb.st_ctime;

  if(case_insensitive) {
    std::transform(key.begin(), key.end(), key.begin(), tolower);
  }
  std::transform(key.begin(), key.end(), key.begin(), clean);
}

const std::string file::fullname() const {
  return path + "/" + name;
}

bool file::operator<(const file& b) const {
  if(size < b.size) return true;
  if(size > b.size) return false;

  if(key < b.key) return true;
  if(key == b.key) {
    return (path < b.path);
  }
  return false;
}

const std::string file::get_sha1() const {
  // current state of the hash
  SHA_CTX ctx;
  SHA1_Init(&ctx);

  unsigned char buf[BUFSIZE];
  FILE *fp = fopen(this->fullname().c_str(), "rb");
  if(!fp) return "";
  while(!feof(fp)) {
    int n = fread(buf, 1, BUFSIZE, fp);
    if(n>0) {
      // Hash data as it comes in
      SHA1_Update(&ctx, buf, n);
    }
  }
  fclose(fp);
  // finalize it
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1_Final(hash, &ctx);

  std::string ret = hexstring(hash, SHA_DIGEST_LENGTH);
  return ret.substr(0, sha_split_point) + "/" + ret.substr(sha_split_point);
}

std::vector<file> files;

void dots() {
  if( (files.size() % 10000)==0) {
    fprintf(stderr, "\n%zu ", files.size() );
  }
  if( (files.size() % 100) == 0) {
    fprintf(stderr, ".");
  }
}

void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters, bool permit_empty=true)
{
  if(permit_empty)
  {
    std::string::size_type start = 0;
    std::string::size_type end = str.find_first_of(delimiters, start);

    while(std::string::npos != start)
    {
      // Found a token, add it to the vector.
      //std::string t_str = str.substr(start, end-start);
      //fprintf(stderr, "'%s'\n", t_str.c_str() );
      tokens.push_back(str.substr(start, end-start));
      // Skip found delimiter
      start = (end==std::string::npos?end:end+1);
      // Find next delimiter
      end = str.find_first_of(delimiters, start);
    }
  }
  else
  {
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
  }
}

void fetchfiles(bool recursive, const std::string &dir) {
  DIR *dirp = opendir( dir.c_str() );
  struct dirent *dp = NULL;
  while ((dp = readdir(dirp)) != NULL) {
    if( (dp->d_type & DT_REG)==DT_REG) {
      dots();
      try {
        files.push_back( file(dp->d_name, dir) );
      } catch(...) {
        // Skip anomalous file
      }
    }
    if( recursive && (dp->d_type & DT_DIR)==DT_DIR && dp->d_name[0]!='.' ) {
      fetchfiles(recursive, dir + "/" + dp->d_name);
    }
  }
  closedir(dirp);
}

#ifndef HAVE_MAGIC_H
#define MAGIC_MIME_TYPE 1
int magic_open(int) {
  return 0;
}
int magic_load(int, void*) {
  return 0;
}
const char *magic_file(int, const char *) {
  return "none";
}
const char *magic_error(int) {
  return "magic library missing.";
}
void magic_close(int) {
}
#endif

std::string box_structure = "%m/%z";
std::map<std::string, off_t> count_keys;
off_t threshold = 1024;

char clean_magic_char(char ch) {
  switch(ch) {
    case '/':
    case '\\':
      return '_';
    default:
      return ch;
  }
}

const std::string clean_magic(magic_t cookie, const char *magic) {
  if(magic==NULL) {
    fprintf(stderr, "%s(%d): %s(%d)\n", __FILE__, __LINE__, magic_error(cookie), magic_errno(cookie));
    throw -1;
  }
  std::string clean = magic;
  size_t pos = clean.find_first_of(';');
  if(pos != std::string::npos) {
    clean = clean.substr(0, pos);
  }
  std::transform(clean.begin(), clean.end(), clean.begin(), clean_magic_char);

  return clean;
}

std::string get_human_size(off_t _size)
{
  double size = _size;
  uint8_t size_code=0;

  while(size > 1024) {
    size_code ++;
    size /= 1024;
  }

  char name = 'b';
  switch(size_code) {
    case 0:
      name = 'b';
      break;
    case 1:
      name = 'K';
      break;
    case 2:
      name = 'M';
      break;
    case 3:
      name = 'G';
      break;
    case 4:
      name = 'T';
      break;
    default:
      size = _size;
      name = 'b';
      break;
  }

  char s[1024];
  snprintf(s, 1024, "%0.1lf%c", size, name);
  return s;
}

void rename(const file &file, const std::string &dest) {
  std::string source = file.fullname();
  std::string target = dest + "/" + file.name;
  if(source == target) return;
  printf( "%s -> %s\n", source.c_str(), target.c_str() );
  if( rename(source.c_str(), target.c_str() )==-1) {
    fprintf(stderr, "rename('%s', '%s'): %d: %s.\n", source.c_str(), target.c_str(), errno, strerror(errno));
    exit(-1);
  }
}

std::string get_date(time_t date) {
  std::tm *ptm = std::localtime(&date);
  char buffer[32];
  std::strftime(buffer, 32, "%Y%m%d", ptm);
  return buffer;
}

void boxfiles() {
  auto magic = magic_open(MAGIC_MIME_TYPE);
  if(magic_load(magic, NULL)==-1) {
    fprintf(stderr, "======== MAGIC ERROR = %s\n", magic_error(magic) );
  }
  std::set<std::string> known_paths;
  for(auto it=files.begin(); it!=files.end(); ++it) {
    try {
      auto sha = it->get_sha1();
    auto size = get_human_size(it->size);
    auto date = get_date(it->m_time);

    std::string magic_id = clean_magic(magic, magic_file( magic, it->fullname().c_str() ));

    std::string key;
    bool in_param = false;
    for(auto it=box_structure.begin(); it!=box_structure.end(); ++it) {
      switch(*it) {
	case '%':
	  if(in_param) {
	    in_param = false;
	    key += '%';
	  } else {
	    in_param = true;
	  }
	  break;

	case 'd':
	  if(in_param) {
	    in_param = false;
	    key += date;
	  } else {
	    key += (*it);
	  }
	  break;
	case 'm':
	  if(in_param) {
	    in_param = false;
	    key += magic_id;
	  } else {
	    key += (*it);
	  }
	  break;
	case 'z':
	  if(in_param) {
	    in_param = false;
	    key += size;
	  } else {
	    key += (*it);
	  }
	  break;
  case '1':
    if(in_param) {
      in_param = false;
      key += sha;
    } else {
      key += (*it);
    }
    break;

	default:
	  key += (*it);
	  break;
      }
    }

    off_t count = count_keys[key];
    count_keys[key]++;
    count /= threshold;
    count ++;

    char group[1024];
    snprintf(group, 1024, "/GROUP_%ju", (uintmax_t)count);
    key += group;
    if(verbosity > 1) {
      printf( "%s %s %s -> %s -- '%s'\n", it->fullname().c_str(), size.c_str(), magic_id.c_str(), key.c_str(), sha.c_str() );
    }
    if(perform_actions==0) continue;

    if( verbosity > 0 ) {
      printf( "Making directory tree..." );
    }
    std::vector<std::string> path_components;
    tokenize(key, path_components, "/");

    // TODO Record known paths - to stop multiply testing/creating
    std::string cur_path = ".";
    for(auto it=path_components.begin(); it!=path_components.end(); ++it) {
      if( verbosity > 1 ) {
	printf("(%s/%s)", cur_path.c_str(), it->c_str() );
      }
      cur_path.push_back('/');
      cur_path.append(*it);
      if(known_paths.find(cur_path)==known_paths.end()) {
	if( mkdir(cur_path.c_str(), 0777)==-1) {
	  if(errno!=EEXIST) {
	    fprintf(stderr, "ERROR(mkdir '%s'): %d: %s.\n", cur_path.c_str(), errno, strerror(errno));
	    exit(-1);
	  }
	}
	known_paths.insert(cur_path);
      }
    }
    if( verbosity > 0 ) {
      printf( "\nMoving file..." );
    }
    if( verbosity > 1 ) {
      printf( "('%s' -> '%s/%s')", it->fullname().c_str(), cur_path.c_str(), it->name.c_str() );
    }
    rename( *it, cur_path );
    if( verbosity > 0 ) {
      printf( "\ndone.\n" );
    }
  } catch(...) {}
  }
  magic_close(magic);
}

void processOption(const std::string &command, const std::string &arg) {
  if(command == "s" || command == "structure") {
    box_structure = arg;
    return;
  }
  if(command == "t" || command == "threshold") {
    sscanf(arg.c_str(), "%zu", &threshold);
    return;
  }
  if(command == "1" || command == "hash-split") {
    sscanf(arg.c_str(), "%zu", &sha_split_point);
    return;
  }
  if(command == "v") {
    verbosity ++;
    return;
  }
}

int main(int argc, char *argv[]) {
  ogs::Options options;
  options.set_name("tidy");
  options.add_option("structure", 's', nullptr, 0, processOption, "PATTERN", "Desired organisational structure. (Default: '%s')", box_structure.c_str() );
  options.add_option("threshold", 't', nullptr, 0, processOption, "THRESHOLD", "Maximum files per group in organisation structure. (Default: %zu)", threshold);
  options.add_option("hash-split", '1', nullptr, 0, processOption, "THRESHOLD", "Position in SHA1 hash to split path (Default: %zu)", sha_split_point);
  options.add_option("dry-run", 'n', &perform_actions, 0, processOption, "", "Do not modify/move files, only show what would be done.");
  options.add_option("", 'v', nullptr, 0, processOption, "", "Increase verbosity.");
  int optind = options.getopt(argc, argv);
  while(optind < argc) {
    fetchfiles(true, argv[optind]);
    optind++;
  }
  printf( "Found %zu files.\n", files.size() );
  printf( "Sorting..." );
  std::sort(files.begin(), files.end());
  printf( "done.\n");
  boxfiles();
}
