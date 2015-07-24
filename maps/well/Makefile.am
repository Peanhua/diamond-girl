
mapsdir = $(pkgdatadir)/maps/well
maps_DATA = 1.bz2 
maps_SOURCEFILES = 1 

1.bz2  : %.bz2 : @srcdir@/%
	$(AM_V_GEN) bzip2 --stdout --compress < $< > $@

EXTRA_DIST = $(maps_SOURCEFILES)

CLEANFILES = $(maps_DATA)

