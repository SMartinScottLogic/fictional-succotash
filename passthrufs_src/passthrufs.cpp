#include <cassert>
#include <string>
#include <algorithm>

#include "passthrufs.h"
#include "log.h"

PassthruFS* PassthruFS::_instance = NULL;

#define RETURN_ERRNO(x) do {if ((x) == 0) { return 0; } else { LOG(0,"%s => %d\n", #x, errno); return (-errno);}}while(false)

inline std::string stringify(const std::set<std::string> &data, const std::string &separator=" ") {
  std::string str;
  for(auto it=data.begin(); it!=data.end(); ++it) {
    if(str.length()>0) str += separator;
    str += (*it);
  }
  return str;
}

void PassthruFS::dump_stat(const std::string &fullPath, struct stat *statbuf) const
{
  LOG(0, "%s: st_dev = %d\n", fullPath.c_str(), statbuf->st_dev );  /* ID of device containing file */
  LOG(0, "%s: st_ino = %d\n", fullPath.c_str(), statbuf->st_ino );  /* inode number */
  LOG(0, "%s: st_mode = %o\n", fullPath.c_str(), statbuf->st_mode );    /* protection */
  LOG(0, "%s: st_nlink = %d\n", fullPath.c_str(), statbuf->st_nlink );   /* number of hard links */
  LOG(0, "%s: st_uid = %d\n", fullPath.c_str(), statbuf->st_uid );     /* user ID of owner */
  LOG(0, "%s: st_gid = %d\n", fullPath.c_str(), statbuf->st_gid );     /* group ID of owner */
  LOG(0, "%s: st_rdev = %d\n", fullPath.c_str(), statbuf->st_rdev );    /* device ID (if special file) */
  LOG(0, "%s: st_size = %d\n", fullPath.c_str(), statbuf->st_size );    /* total size, in bytes */
  LOG(0, "%s: st_blksize = %d\n", fullPath.c_str(), statbuf->st_blksize ); /* blocksize for file system I/O */
  LOG(0, "%s: st_blocks = %d\n", fullPath.c_str(), statbuf->st_blocks );  /* number of 512B blocks allocated */
  LOG(0, "%s: st_atime = %d\n", fullPath.c_str(), statbuf->st_atime );   /* time of last access */
  LOG(0, "%s: st_mtime = %d\n", fullPath.c_str(), statbuf->st_mtime );   /* time of last modification */
  LOG(0, "%s: st_ctime = %d\n", fullPath.c_str(), statbuf->st_ctime );   /* time of last status change */
}

PassthruFS* PassthruFS::Instance() {
  if(_instance == NULL) {
    _instance = new PassthruFS();
  }
  return _instance;
}

PassthruFS::PassthruFS(): m_root() {
}

PassthruFS::~PassthruFS() {
  LOG(0,"Unmounted 'tagfs' rooted from '%s'", m_root.c_str() );
  _instance = NULL;
}

void PassthruFS::setRootDir(const char *path) {
  do {
    char *_path = realpath(path, NULL);
    if(_path==NULL) {
      LOG(0,"Error ('%s') setting FS root to: %s\n", strerror(errno), path);
    }
    m_root = _path;
    free(_path);
  } while(false);
  LOG(0,"setting FS root to: %s\n", m_root.c_str());
}

int PassthruFS::Getattr(const char *path, struct stat *statbuf) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  auto *context = fuse_get_context();
  std::string root_path = (m_root + "/" + path);
  auto r = lstat(root_path.c_str(), statbuf);
  if(r!=0) {
    r = -errno;
    LOG(0,"lstat('%s', statbuf) => %d\n", root_path.c_str(), errno);
    return r;
  }
  return 0;
}

int PassthruFS::Readlink(const char *path, char *link, size_t size) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"readlink(path=%s, link=%s, size=%d)\n", path, link, (int)size);
  return -ENOENT;
}

/** \brief Handles creating FIFOs, regular files, etc...
*/
int PassthruFS::Mknod(const char *path, mode_t mode, dev_t /*dev*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"mknod(path=%s, mode=%o)\n", path, mode);
  return -EPERM;
}

/** \todo Decide what to do on \em original filesystem */
int PassthruFS::Mkdir(const char *path, mode_t mode) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"mkdir(path=%s, mode=%o)\n", path, (int)mode);
  return -EPERM;
}

