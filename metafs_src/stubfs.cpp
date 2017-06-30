#include <cassert>
#include <string>
#include <algorithm>

#include "stubfs.h"
#include "log.h"

void StubFS::dump_stat(const std::string &fullPath, struct stat *statbuf) const
{
  LOG(0, "%s: st_dev = %d", fullPath.c_str(), statbuf->st_dev );  /* ID of device containing file */
  LOG(0, "%s: st_ino = %d", fullPath.c_str(), statbuf->st_ino );  /* inode number */
  LOG(0, "%s: st_mode = %o", fullPath.c_str(), statbuf->st_mode );    /* protection */
  LOG(0, "%s: st_nlink = %d", fullPath.c_str(), statbuf->st_nlink );   /* number of hard links */
  LOG(0, "%s: st_uid = %d", fullPath.c_str(), statbuf->st_uid );     /* user ID of owner */
  LOG(0, "%s: st_gid = %d", fullPath.c_str(), statbuf->st_gid );     /* group ID of owner */
  LOG(0, "%s: st_rdev = %d", fullPath.c_str(), statbuf->st_rdev );    /* device ID (if special file) */
  LOG(0, "%s: st_size = %d", fullPath.c_str(), statbuf->st_size );    /* total size, in bytes */
  LOG(0, "%s: st_blksize = %d", fullPath.c_str(), statbuf->st_blksize ); /* blocksize for file system I/O */
  LOG(0, "%s: st_blocks = %d", fullPath.c_str(), statbuf->st_blocks );  /* number of 512B blocks allocated */
  LOG(0, "%s: st_atime = %d", fullPath.c_str(), statbuf->st_atime );   /* time of last access */
  LOG(0, "%s: st_mtime = %d", fullPath.c_str(), statbuf->st_mtime );   /* time of last modification */
  LOG(0, "%s: st_ctime = %d", fullPath.c_str(), statbuf->st_ctime );   /* time of last status change */
}

int StubFS::Getattr(const char *path, struct stat * /*statbuf*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"Getattr(path=%s)", path );
  return -ENOENT;
}

int StubFS::Readlink(const char *path, char *link, size_t size) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"readlink(path=%s, link=%s, size=%d)", path, link, (int)size);
  return -ENOENT;
}

/** \brief Handles creating FIFOs, regular files, etc...
*/
int StubFS::Mknod(const char *path, mode_t mode, dev_t /*dev*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"mknod(path=%s, mode=%o)", path, mode);
  return -EPERM;
}

/** \todo Decide what to do on \em original filesystem */
int StubFS::Mkdir(const char *path, mode_t mode) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"mkdir(path=%s, mode=%o)", path, (int)mode);
  return -EPERM;
}

int StubFS::Unlink(const char *path) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"unlink(path=%s)", path);
  return -ENOENT;
}

int StubFS::Rmdir(const char *path) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"rmdir(path=%s\n)", path);
  return -ENOENT;
}

int StubFS::Symlink(const char *path, const char *link) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"symlink(path=%s, link=%s)", path, link);
  return -ENOENT;
}

int StubFS::Rename(const char *src, const char *tgt) {
  LOG(0,"%s: '%s' '%s'", __PRETTY_FUNCTION__, src, tgt );
  return -ENOENT;
}

int StubFS::Link(const char *path, const char *newpath) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"link(path=%s, newPath=%s)", path, newpath);
  return -ENOENT;
}

int StubFS::Chmod(const char *path, mode_t mode) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"chmod(path=%s, mode=%o)", path, mode);
  return -ENOENT;
}

int StubFS::Chown(const char *path, uid_t uid, gid_t gid) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"chown(path=%s, uid=%d, gid=%d)", path, (int)uid, (int)gid);
  return -ENOENT;
}

int StubFS::Truncate(const char *path, off_t newSize) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"truncate(path=%s, newSize=%d", path, (int)newSize);
  return -ENOENT;
}

int StubFS::Utime(const char *path, struct utimbuf * /*ubuf*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int StubFS::Open(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int StubFS::Read(const char *path, char * /*buf*/, size_t /*size*/, off_t /*offset*/, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int StubFS::Write(const char *path, const char * /*buf*/, size_t /*size*/, off_t /*offset*/, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int StubFS::Statfs(const char *path, struct statvfs * /*statInfo*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"statfs(path=%s)", path);
  //RETURN_ERRNO(statvfs(m_root.c_str(), statInfo));
  return -ENOENT;
}

int StubFS::Flush(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  //noop because we don't maintain our own buffers
  return 0;
}

int StubFS::Release(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int StubFS::Fsync(const char *path, int /*datasync*/, struct fuse_file_info* /*fi*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  return -ENOENT;
}

int StubFS::Setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"setxattr(path=%s, name=%s, value=%s, size=%d, flags=%d", path, name, value, (int)size, flags);
  return -ENOENT;
}

int StubFS::Getxattr(const char *path, const char *name, char * /*value*/, size_t size) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"getxattr(path=%s, name=%s, size=%d", path, name, (int)size);
  return -ENOENT;
}

int StubFS::Listxattr(const char *path, char * /*list*/, size_t size) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"listxattr(path=%s, size=%d)", path, (int)size);
  return -ENOENT;
}

int StubFS::Removexattr(const char *path, const char *name) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"removexattry(path=%s, name=%s)", path, name);
  return -ENOENT;
}

int StubFS::Opendir(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  if(strcmp(path, "/")==0 || strcmp(path, "/test")==0) {
    return 0;
  }
  return -ENOENT;
}

int StubFS::Readdir(const char *path, void * /*buf*/, fuse_fill_dir_t /*filler*/, off_t offset, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s' %zu", __PRETTY_FUNCTION__, path, offset );

  return -ENOENT;
}

int StubFS::Releasedir(const char *path, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  return 0;
}

int StubFS::Fsyncdir(const char *path, int /*datasync*/, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  return 0;
}

/*
void *StubFS::Init(struct fuse_conn_info *conn) {
  LOG(0, "%s\n", __PRETTY_FUNCTION__ );
  LOG(0, "init\n");
  return _instance;
}
*/

int StubFS::Truncate(const char *path, off_t offset, struct fuse_file_info * /*fileInfo*/) {
  LOG(0,"%s: '%s'", __PRETTY_FUNCTION__, path );
  LOG(0,"truncate(path=%s, offset=%d)", path, (int)offset);
  return -ENOENT;
}

