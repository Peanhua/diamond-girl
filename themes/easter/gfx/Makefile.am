gfxdir = $(pkgdatadir)/themes/easter/gfx
diamond.png : glyphs-24x24.xcf
	$(AM_V_GEN) xcf2png --autocrop $< $$(basename $@ .png) > $@ 2>/dev/null
	$(AM_V_GEN) if [ -n "@optipng@" ]; then @optipng@ -nc -quiet $@ ; fi

 : %.bz2 : %
	$(AM_V_GEN) bzip2 --stdout --compress < $< > $@

gfx_DATA = diamond.png 
CLEANFILES = diamond.png diamond.png.bak 
EXTRA_DIST = glyphs-24x24.xcf 
