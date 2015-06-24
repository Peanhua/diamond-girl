extradatadir = $(pkgdatadir)/themes

extradata_DATA = easter.config.bz2 halloween.config.bz2 iron-girl.config.bz2 new-year.config.bz2 x-mas.config.bz2 

extradata_SOURCEFILES = easter.config halloween.config iron-girl.config new-year.config x-mas.config 

easter.config.bz2 halloween.config.bz2 iron-girl.config.bz2 new-year.config.bz2 x-mas.config.bz2  : %.bz2 : %
	$(AM_V_GEN) bzip2 --stdout --compress < $< > $@

EXTRA_DIST = $(extradata_SOURCEFILES)

CLEANFILES = easter.config.bz2 halloween.config.bz2 iron-girl.config.bz2 new-year.config.bz2 x-mas.config.bz2 

SUBDIRS = default new-year halloween x-mas easter iron-girl

