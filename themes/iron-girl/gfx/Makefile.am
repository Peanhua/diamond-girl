gfxdir = $(pkgdatadir)/themes/iron-girl/gfx
player-2.png player-1.png player-0.png : glyphs-24x24.xcf
	$(AM_V_GEN) xcf2png --autocrop $< $$(basename $@ .png) > $@ 2>/dev/null
	$(AM_V_GEN) if [ -n "@optipng@" ]; then @optipng@ -nc -quiet $@ ; fi

 : %.bz2 : %
	$(AM_V_GEN) bzip2 --stdout --compress < $< > $@

gfx_DATA = player-2.png player-1.png player-0.png 
CLEANFILES = player-2.png player-2.png.bak player-1.png player-1.png.bak player-0.png player-0.png.bak 
EXTRA_DIST = glyphs-24x24.xcf 
