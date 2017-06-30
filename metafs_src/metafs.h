#ifndef TAGFS_H
#define TAGFS_H

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
#include <deque>

#include "entries.h"
#include "stubfs.h"
#include "metafs_config.hh"

class MetaFS: public StubFS {
  public:
    static void InitLogging();

    void scan(const config &config);

    static MetaFS *Instance();
    void *Init(struct fuse_conn_info *conn);

    void set_config_file(const char *config_pathname);
    int Statfs(const char *path, struct statvfs *statInfo);
    int Getattr(const char *path, struct stat *statbuf);

    int Mkdir(const char *path, mode_t mode);
    int Opendir(const char *path, struct fuse_file_info *fileInfo);
    int Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo);
    int Releasedir(const char *path, struct fuse_file_info *fileInfo);

    int Mknod(const char *path, mode_t mode, dev_t dev);
    int Open(const char *path, struct fuse_file_info *fileInfo);
    int Flush(const char *path, struct fuse_file_info *fileInfo);
    int Release(const char *path, struct fuse_file_info *fileInfo);
    int Read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);

    int Unlink(const char *path);

    MetaFS();
    MetaFS(const MetaFS&)=delete;
    MetaFS &operator=(const MetaFS&)=delete;
    ~MetaFS();

  private:
    std::list<std::string> split_path(const std::string &path) const;
    const struct stat *generate_file_stat(const file_entry &, struct stat *statbuf) const;

    std::string m_config_pathname;
    static MetaFS *_instance;
    Entries *m_entries;
};



#endif//TAGFS_H
