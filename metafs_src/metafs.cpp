#include <cassert>
#include <string>
#include <algorithm>

#include "metafs.h"
#include "plugin.h"
#include "entries.h"
#include "log.h"

MetaFS* MetaFS::_instance = nullptr;
MetaFS* MetaFS::Instance() {
  if(_instance == nullptr) {
    _instance = new MetaFS();
  }
  return _instance;
}

MetaFS::~MetaFS() {
  if(m_entries!=nullptr) {
    delete m_entries;
  }
  _instance = nullptr;
}

MetaFS::MetaFS()
: StubFS()
, m_config_pathname(), m_entries(nullptr)
{
  plugin::initialize();
  m_entries = new Entries();
}
  
void MetaFS::set_config_file(const char *config_pathname) {
  m_config_pathname = config_pathname;
  LOG(0, "%s: '%s' : %s", __PRETTY_FUNCTION__, "TODO: load", m_config_pathname.c_str());
  Config::Instance()->load_file(m_config_pathname);
  m_entries->config_update();
}

std::list<std::string> MetaFS::split_path(const std::string &path) const {
  std::list<std::string> parts;
  assert(path[0]=='/');

  size_t start = 0;
  size_t len = 0;

  do {
    start = path.find_first_not_of("/", start+len);
    if(start==path.npos) {
      break;
    }
    size_t end = path.find_first_of("/", start);
    if(end==path.npos) {
      parts.push_back( path.substr(start) );
      break;
    }
    len = (end-start);
    parts.push_back( path.substr(start, len) );
  }while(true);

  return parts;
}

const struct stat *MetaFS::generate_file_stat(const file_entry &file, struct stat *statbuf) const {
  LOG(0, "%s: '%s' : %p", __PRETTY_FUNCTION__, file.full_path.c_str(), statbuf);
  if(statbuf == NULL) {
    return statbuf;
  }
  auto r = lstat(file.full_path.c_str(), statbuf);
  if(r!=0) {
    return NULL;
  }
  //statbuf->st_dev = 0;     /* ID of device containing file */
  statbuf->st_ino = 0;     /* inode number */
  //statbuf->st_mode = 0644 | __S_IFREG;    /* protection */
  statbuf->st_nlink = 1;   /* number of hard links */
  //statbuf->st_uid=context->uid;     /* user ID of owner */
  //statbuf->st_gid=context->gid;     /* group ID of owner */
  //statbuf->st_rdev=0;    /* device ID (if special file) */
  //statbuf->st_size=0;    /* total size, in bytes */
  //statbuf->st_blksize=4096; /* blocksize for file system I/O */
  //statbuf->st_blocks=1;  /* number of 512B blocks allocated */
  //statbuf->st_atime=0;   /* time of last access */
  //statbuf->st_mtime=0;   /* time of last modification */
  //statbuf->st_ctime=0;   /* time of last status change */
  //dump_stat(path, statbuf);
  return statbuf;
}

int MetaFS::Statfs(const char *path, struct statvfs *statInfo) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"statfs(path=%s)", path);
  memset(statInfo, 0, sizeof(struct statvfs));
  return 0;
  //RETURN_ERRNO(statvfs(m_root.c_str(), statInfo));
}

