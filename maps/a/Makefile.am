
mapsdir = $(pkgdatadir)/maps/a
maps_DATA = 1.bz2 10.bz2 11.bz2 12.bz2 13.bz2 14.bz2 15.bz2 16.bz2 17.bz2 18.bz2 19.bz2 2.bz2 20.bz2 21.bz2 22.bz2 23.bz2 24.bz2 25.bz2 26.bz2 27.bz2 28.bz2 29.bz2 3.bz2 30.bz2 31.bz2 32.bz2 33.bz2 4.bz2 5.bz2 6.bz2 7.bz2 8.bz2 9.bz2 
maps_SOURCEFILES = 1 10 11 12 13 14 15 16 17 18 19 2 20 21 22 23 24 25 26 27 28 29 3 30 31 32 33 4 5 6 7 8 9 

1.bz2 10.bz2 11.bz2 12.bz2 13.bz2 14.bz2 15.bz2 16.bz2 17.bz2 18.bz2 19.bz2 2.bz2 20.bz2 21.bz2 22.bz2 23.bz2 24.bz2 25.bz2 26.bz2 27.bz2 28.bz2 29.bz2 3.bz2 30.bz2 31.bz2 32.bz2 33.bz2 4.bz2 5.bz2 6.bz2 7.bz2 8.bz2 9.bz2  : %.bz2 : @srcdir@/%
	$(AM_V_GEN) bzip2 --stdout --compress < $< > $@

EXTRA_DIST = $(maps_SOURCEFILES)

CLEANFILES = $(maps_DATA)

