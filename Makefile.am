SUBDIRS = po themes src maps data ui tests

miscdir   = $(pkgdatadir)
misc_DATA = CREDITS README LICENSE Changelog

appsdir   = $(datadir)/applications
apps_DATA = diamond-girl.desktop

man_MANS = diamond-girl.6


EXTRA_DIST = config.rpath m4/ChangeLog  \
	$(misc_DATA) \
	$(apps_DATA) \
	$(man_MANS)  \
	diamond-girl-win32.nsi \
	README.building README.sfx README.translations \
	translate.sh \
	translate-map-fi.sed translate-unmap-fi.sed \
	translate-map-fr.sed translate-unmap-fr.sed \
	mtwist-1.5/CHANGELOG \
	mtwist-1.5/README \
	mtwist-1.5/mtcctest.cc \
	mtwist-1.5/mttest.c \
	mtwist-1.5/mtwist.3 \
	mtwist-1.5/mtwist.c \
	mtwist-1.5/mtwist.h \
	mtwist-1.5/randistrs.3 \
	mtwist-1.5/randistrs.c \
	mtwist-1.5/randistrs.h \
	mtwist-1.5/rdcctest.cc \
	mtwist-1.5/rdtest.c

ACLOCAL_AMFLAGS = -I m4

TESTS = tests/random