int MetaFS::Mkdir(const char *path, mode_t /*mode*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  return -EPERM;
}

int MetaFS::Opendir(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );

  auto parts = split_path(path);
  if(m_entries->path_exists(parts)) {
    return 0;
  }
  return -ENOTDIR;
}

int MetaFS::Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s' %zu", __PRETTY_FUNCTION__, path, offset );
  auto parts = split_path(path);
  if(!m_entries->path_exists(parts)) {
    return 0;
  }
  if(filler(buf, ".", NULL, 0) != 0) {
    return -ENOMEM;
  }
  if(filler(buf, "..", NULL, 0) != 0) {
    return -ENOMEM;
  }
  if( offset == 0 && strcmp(path, "/")==0 ) {
    if(filler(buf, ".config", NULL, 0) != 0) {
      return -ENOMEM;
    }
  }
   // Paths
  auto paths = m_entries->get_children(parts);
  for(auto path : paths) {
    LOG(0,"%s: child path: '%s'", __PRETTY_FUNCTION__, path.c_str() );
    if(filler(buf, path.c_str(), NULL, 0) != 0) {
      return -ENOMEM;
    }
  }
  auto files = m_entries->get_files(parts);
  for(auto file : files) {
    LOG(0,"%s: '%s' -> '%s'", __PRETTY_FUNCTION__, file.name.c_str(), file.full_path.c_str());
    struct stat sb;
    if(filler(buf, file.name.c_str(), generate_file_stat(file, &sb), 0) != 0) {
      return -ENOMEM;
    }
  }
  return 0;
}

int MetaFS::Releasedir(const char *path, struct fuse_file_info *fileInfo) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  if( NULL != (DIR*)(fileInfo->fh) ) {
    closedir( (DIR*)(fileInfo->fh) );
  }
  return 0;
}

int MetaFS::Getattr(const char *path, struct stat *statbuf) {
  LOG(0, "%s: '%s'", __PRETTY_FUNCTION__, path );
  auto *context = fuse_get_context();
  if( strcmp(path, "/.config")==0 ) {
    memset(statbuf, 0, sizeof(struct stat));
    //auto r = lstat(m_root.c_str(), statbuf);
    //if(r==0) {
      //statbuf->st_dev = 0;     /* ID of device containing file */
      statbuf->st_ino = 0;     /* inode number */
      statbuf->st_mode = 0644 | __S_IFREG;    /* protection */
      statbuf->st_nlink = 1;   /* number of hard links */
      statbuf->st_uid=context->uid;     /* user ID of owner */
      statbuf->st_gid=context->gid;     /* group ID of owner */
      //statbuf->st_rdev=0;    /* device ID (if special file) */
      statbuf->st_size=9;    /* total size, in bytes */
      //statbuf->st_blksize=4096; /* blocksize for file system I/O */
      //statbuf->st_blocks=1;  /* number of 512B blocks allocated */
      //statbuf->st_atime=0;   /* time of last access */
      //statbuf->st_mtime=0;   /* time of last modification */
      //statbuf->st_ctime=0;   /* time of last status change */
      //dump_stat(path, statbuf);
      return 0;
    //}
    //r = -errno;
    //LOG(0,"lstat('%s', statbuf) => %d", m_root.c_str(), errno);
    //return r;
  }
  auto parts = split_path(path);
  if(m_entries->path_exists(parts)) {
    memset(statbuf, 0, sizeof(struct stat));
    //auto r = lstat(m_root.c_str(), statbuf);
    //if(r==0) {
      //statbuf->st_dev = 0;     /* ID of device containing file */
      statbuf->st_ino = 0;     /* inode number */
      statbuf->st_mode = 0755 | __S_IFDIR;    /* protection */
      statbuf->st_nlink = 1;   /* number of hard links */
      statbuf->st_uid=context->uid;     /* user ID of owner */
      statbuf->st_gid=context->gid;     /* group ID of owner */
      //statbuf->st_rdev=0;    /* device ID (if special file) */
      statbuf->st_size=0;    /* total size, in bytes */
      //statbuf->st_blksize=4096; /* blocksize for file system I/O */
      //statbuf->st_blocks=1;  /* number of 512B blocks allocated */
      //statbuf->st_atime=0;   /* time of last access */
      //statbuf->st_mtime=0;   /* time of last modification */
      //statbuf->st_ctime=0;   /* time of last status change */
      //dump_stat(path, statbuf);
      return 0;
    //}
    //r = -errno;
    //LOG(0,"lstat('%s', statbuf) => %d", m_root.c_str(), errno);
    //return r;
  }
  auto name = parts.back();
  parts.pop_back();
  auto file = m_entries->get_file(parts, name);
  if(file.deleted) {
    return -ENOENT;
  }
  LOG(0,"%s: '%s' -> '%s' ~ '%s'", __PRETTY_FUNCTION__, path, file.name.c_str(), file.full_path.c_str());
  if( generate_file_stat(file, statbuf) != NULL) {
    return 0;
  }
  LOG(0, "generate_file_stat('%s') FAILURE", path);
  return StubFS::Getattr(path, statbuf);
}

