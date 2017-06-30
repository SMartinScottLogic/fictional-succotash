#include "wrap.hh"
#include <fuse.h>
#include <stdio.h>
#include <stddef.h>

struct fuse_operations examplefs_oper;

/** options for fuse_opt.h */
struct logfs_options {
  char* root;
};

struct logfs_options options;

/** macro to define options */
#define TAGFS_OPT_KEY(t, p, v) { t, offsetof(struct logfs_options, p), v }

/** keys for FUSE_OPT_ options */
enum
{
  KEY_VERSION,
  KEY_HELP,
};

static struct fuse_opt logfs_opts[] =
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
  examplefs_oper.getattr = wrap_getattr;
  examplefs_oper.readlink = wrap_readlink;
  examplefs_oper.getdir = NULL;
  examplefs_oper.mknod = wrap_mknod;
  examplefs_oper.mkdir = wrap_mkdir;
  examplefs_oper.unlink = wrap_unlink;
  examplefs_oper.rmdir = wrap_rmdir;
  examplefs_oper.symlink = wrap_symlink;
  examplefs_oper.rename = wrap_rename;
  examplefs_oper.link = wrap_link;
  examplefs_oper.chmod = wrap_chmod;
  examplefs_oper.chown = wrap_chown;
  examplefs_oper.truncate = wrap_truncate;
  examplefs_oper.utime = wrap_utime;
  examplefs_oper.open = wrap_open;
  examplefs_oper.read = wrap_read;
  examplefs_oper.write = wrap_write;
  examplefs_oper.statfs = wrap_statfs;
  examplefs_oper.flush = wrap_flush;
  examplefs_oper.release = wrap_release;
  examplefs_oper.fsync = wrap_fsync;
#if 0
  examplefs_oper.setxattr = wrap_setxattr;
  examplefs_oper.getxattr = wrap_getxattr;
  examplefs_oper.listxattr = wrap_listxattr;
  examplefs_oper.removexattr = wrap_removexattr;
#else
  examplefs_oper.setxattr = NULL;
  examplefs_oper.getxattr = NULL;
  examplefs_oper.listxattr = NULL;
  examplefs_oper.removexattr = NULL;
#endif
  examplefs_oper.opendir = wrap_opendir;
  examplefs_oper.readdir = wrap_readdir;
  examplefs_oper.releasedir = wrap_releasedir;
  examplefs_oper.fsyncdir = wrap_fsyncdir;
  examplefs_oper.init = wrap_init;
  examplefs_oper.destroy = wrap_destroy;
}

int main(int argc, char *argv[]) {
  int ret;
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

  /* clear structure that holds our options */
  memset(&options, 0, sizeof(struct logfs_options));

  if (fuse_opt_parse(&args, &options, logfs_opts, NULL) == -1) {
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

  ret = fuse_main(args.argc, args.argv, &examplefs_oper, NULL);

  if (ret) {
    printf("\n");
  }

  /** free arguments */
  fuse_opt_free_args(&args);

  return ret;
}


