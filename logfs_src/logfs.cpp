#include <cassert>
#include <string>
#include <algorithm>

#include "logfs.h"
#include "log.h"

LogFS* LogFS::_instance = NULL;

#define RETURN_ERRNO(x) do {if ((x) == 0) { return 0; } else { LOG("%s => %d\n", #x, errno); return (-errno);}}while(false)

inline std::string stringify(const std::set<std::string> &data, const std::string &separator=" ") {
  std::string str;
  for(auto it=data.begin(); it!=data.end(); ++it) {
    if(str.length()>0) str += separator;
    str += (*it);
  }
  return str;
}

void LogFS::dump_stat(const std::string &fullPath, struct stat *statbuf) const
{
  LOG( "%s: st_dev = %d\n", fullPath.c_str(), statbuf->st_dev );  /* ID of device containing file */
  LOG( "%s: st_ino = %d\n", fullPath.c_str(), statbuf->st_ino );  /* inode number */
  LOG( "%s: st_mode = %o\n", fullPath.c_str(), statbuf->st_mode );    /* protection */
  LOG( "%s: st_nlink = %d\n", fullPath.c_str(), statbuf->st_nlink );   /* number of hard links */
  LOG( "%s: st_uid = %d\n", fullPath.c_str(), statbuf->st_uid );     /* user ID of owner */
  LOG( "%s: st_gid = %d\n", fullPath.c_str(), statbuf->st_gid );     /* group ID of owner */
  LOG( "%s: st_rdev = %d\n", fullPath.c_str(), statbuf->st_rdev );    /* device ID (if special file) */
  LOG( "%s: st_size = %d\n", fullPath.c_str(), statbuf->st_size );    /* total size, in bytes */
  LOG( "%s: st_blksize = %d\n", fullPath.c_str(), statbuf->st_blksize ); /* blocksize for file system I/O */
  LOG( "%s: st_blocks = %d\n", fullPath.c_str(), statbuf->st_blocks );  /* number of 512B blocks allocated */
  LOG( "%s: st_atime = %d\n", fullPath.c_str(), statbuf->st_atime );   /* time of last access */
  LOG( "%s: st_mtime = %d\n", fullPath.c_str(), statbuf->st_mtime );   /* time of last modification */
  LOG( "%s: st_ctime = %d\n", fullPath.c_str(), statbuf->st_ctime );   /* time of last status change */
}

LogFS* LogFS::Instance() {
  if(_instance == NULL) {
    _instance = new LogFS();
  }
  return _instance;
}

LogFS::LogFS(): m_root() {
}

LogFS::~LogFS() {
  LOG("Unmounted 'tagfs' rooted from '%s'", m_root.c_str() );
  _instance = NULL;
}

void LogFS::setRootDir(const char *path) {
  do {
    char *_path = realpath(path, NULL);
    if(_path==NULL) {
      LOG("Error ('%s') setting FS root to: %s\n", strerror(errno), path);
    }
    m_root = _path;
    free(_path);
  } while(false);
  LOG("setting FS root to: %s\n", m_root.c_str());
}

int LogFS::Getattr(const char *path, struct stat *statbuf) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  auto *context = fuse_get_context();
  auto r = lstat(m_root.c_str(), statbuf);
  if(r!=0) {
    r = -errno;
    LOG("lstat('%s', statbuf) => %d\n", m_root.c_str(), errno);
    return r;
  } else {
    if( strcmp(path, "/")==0 || strcmp(path, "/test")==0 ) {
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
      dump_stat(path, statbuf);
      return 0;
    }
    if(strcmp(path, "/delete me.txt")==0 || strcmp(path, "/rename me.txt")==0 || strcmp(path, "/test/move me.txt")==0) {
      //statbuf->st_dev = 0;     /* ID of device containing file */
      statbuf->st_ino = 0;     /* inode number */
      statbuf->st_mode = 0644 | __S_IFREG;    /* protection */
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
      dump_stat(path, statbuf);
      return 0;
    }
  }
  return -ENOENT;
}

int LogFS::Readlink(const char *path, char *link, size_t size) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("readlink(path=%s, link=%s, size=%d)\n", path, link, (int)size);
  return -ENOENT;
}

/** \brief Handles creating FIFOs, regular files, etc...
*/
int LogFS::Mknod(const char *path, mode_t mode, dev_t /*dev*/) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("mknod(path=%s, mode=%o)\n", path, mode);
  return -EPERM;
}

