## Makefile.am -- Process this file with automake to produce Makefile.in
AM_CPPFLAGS   = -I$(top_builddir) -I$(top_srcdir) \
                -I$(top_builddir)/src -I$(top_srcdir)/src

AUTOMAKE_OPTIONS = color-tests

TESTS = metafs_regressions

SUBDIRS = 

check_PROGRAMS = 
bin_PROGRAMS =

bin_PROGRAMS+=tidy
tidy_SOURCES = src/main.cpp
tidy_SOURCES+= src/options.cpp src/options.h

bin_PROGRAMS += fdupes
fdupes_SOURCES = src/fdupes.cpp
fdupes_SOURCES+= src/crc_32.h src/crc_32.cpp

bin_PROGRAMS += logfs
logfs_SOURCES = logfs_src/main.c
logfs_SOURCES += logfs_src/wrap.cc logfs_src/wrap.hh
logfs_SOURCES += logfs_src/logfs.cpp logfs_src/logfs.h
logfs_SOURCES += logfs_src/log.cpp logfs_src/log.h
logfs_CXXFLAGS = $(FUSE_CFLAGS) $(SQLITE_CFLAGS) -g
logfs_CFLAGS = $(FUSE_CFLAGS) $(SQLITE_CFLAGS) -g
logfs_LDADD = $(FUSE_LIBS) $(SQLITE_LIBS)

bin_PROGRAMS += passthrufs
passthrufs_SOURCES = passthrufs_src/main.c
passthrufs_SOURCES += passthrufs_src/wrap.cc passthrufs_src/wrap.hh
passthrufs_SOURCES += passthrufs_src/passthrufs.cpp passthrufs_src/passthrufs.h
passthrufs_SOURCES += passthrufs_src/log.cpp passthrufs_src/log.h
passthrufs_CXXFLAGS = -DBOOST_LOG_DYN_LINK $(PTHREAD_CFLAGS) $(FUSE_CFLAGS) $(SQLITE_CFLAGS) $(BOOST_CPPFLAGS) -g
passthrufs_CFLAGS = -DBOOST_LOG_DYN_LINK $(PTHREAD_CFLAGS) $(FUSE_CFLAGS) $(SQLITE_CFLAGS) -g
passthrufs_LDFLAGS = -DBOOST_LOG_DYN_LINK $(BOOST_LDFLAGS)
passthrufs_LDADD = $(PTHREAD_LIBS) $(FUSE_LIBS) $(SQLITE_LIBS) $(BOOST_SYSTEM_LIB) $(BOOST_THREAD_LIB) $(BOOST_LOG_LIB)

bin_PROGRAMS += metafs
metafs_SOURCES = metafs_src/main.c
metafs_SOURCES += metafs_src/wrap.cc metafs_src/wrap.hh
metafs_SOURCES += metafs_src/metafs.cpp metafs_src/metafs.h
metafs_SOURCES += metafs_src/stubfs.cpp metafs_src/stubfs.h
metafs_SOURCES += metafs_src/log.cpp metafs_src/log.h
metafs_SOURCES += metafs_src/entries.cpp metafs_src/entries.h
metafs_SOURCES += metafs_src/entries_scanner.cpp metafs_src/entries_scanner.h
metafs_SOURCES += metafs_src/metafs_config.cpp metafs_src/metafs_config.h metafs_src/metafs_config.hh

metafs_SOURCES += metafs_src/plugin.cpp metafs_src/plugin.h
metafs_SOURCES += metafs_src/plugin_magic.cpp
metafs_SOURCES += metafs_src/plugin_size.cpp
metafs_CXXFLAGS = -DBOOST_LOG_DYN_LINK $(PTHREAD_CFLAGS) $(FUSE_CFLAGS) $(SQLITE_CFLAGS) $(BOOST_CPPFLAGS) -g
metafs_CFLAGS = -DBOOST_LOG_DYN_LINK $(PTHREAD_CFLAGS) $(FUSE_CFLAGS) $(SQLITE_CFLAGS) -g
metafs_LDFLAGS = -DBOOST_LOG_DYN_LINK $(BOOST_LDFLAGS)
metafs_LDADD = $(PTHREAD_LIBS) $(FUSE_LIBS) $(SQLITE_LIBS) $(BOOST_SYSTEM_LIB) $(BOOST_THREAD_LIB) $(BOOST_LOG_LIB)

strip: $(bin_PROGRAMS)
	$(STRIP) $^

TESTS += $(check_PROGRAMS)

package: distdir
	tardir=$(distdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	cd $(distdir) && \
	dh_make --library -e '$(PACKAGE_BUGREPORT)' -f ../$(distdir).tar.gz && \
	mv debian/control debian/control.EX && \
	awk '{if(match($$0, /^Build-Depends:/)){print $$0 depends}else{print $$0}}' depends="`dpkg-depcheck -d ./configure 2>&1 | sed 's|^[[:space:]]\+||g' | awk '{if(match($$0, /^Packages.needed:$$/)){keep=1}else if(keep==1){str=str", "$$0}}END{print str}'`" debian/control.EX | sed 's|BROKEN|1|g' > debian/control && \
	[ -f debian/$(PACKAGE)-dev.install ] && ( mv debian/$(PACKAGE)-dev.install debian/$(PACKAGE)-dev.install.EX && \
	  cat debian/$(PACKAGE)-dev.install.EX | grep -v usr/lib/pkgconfig/ > debian/$(PACKAGE)-dev.install ) && \
	[ -f debian/$(PACKAGE)1.install ] && echo "usr/bin/*" >> debian/$(PACKAGE)1.install && \
	dpkg-buildpackage -sa -rfakeroot -us
