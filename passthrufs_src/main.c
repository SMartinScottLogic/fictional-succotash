#include "wrap.hh"
#include <fuse.h>
#include <stdio.h>
#include <stddef.h>

struct fuse_operations passthrufs_oper;

/** options for fuse_opt.h */
struct passthrufs_options {
  char* root;
};

struct passthrufs_options options;

/** macro to define options */
#define TAGFS_OPT_KEY(t, p, v) { t, offsetof(struct passthrufs_options, p), v }

/** keys for FUSE_OPT_ options */
enum
{
  KEY_VERSION,
  KEY_HELP,
};

static struct fuse_opt passthrufs_opts[] =
{
  TAGFS_OPT_KEY("--source %s", root, 0),
  TAGFS_OPT_KEY("source=%s", root, 0),

  // #define FUSE_OPT_KEY(templ, key) { templ, -1U, key }
  FUSE_OPT_KEY("-V",             KEY_VERSION),
  FUSE_OPT_KEY("--version",      KEY_VERSION),
  FUSE_OPT_KEY("-h",             KEY_HELP),
  FUSE_OPT_KEY("--help",         KEY_HELP),
  FUSE_OPT_END
};

void setup() {
  passthrufs_oper.getattr = wrap_getattr;
  passthrufs_oper.readlink = wrap_readlink;
  passthrufs_oper.getdir = NULL;
  passthrufs_oper.mknod = wrap_mknod;
  passthrufs_oper.mkdir = wrap_mkdir;
  passthrufs_oper.unlink = wrap_unlink;
  passthrufs_oper.rmdir = wrap_rmdir;
  passthrufs_oper.symlink = wrap_symlink;
  passthrufs_oper.rename = wrap_rename;
  passthrufs_oper.link = wrap_link;
  passthrufs_oper.chmod = wrap_chmod;
  passthrufs_oper.chown = wrap_chown;
  passthrufs_oper.truncate = wrap_truncate;
  passthrufs_oper.utime = wrap_utime;
  passthrufs_oper.open = wrap_open;
  passthrufs_oper.read = wrap_read;
  passthrufs_oper.write = wrap_write;
  passthrufs_oper.statfs = wrap_statfs;
  passthrufs_oper.flush = wrap_flush;
  passthrufs_oper.release = wrap_release;
  passthrufs_oper.fsync = wrap_fsync;
#if 0
  passthrufs_oper.setxattr = wrap_setxattr;
  passthrufs_oper.getxattr = wrap_getxattr;
  passthrufs_oper.listxattr = wrap_listxattr;
  passthrufs_oper.removexattr = wrap_removexattr;
#else
  passthrufs_oper.setxattr = NULL;
  passthrufs_oper.getxattr = NULL;
  passthrufs_oper.listxattr = NULL;
  passthrufs_oper.removexattr = NULL;
#endif
  passthrufs_oper.opendir = wrap_opendir;
  passthrufs_oper.readdir = wrap_readdir;
  passthrufs_oper.releasedir = wrap_releasedir;
  passthrufs_oper.fsyncdir = wrap_fsyncdir;
  passthrufs_oper.init = wrap_init;
  passthrufs_oper.destroy = wrap_destroy;
}

int main(int argc, char *argv[]) {
  int ret;
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

  /* clear structure that holds our options */
  memset(&options, 0, sizeof(struct passthrufs_options));

  if (fuse_opt_parse(&args, &options, passthrufs_opts, NULL) == -1) {
    /** error parsing options */
    return -1;
  }

  if(options.root==NULL) {
    fprintf(stderr, "ERROR: No source provided. Aborting.");
    exit(-1);
  }

  setup();
  set_rootdir(options.root);
  scan_rootdir();
  printf("mounting file system...\n");

  ret = fuse_main(args.argc, args.argv, &passthrufs_oper, NULL);

  if (ret) {
    printf("\n");
  }

  /** free arguments */
  fuse_opt_free_args(&args);

  return ret;
}


