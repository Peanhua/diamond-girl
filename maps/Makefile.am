
a_mapsdir = $(pkgdatadir)/maps/a
a_maps_DATA = a/1.bz2 a/10.bz2 a/11.bz2 a/12.bz2 a/13.bz2 a/14.bz2 a/15.bz2 a/16.bz2 a/17.bz2 a/18.bz2 a/19.bz2 a/2.bz2 a/20.bz2 a/21.bz2 a/22.bz2 a/23.bz2 a/24.bz2 a/25.bz2 a/26.bz2 a/27.bz2 a/28.bz2 a/29.bz2 a/3.bz2 a/30.bz2 a/31.bz2 a/32.bz2 a/33.bz2 a/4.bz2 a/5.bz2 a/6.bz2 a/7.bz2 a/8.bz2 a/9.bz2 
a_maps_SOURCEFILES = a/1 a/10 a/11 a/12 a/13 a/14 a/15 a/16 a/17 a/18 a/19 a/2 a/20 a/21 a/22 a/23 a/24 a/25 a/26 a/27 a/28 a/29 a/3 a/30 a/31 a/32 a/33 a/4 a/5 a/6 a/7 a/8 a/9 

beginners_cave_mapsdir = $(pkgdatadir)/maps/beginners_cave
beginners_cave_maps_DATA = beginners_cave/1.bz2 beginners_cave/10.bz2 beginners_cave/2.bz2 beginners_cave/3.bz2 beginners_cave/4.bz2 beginners_cave/5.bz2 beginners_cave/6.bz2 beginners_cave/7.bz2 beginners_cave/8.bz2 beginners_cave/9.bz2 
beginners_cave_maps_SOURCEFILES = beginners_cave/1 beginners_cave/10 beginners_cave/2 beginners_cave/3 beginners_cave/4 beginners_cave/5 beginners_cave/6 beginners_cave/7 beginners_cave/8 beginners_cave/9 

random_mapsdir = $(pkgdatadir)/maps/random
random_maps_DATA = random/100.bz2 
random_maps_SOURCEFILES = random/100 

well_mapsdir = $(pkgdatadir)/maps/well
well_maps_DATA = well/1.bz2 
well_maps_SOURCEFILES = well/1 

a/1.bz2 a/10.bz2 a/11.bz2 a/12.bz2 a/13.bz2 a/14.bz2 a/15.bz2 a/16.bz2 a/17.bz2 a/18.bz2 a/19.bz2 a/2.bz2 a/20.bz2 a/21.bz2 a/22.bz2 a/23.bz2 a/24.bz2 a/25.bz2 a/26.bz2 a/27.bz2 a/28.bz2 a/29.bz2 a/3.bz2 a/30.bz2 a/31.bz2 a/32.bz2 a/33.bz2 a/4.bz2 a/5.bz2 a/6.bz2 a/7.bz2 a/8.bz2 a/9.bz2 beginners_cave/1.bz2 beginners_cave/10.bz2 beginners_cave/2.bz2 beginners_cave/3.bz2 beginners_cave/4.bz2 beginners_cave/5.bz2 beginners_cave/6.bz2 beginners_cave/7.bz2 beginners_cave/8.bz2 beginners_cave/9.bz2 random/100.bz2 well/1.bz2  : %.bz2 : %
	$(AM_V_GEN) bzip2 --stdout --compress < $< > $@

EXTRA_DIST = $(a_maps_SOURCEFILES) $(beginners_cave_maps_SOURCEFILES) $(random_maps_SOURCEFILES) $(well_maps_SOURCEFILES) 

CLEANFILES = $(a_maps_DATA) $(beginners_cave_maps_DATA) $(random_maps_DATA) $(well_maps_DATA) 

