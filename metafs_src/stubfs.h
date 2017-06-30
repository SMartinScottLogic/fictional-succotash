#ifndef STUBFS_H
#define STUBFS_H

#include <cctype>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <limits.h>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/xattr.h>

#include <string>
#include <set>
#include <vector>

#define RETURN_ERRNO(x) do {if ((x) == 0) { return 0; } else { LOG(logging::trivial::info, "%s => %d\n", #x, errno); return (-errno);}}while(false)
//#define RETURN_ERRNO(x) do {if ((x) == 0) { return 0; } else { BOOST_LOG_SEV(logger, logging::trivial::info) << x << " => " << errno;return (-errno);}}while(false)

class StubFS {
  public:
    virtual ~StubFS() {};

    virtual void *Init(struct fuse_conn_info *conn)=0;

    virtual int Getattr(const char *path, struct stat *statbuf);
    virtual int Readlink(const char *path, char *link, size_t size);
    virtual int Mknod(const char *path, mode_t mode, dev_t dev);
    virtual int Mkdir(const char *path, mode_t mode);
    virtual int Unlink(const char *path);
    virtual int Rmdir(const char *path);
    virtual int Symlink(const char *path, const char *link);
    virtual int Rename(const char *path, const char *newpath);
    virtual int Link(const char *path, const char *newpath);
    virtual int Chmod(const char *path, mode_t mode);
    virtual int Chown(const char *path, uid_t uid, gid_t gid);
    virtual int Truncate(const char *path, off_t newSize);
    virtual int Utime(const char *path, struct utimbuf *ubuf);
    virtual int Open(const char *path, struct fuse_file_info *fileInfo);
    virtual int Read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
    virtual int Write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
    virtual int Statfs(const char *path, struct statvfs *statInfo);
    virtual int Flush(const char *path, struct fuse_file_info *fileInfo);
    virtual int Release(const char *path, struct fuse_file_info *fileInfo);
    virtual int Fsync(const char *path, int datasync, struct fuse_file_info *fi);
    virtual int Setxattr(const char *path, const char *name, const char *value, size_t size, int flags);
    virtual int Getxattr(const char *path, const char *name, char *value, size_t size);
    virtual int Listxattr(const char *path, char *list, size_t size);
    virtual int Removexattr(const char *path, const char *name);
    virtual int Opendir(const char *path, struct fuse_file_info *fileInfo);
    virtual int Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo);
    virtual int Releasedir(const char *path, struct fuse_file_info *fileInfo);
    virtual int Fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo);
    virtual int Truncate(const char *path, off_t offset, struct fuse_file_info *fileInfo);
  protected:
    void dump_stat(const std::string &fullPath, struct stat *statbuf) const;
};

#endif//STUBFS_H