int MetaFS::Mknod(const char *path, mode_t mode, dev_t dev) {
  LOG(0,"%s: mknod('%s', %o)", __PRETTY_FUNCTION__, path, mode );

  return StubFS::Mknod(path, mode, dev);
}

int MetaFS::Open(const char *path, struct fuse_file_info *fileInfo) {
  LOG(0,"%s: '%s' %0x", __PRETTY_FUNCTION__, path, fileInfo->flags );
  if( strcmp(path, "/.config")==0 ) {
    fileInfo->direct_io = 1;
    fileInfo->fh = 0;
    return 0;
  }
  auto parts = split_path(path);
  auto name = parts.back();
  parts.pop_back();
  auto file = m_entries->get_file(parts, name);
  if(!file.deleted)
  {
    LOG(0,"%s: '%s' -> '%s' ~ '%s'", __PRETTY_FUNCTION__, path, file.name.c_str(), file.full_path.c_str());
    int fd = open(file.full_path.c_str(), fileInfo->flags);
    if (fd == -1) {
      return -errno;
    }
    fileInfo->direct_io = 1;
    fileInfo->fh = (unsigned long)fd;
    LOG(0,"%s: '%s' -> %d", __PRETTY_FUNCTION__, path, fd);
    return 0;
  }
  return StubFS::Open(path, fileInfo);
}

int MetaFS::Flush(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  //noop because we don't maintain our own buffers
  LOG(0,"flush(path=%s)", path);
  return 0;
}

int MetaFS::Release(const char *path, struct fuse_file_info *fileInfo) {
  LOG(0,"%s: '%s' -> %d", __PRETTY_FUNCTION__, path, fileInfo->fh );
  if( fileInfo->fh == 0) {
    return 0;
  }
  int res = close(fileInfo->fh);
  if (res == -1) {
    return -errno;
  }
  return 0;
}


int MetaFS::Read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
  LOG(boost::log::trivial::severity_level::debug, "%s: '%s' %zu %zu %0x", __PRETTY_FUNCTION__, path, size, offset, fileInfo->flags );
  if( strcmp(path, "/.config")==0 ) {
    std::string data = "{\"sort\":{\"key\":\"size\",\"order\":\"desc\"},\"template\":\"{magic}\"}";
    data = data.substr(offset, size);
    memcpy(buf, data.data(), data.size());
    LOG(0,"%s: '%s' : '%s'", __PRETTY_FUNCTION__, path, data.c_str());
    return std::min(size, data.size());
  }
  if( fileInfo->fh != 0 ) {
    int res = pread(fileInfo->fh, buf, size, offset);
    if (res == -1) {
      return -errno;
    }
    return res;
  }
  return StubFS::Read(path, buf, size, offset, fileInfo);
}

int MetaFS::Unlink(const char *path) {
  LOG(boost::log::trivial::severity_level::debug, "%s: '%s'", __PRETTY_FUNCTION__, path );
  auto parts = split_path(path);
  auto name = parts.back();
  parts.pop_back();
  auto file = m_entries->get_file(parts, name);
  if(!file.deleted)
  {
    LOG(0,"%s: '%s' -> '%s' ~ '%s'", __PRETTY_FUNCTION__, path, file.name.c_str(), file.full_path.c_str());
    int res = unlink(file.full_path.c_str());
    if (res == -1) {
      return -errno;
    }
    LOG(0, "%s:  Mark deleted", __PRETTY_FUNCTION__);
    m_entries->delete_file(parts, name);
    return 0;
  }
  return StubFS::Unlink(path);
}

void *MetaFS::Init(struct fuse_conn_info * /*conn*/) {
  return _instance;
}

