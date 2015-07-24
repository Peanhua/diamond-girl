
mapsdir = $(pkgdatadir)/maps/beginners_cave
maps_DATA = 1.bz2 10.bz2 2.bz2 3.bz2 4.bz2 5.bz2 6.bz2 7.bz2 8.bz2 9.bz2 
maps_SOURCEFILES = 1 10 2 3 4 5 6 7 8 9 

1.bz2 10.bz2 2.bz2 3.bz2 4.bz2 5.bz2 6.bz2 7.bz2 8.bz2 9.bz2  : %.bz2 : @srcdir@/%
	$(AM_V_GEN) bzip2 --stdout --compress < $< > $@

EXTRA_DIST = $(maps_SOURCEFILES)

CLEANFILES = $(maps_DATA)

