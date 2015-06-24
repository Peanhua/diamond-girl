gfxdir = $(pkgdatadir)/themes/x-mas/gfx
player-2.png player-1.png player-0.png boulder.png : glyphs-24x24.xcf
	$(AM_V_GEN) xcf2png --autocrop $< $$(basename $@ .png) > $@
	$(AM_V_GEN) if [ -n "@optipng@" ]; then @optipng@ -nc -quiet $@ ; fi

 : %.bz2 : %
	$(AM_V_GEN) bzip2 --stdout --compress < $< > $@

gfx_DATA = player-2.png player-1.png player-0.png boulder.png 
CLEANFILES = player-2.png player-1.png player-0.png boulder.png 
EXTRA_DIST = glyphs-24x24.xcf 
