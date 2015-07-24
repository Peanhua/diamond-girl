
mapsdir = $(pkgdatadir)/maps/random
maps_DATA = 100.bz2 
maps_SOURCEFILES = 100 

100.bz2  : %.bz2 : @srcdir@/%
	$(AM_V_GEN) bzip2 --stdout --compress < $< > $@

EXTRA_DIST = $(maps_SOURCEFILES)

CLEANFILES = $(maps_DATA)

