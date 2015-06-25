gfxdir = $(pkgdatadir)/themes/default/gfx
mode_switch_up.png mode_switch_left.png mode_switch_right.png widget_select.png checkbox_on.png close_button.png : buttons-16x16.xcf
	$(AM_V_GEN) xcf2png --autocrop $< $$(basename $@ .png) > $@
	$(AM_V_GEN) if [ -n "@optipng@" ]; then @optipng@ -nc -quiet $@ ; fi

trait_quests.png trait_pyjama_party.png trait_iron_girl.png trait_key.png trait_dynamite.png trait_chaos.png trait_stars3.png trait_stars2.png trait_stars1.png trait_recycler.png trait_diamond_push.png trait_adventure_mode.png trait_ribbon.png trait_greedy.png trait_time_control.png trait_power_push.png forward.png rewind.png stop.png eject.png play.png : buttons-40x40.xcf
	$(AM_V_GEN) xcf2png --autocrop $< $$(basename $@ .png) > $@
	$(AM_V_GEN) if [ -n "@optipng@" ]; then @optipng@ -nc -quiet $@ ; fi

desktop-icon.png : desktop-icon.xcf
	$(AM_V_GEN) xcf2png --autocrop $< $$(basename $@ .png) > $@
	$(AM_V_GEN) if [ -n "@optipng@" ]; then @optipng@ -nc -quiet $@ ; fi

font-en.png font-fi.png font-fr.png : font.xcf
	$(AM_V_GEN) xcf2png --autocrop $< $$(basename $@ .png) > $@
	$(AM_V_GEN) if [ -n "@optipng@" ]; then @optipng@ -nc -quiet $@ ; fi

game_score.png : game_score.xcf
	$(AM_V_GEN) xcf2png --autocrop $< $$(basename $@ .png) > $@
	$(AM_V_GEN) if [ -n "@optipng@" ]; then @optipng@ -nc -quiet $@ ; fi

unbreakable_brick.png smoke.png sand.png player-2.png player-1.png player-0.png illegal.png game_border.png extra_life_anim.png exit_level.png enemy2.png enemy1.png treasure.png empty.png diamond.png brick_morpher.png brick_expanding.png brick.png boulder.png slime.png ameba.png : glyphs-24x24.xcf
	$(AM_V_GEN) xcf2png --autocrop $< $$(basename $@ .png) > $@
	$(AM_V_GEN) if [ -n "@optipng@" ]; then @optipng@ -nc -quiet $@ ; fi

intermission-en.png intermission-fi.png : intermission.xcf
	$(AM_V_GEN) xcf2png --autocrop $< $$(basename $@ .png) > $@
	$(AM_V_GEN) if [ -n "@optipng@" ]; then @optipng@ -nc -quiet $@ ; fi

logo_tex.png : logo_texture.xcf
	$(AM_V_GEN) xcf2png --autocrop $< $$(basename $@ .png) > $@
	$(AM_V_GEN) if [ -n "@optipng@" ]; then @optipng@ -nc -quiet $@ ; fi

map_cursor.png : map_cursor.xcf
	$(AM_V_GEN) xcf2png --autocrop $< $$(basename $@ .png) > $@
	$(AM_V_GEN) if [ -n "@optipng@" ]; then @optipng@ -nc -quiet $@ ; fi


pixmapsdir = $(datadir)/pixmaps
pixmaps_DATA = diamond-girl.png


diamond-girl.png: desktop-icon.png
	$(AM_V_GEN) cp $< $@

font-en.txt.bz2 font-fi.txt.bz2 font-fr.txt.bz2  : %.bz2 : %
	$(AM_V_GEN) bzip2 --stdout --compress < $< > $@

gfx_DATA = mode_switch_up.png mode_switch_left.png mode_switch_right.png widget_select.png checkbox_on.png close_button.png trait_quests.png trait_pyjama_party.png trait_iron_girl.png trait_key.png trait_dynamite.png trait_chaos.png trait_stars3.png trait_stars2.png trait_stars1.png trait_recycler.png trait_diamond_push.png trait_adventure_mode.png trait_ribbon.png trait_greedy.png trait_time_control.png trait_power_push.png forward.png rewind.png stop.png eject.png play.png desktop-icon.png font-en.png font-fi.png font-fr.png game_score.png unbreakable_brick.png smoke.png sand.png player-2.png player-1.png player-0.png illegal.png game_border.png extra_life_anim.png exit_level.png enemy2.png enemy1.png treasure.png empty.png diamond.png brick_morpher.png brick_expanding.png brick.png boulder.png slime.png ameba.png intermission-en.png intermission-fi.png logo_tex.png map_cursor.png logo.png logo.3ds ameba.png.cnf brick_morpher.png.cnf diamond.png.cnf empty.png.cnf enemy1.png.cnf enemy2.png.cnf exit_level.png.cnf extra_life_anim.png.cnf player-0.png.cnf player-1.png.cnf player-2.png.cnf player-3.png.cnf slime.png.cnf smoke.png.cnf font-en.txt.bz2 font-fi.txt.bz2 font-fr.txt.bz2 
CLEANFILES = mode_switch_up.png mode_switch_left.png mode_switch_right.png widget_select.png checkbox_on.png close_button.png trait_quests.png trait_pyjama_party.png trait_iron_girl.png trait_key.png trait_dynamite.png trait_chaos.png trait_stars3.png trait_stars2.png trait_stars1.png trait_recycler.png trait_diamond_push.png trait_adventure_mode.png trait_ribbon.png trait_greedy.png trait_time_control.png trait_power_push.png forward.png rewind.png stop.png eject.png play.png desktop-icon.png font-en.png font-fi.png font-fr.png game_score.png unbreakable_brick.png smoke.png sand.png player-2.png player-1.png player-0.png illegal.png game_border.png extra_life_anim.png exit_level.png enemy2.png enemy1.png treasure.png empty.png diamond.png brick_morpher.png brick_expanding.png brick.png boulder.png slime.png ameba.png intermission-en.png intermission-fi.png logo_tex.png map_cursor.png  font-en.txt.bz2 font-fi.txt.bz2 font-fr.txt.bz2 diamond-girl.png
EXTRA_DIST = buttons-16x16.xcf buttons-40x40.xcf desktop-icon.xcf font.xcf game_score.xcf glyphs-24x24.xcf intermission.xcf logo_texture.xcf map_cursor.xcf logo.png logo.3ds ameba.png.cnf brick_morpher.png.cnf diamond.png.cnf empty.png.cnf enemy1.png.cnf enemy2.png.cnf exit_level.png.cnf extra_life_anim.png.cnf player-0.png.cnf player-1.png.cnf player-2.png.cnf player-3.png.cnf slime.png.cnf smoke.png.cnf font-en.txt font-fi.txt font-fr.txt 
