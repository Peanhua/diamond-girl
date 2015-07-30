gfxdir = $(pkgdatadir)/themes/new-year/gfx
sand.png extra_life_anim.png exit_level.png empty.png boulder.png : glyphs-24x24.xcf
	$(AM_V_GEN) xcf2png --autocrop $< $$(basename $@ .png) > $@ 2>/dev/null
	$(AM_V_GEN) if [ -n "@optipng@" ]; then @optipng@ -nc -quiet $@ ; fi

 : %.bz2 : %
	$(AM_V_GEN) bzip2 --stdout --compress < $< > $@

gfx_DATA = sand.png extra_life_anim.png exit_level.png empty.png boulder.png extra_life_anim.png.cnf 
CLEANFILES = sand.png sand.png.bak extra_life_anim.png extra_life_anim.png.bak exit_level.png exit_level.png.bak empty.png empty.png.bak boulder.png boulder.png.bak 
EXTRA_DIST = glyphs-24x24.xcf extra_life_anim.png.cnf 
