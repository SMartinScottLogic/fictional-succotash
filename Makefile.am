## Makefile.am -- Process this file with automake to produce Makefile.in
INCLUDES        = -I$(top_builddir) -I$(top_srcdir) \
                -I$(top_builddir)/src -I$(top_srcdir)/src

AUTOMAKE_OPTIONS = color-tests

SUBDIRS = 

check_PROGRAMS = 
bin_PROGRAMS =

bin_PROGRAMS+=tidy
tidy_SOURCES = src/main.cpp
tidy_SOURCES+= src/options.cpp src/options.h

TESTS = $(check_PROGRAMS)

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