int PassthruFS::Unlink(const char *path) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"unlink(path=%s)", path);
  return -ENOENT;
}

int PassthruFS::Rmdir(const char *path) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"rmdir(path=%s\n)", path);
  return -ENOENT;
}

int PassthruFS::Symlink(const char *path, const char *link) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"symlink(path=%s, link=%s)\n", path, link);
  return -ENOENT;
}

int PassthruFS::Rename(const char *src, const char *tgt) {
  LOG(0,"%s: '%s' '%s'\n", __PRETTY_FUNCTION__, src, tgt );
  return -ENOENT;
}

int PassthruFS::Link(const char *path, const char *newpath) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"link(path=%s, newPath=%s)\n", path, newpath);
  return -ENOENT;
}

int PassthruFS::Chmod(const char *path, mode_t mode) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"chmod(path=%s, mode=%o)\n", path, mode);
  return -ENOENT;
}

int PassthruFS::Chown(const char *path, uid_t uid, gid_t gid) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"chown(path=%s, uid=%d, gid=%d)\n", path, (int)uid, (int)gid);
  return -ENOENT;
}

int PassthruFS::Truncate(const char *path, off_t newSize) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"truncate(path=%s, newSize=%d\n", path, (int)newSize);
  return -ENOENT;
}

int PassthruFS::Utime(const char *path, struct utimbuf * /*ubuf*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int PassthruFS::Open(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int PassthruFS::Read(const char *path, char * /*buf*/, size_t /*size*/, off_t /*offset*/, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int PassthruFS::Write(const char *path, const char * /*buf*/, size_t /*size*/, off_t /*offset*/, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int PassthruFS::Statfs(const char *path, struct statvfs *statInfo) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"statfs(path=%s)\n", path);
  RETURN_ERRNO(statvfs(m_root.c_str(), statInfo));
  //return -ENOENT;
}

int PassthruFS::Flush(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  //noop because we don't maintain our own buffers
  //LOG(0,"flush(path=%s)\n", path);
  return 0;
}

int PassthruFS::Release(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int PassthruFS::Fsync(const char *path, int /*datasync*/, struct fuse_file_info * /*fi*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int PassthruFS::Setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"setxattr(path=%s, name=%s, value=%s, size=%d, flags=%d\n", path, name, value, (int)size, flags);
  return -ENOENT;
}

int PassthruFS::Getxattr(const char *path, const char *name, char * /*value*/, size_t size) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"getxattr(path=%s, name=%s, size=%d\n", path, name, (int)size);
  return -ENOENT;
}

int PassthruFS::Listxattr(const char *path, char * /*list*/, size_t size) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"listxattr(path=%s, size=%d)\n", path, (int)size);
  return -ENOENT;
}

int PassthruFS::Removexattr(const char *path, const char *name) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"removexattry(path=%s, name=%s)\n", path, name);
  return -ENOENT;
}

int PassthruFS::Opendir(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  if(strcmp(path, "/")==0 || strcmp(path, "/test")==0) {
    return 0;
  }
  return -ENOENT;
}

int PassthruFS::Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s' %zu\n", __PRETTY_FUNCTION__, path, offset );

  DIR *dp = opendir((m_root + "/" + path).c_str());
  if(dp) {
    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
      if (filler(buf, de->d_name, NULL, 0)) break;
    }
    closedir(dp);
  }
  return 0;
}

int PassthruFS::Releasedir(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return 0;
}

int PassthruFS::Fsyncdir(const char *path, int /*datasync*/, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  return 0;
}

void *PassthruFS::Init(struct fuse_conn_info * /*conn*/) {
  LOG(0,"%s\n", __PRETTY_FUNCTION__ );
  LOG(0,"init\n");
  return _instance;
}

int PassthruFS::Truncate(const char *path, off_t offset, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'\n", __PRETTY_FUNCTION__, path );
  LOG(0,"truncate(path=%s, offset=%d)\n", path, (int)offset);
  return -ENOENT;
}