/** \todo Decide what to do on \em original filesystem */
int LogFS::Mkdir(const char *path, mode_t mode) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("mkdir(path=%s, mode=%o)\n", path, (int)mode);
  return -EPERM;
}

int LogFS::Unlink(const char *path) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("unlink(path=%s)", path);
  return -ENOENT;
}

int LogFS::Rmdir(const char *path) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("rmdir(path=%s\n)", path);
  return -ENOENT;
}

int LogFS::Symlink(const char *path, const char *link) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("symlink(path=%s, link=%s)\n", path, link);
  return -ENOENT;
}

int LogFS::Rename(const char *src, const char *tgt) {
  LOG("%s: '%s' '%s'\n", __PRETTY_FUNCTION__, src, tgt );
  return -ENOENT;
}

int LogFS::Link(const char *path, const char *newpath) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("link(path=%s, newPath=%s)\n", path, newpath);
  return -ENOENT;
}

int LogFS::Chmod(const char *path, mode_t mode) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("chmod(path=%s, mode=%o)\n", path, mode);
  return -ENOENT;
}

int LogFS::Chown(const char *path, uid_t uid, gid_t gid) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("chown(path=%s, uid=%d, gid=%d)\n", path, (int)uid, (int)gid);
  return -ENOENT;
}

int LogFS::Truncate(const char *path, off_t newSize) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("truncate(path=%s, newSize=%d\n", path, (int)newSize);
  return -ENOENT;
}

int LogFS::Utime(const char *path, struct utimbuf * /*ubuf*/) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int LogFS::Open(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int LogFS::Read(const char *path, char * /*buf*/, size_t /*size*/, off_t /*offset*/, struct fuse_file_info * /*fileInfo*/) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int LogFS::Write(const char *path, const char * /*buf*/, size_t /*size*/, off_t /*offset*/, struct fuse_file_info * /*fileInfo*/) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int LogFS::Statfs(const char *path, struct statvfs *statInfo) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("statfs(path=%s)\n", path);
  RETURN_ERRNO(statvfs(m_root.c_str(), statInfo));
  //return -ENOENT;
}

int LogFS::Flush(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  //noop because we don't maintain our own buffers
  //LOG("flush(path=%s)\n", path);
  return 0;
}

int LogFS::Release(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int LogFS::Fsync(const char *path, int /*datasync*/, struct fuse_file_info * /*fi*/) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int LogFS::Setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("setxattr(path=%s, name=%s, value=%s, size=%d, flags=%d\n", path, name, value, (int)size, flags);
  return -ENOENT;
}

int LogFS::Getxattr(const char *path, const char *name, char * /*value*/, size_t size) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("getxattr(path=%s, name=%s, size=%d\n", path, name, (int)size);
  return -ENOENT;
}

int LogFS::Listxattr(const char *path, char * /*list*/, size_t size) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("listxattr(path=%s, size=%d)\n", path, (int)size);
  return -ENOENT;
}

int LogFS::Removexattr(const char *path, const char *name) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("removexattry(path=%s, name=%s)\n", path, name);
  return -ENOENT;
}

int LogFS::Opendir(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  if(strcmp(path, "/")==0 || strcmp(path, "/test")==0) {
    return 0;
  }
  return -ENOENT;
}

int LogFS::Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info * /*fileInfo*/) {
  LOG("%s: '%s' %zu\n", __PRETTY_FUNCTION__, path, offset );

  if(offset == 0 && strcmp(path, "/")==0) {
    if(filler(buf, "rename me.txt", NULL, 0) != 0) {
      return -ENOMEM;
    }
    if(filler(buf, "delete me.txt", NULL, 0) != 0) {
      return -ENOMEM;
    }
    return 0;
  }
  if(offset == 0 && strcmp(path, "/test")==0) {
    if(filler(buf, "move me.txt", NULL, 0) != 0) {
      return -ENOMEM;
    }
    return 0;
  }
  return -ENOENT;
}

int LogFS::Releasedir(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return 0;
}

int LogFS::Fsyncdir(const char *path, int /*datasync*/, struct fuse_file_info * /*fileInfo*/) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return 0;
}

void *LogFS::Init(struct fuse_conn_info * /*conn*/) {
  LOG("%s\n", __PRETTY_FUNCTION__ );
  LOG("init\n");
  return _instance;
}

int LogFS::Truncate(const char *path, off_t offset, struct fuse_file_info * /*fileInfo*/) {
  LOG("%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG("truncate(path=%s, offset=%d)\n", path, (int)offset);
  return -ENOENT;
}

