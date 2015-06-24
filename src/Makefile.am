
if WITH_BUNDLED_MTWIST
   WBM_CFLAGS=-DWITH_BUNDLED_MTWIST
endif

if WITH_OPENGL

if WITH_LIB3DS_V1
LIB3DS_V1=-DLIB3DS_V1
endif

OGL_CFLAGS = \
	-DWITH_OPENGL         \
	-DGL_GLEXT_PROTOTYPES \
	$(gl_CFLAGS)          \
	$(glew_CFLAGS)        \
	$(glu_CFLAGS)         \
	$(LIB3DS_CFLAGS) $(LIB3DS_V1)

OGL_LIBS = \
	$(gl_LIBS)     \
	$(glew_LIBS)   \
	$(glu_LIBS)    \
	$(LIB3DS_LIBS)
endif

if WITH_NONOPENGL
NONOGL_CFLAGS = -DWITH_NONOPENGL
NONOGL_LIBS =
endif

if WITH_CHECKSUMMING
CHECKSUMMING_CFLAGS = -DWITH_CHECKSUMMING -DCHECKSUMMING_LIBRARY_MHASH=@CHECKSUMMING_LIBRARY_MHASH@ -DCHECKSUMMING_LIBRARY_CRYPTO=@CHECKSUMMING_LIBRARY_CRYPTO@
else
CHECKSUMMING_CFLAGS =
endif

if WITH_MIKMODFREE
MIKMODFREE_CFLAGS = -DWITH_MIKMODFREE
else
MIKMODFREE_CFLAGS =
endif

if WITH_WIN32
WIN32_LDADD = windows32.o
else
WIN32_LDADD =
endif

AM_CFLAGS = \
	-DDIAMOND_GIRL_DATADIR=\"$(datadir)\" \
	-DLOCALEDIR=\"$(localedir)\"          \
	$(DEBUGGING_CFLAGS)      \
	$(WCD_CFLAGS)            \
	$(WBM_CFLAGS)            \
	$(CHECKSUMMING_CFLAGS)   \
	$(OGL_CFLAGS)            \
	$(NONOGL_CFLAGS)         \
	$(CWARNFLAGS)            \
	$(libmikmod_CFLAGS)      \
	$(LIBMIKMOD_CFLAGS)      \
	$(MIKMODFREE_CFLAGS)     \
	$(libcrypto_CFLAGS)      \
	$(libpng_CFLAGS)         \
	$(lua_CFLAGS)            \
        $(json_CFLAGS)           \
	$(mtwist_CFLAGS)         \
	$(openal_CFLAGS)         \
	$(sdl_CFLAGS)            \
	$(sdl_gfx_CFLAGS)        \
	$(sdl_image_CFLAGS)      \
	$(sndfile_CFLAGS)        \
	$(vorbisfile_CFLAGS)

bin_PROGRAMS = diamond-girl


diamond_girl_SOURCES = ai_brains.c ai_diamond_hunter.c ai_free.c ai_get_next_target.c ai_get_possible_moves.c ai.h ai_is_passable.c ai_new.c ai_plan_level.c ai_playback.c ai_pyjama_party.c ai_set_engine_name.c backtrace.h bundled_mtwist.c cave_cleanup.c cave_displayname.c cave_filename.c cave_get.c cave.h cave_load.c cave_save.c check_installation.c clear_area.c diamond_girl.h draw_legend.c draw_map_borders.c draw_map.c draw_title_logo.c draw_title_starfield.c editor.c event.c event.h font.c font.h font_open.c free_cave_names.c game.c gamedata_free.c gamedata_new.c game.h game_move_girl.c game_player_death.c game_player_movement.c game_player_next_level.c game_pyjama_party_ask_next.c game_show_text.c gc.c gc.h get_backtrace.c get_cave_level_count.c get_cave_names.c get_data_filename.c get_random_name.c get_save_filename.c gfx_2d.c gfx2d_draw_borders.c gfx2d_draw_image.c gfx_3d.c gfx_bind_texture.c gfxbuf.c gfxbuf.h gfx.c gfx_capture_screen.c gfx_colour.c gfx_draw_hline.c gfx_draw_rectangle.c gfx_draw_vline.c gfx_flip.c gfx_gl_log.c gfx_glyph.c gfx_glyph.h gfx.h gfx_image.c gfx_image.h girl.c girl_free.c girl.h girl_load.c girl_save.c globals.h highscore.c highscore.h image_blit.c image_duplicate.c image_expand.c image_free.c image.h image_load.c image_load_from_surface.c image_new.c image_pack.c image_save.c image_to_greyscale.c image_to_sdl.c image_to_texture.c intro.c main.c map_animations_tick.c map_calculate_hash.c map_copy.c map_find_path.c map_free.c map_free_path.c map_get.c map.h map_level_start_animation_start.c map_level_start_animation_stop.c map_level_start_animation_tick.c map_load.c map_load_state.c map_new.c map_physics_tick.c map_random.c map_save.c map_save_state.c map_scroll_to_girl.c maptarget_free.c map_thumbnail_image.c map_well.c parse_commandline_arguments.c playback_append_step.c playback_check.c playback_copy.c playback_free.c playback_get.c playback.h playback_load.c playback_new.c playback_next_step.c playback_save.c playback_step_initialize.c pyjama_party.c random.c random.h read_file.c read_localized_text_file.c read_text_file.c round_to_power_of_two.c set_cave_selection.c set_language.c settings.c settings_default.c settings_read.c settings_read_v1.c settings_read_v2.c settings_write.c setup_user_homedir.c sfx.c sfx.h sfx_load.c sfx_load_file.c sfx_load_ogg.c sfx_load_xm.c sfx_synth.c sfx_synth.h sfx_synth_load.c sfx_synth_to_buffer.c stack.c stack.h td_object_draw.c td_object.h td_object_load.c td_object_unload.c texture.h texture_load.c texture_setup_from_image.c themes.c themes.h title.c title_game.c title.h title_help_tick.c title_load_playback.c title_map_girl_movement.c title_map_tick.c trait_get_name.c traits.c traits.h traits_level_gives.c twinkle.c twinkle.h ui_bindings.c ui_bindings_default.c ui.c ui_command_name.c ui_draw.c ui_get_widget_at_coordinates.c ui.h ui_handlers.c ui_keyboard_key_name.c ui_keyboard_modifier_name.c ui_last_user_action.c ui_process_events.c ui_process_mouse_button.c ui_process_mouse_move.c ui_set_common_handlers.c ui_set_navigation_handlers.c widget_center.c widget_center_on_parent.c widget_draw.c widget_dump.c widget_enabled.c widget_factory.c widget_factory.h widget_find.c widget_flags.c widget_get_navigation_down.c widget_get_navigation_left.c widget_get_navigation_right.c widget_get_navigation_up.c widget.h widget_new_button.c widget_new.c widget_new_checkbox.c widget_new_child.c widget_new_frame.c widget_new_game_help_window.c widget_new_game_window.c widget_new_grid.c widget_new_grid_row.c widget_new_highscore_einfo.c widget_new_legend.c widget_new_list.c widget_new_map.c widget_new_message_window.c widget_new_osd_volume.c widget_new_playback_controls.c widget_new_select.c widget_new_select_option.c widget_new_slider.c widget_new_spacer.c widget_new_text.c widget_new_theme_info_button.c widget_new_theme_info_window.c widget_new_title_credits.c widget_new_title_help.c widget_new_title_highscores.c widget_new_title_partystatus.c widget_new_tooltip.c widget_new_trait_button.c widget_new_trait_key_button.c widget_new_traits_available_window.c widget_new_window.c widget_resize_to_fit_children.c widget_set_enabled.c widget_set_navigation_down.c widget_set_navigation_left.c widget_set_navigation_leftright.c widget_set_navigation_right.c widget_set_navigation_up.c widget_set_navigation_updown.c widget_set_no_focus.c widget_set_select_options.c widget_set_tooltip.c widget_user_data.c write_file.c 


diamond_girl_LDADD =       \
	$(OGL_LIBS)        \
	$(NONOGL_LIBS)     \
	$(libmikmod_LIBS)  \
	$(LIBMIKMOD_LIBS) $(LIBMIKMOD_LDADD) \
	$(libcrypto_LIBS)  \
	$(libpng_LIBS)     \
	$(lua_LIBS)        \
	$(json_LIBS)       \
	$(mtwist_LIBS)     \
	$(openal_LIBS)     \
	$(sdl_LIBS)        \
	$(sdl_gfx_LIBS)    \
	$(sdl_image_LIBS)  \
	$(sndfile_LIBS)    \
	$(vorbisfile_LIBS) \
	$(WIN32_LDADD)     \
	@LIBINTL@


if WITH_WIN32
diamond-girl.ico: ../themes/default/gfx/desktop-icon.png
	$(AM_V_GEN) $(ICOTOOL) --create --output=$@ $<

windows32.o: windows32.rc diamond-girl.ico
	$(AM_V_GEN) $(WINDRES) --input=$< --output=$@ --output-format=coff
endif

CLEANFILES = diamond-girl.ico


# Add clang static analyzer support.
# Shamelessly stolen from Alex Rønne Petersen: http://blog.alexrp.com/2013/09/26/clangs-static-analyzer-and-automake/

analyze_srcs = ai_brains.c ai_diamond_hunter.c ai_free.c ai_get_next_target.c ai_get_possible_moves.c ai_is_passable.c ai_new.c ai_plan_level.c ai_playback.c ai_pyjama_party.c ai_set_engine_name.c bundled_mtwist.c cave_cleanup.c cave_displayname.c cave_filename.c cave_get.c cave_load.c cave_save.c check_installation.c clear_area.c draw_legend.c draw_map_borders.c draw_map.c draw_title_logo.c draw_title_starfield.c editor.c event.c font.c font_open.c free_cave_names.c game.c gamedata_free.c gamedata_new.c game_move_girl.c game_player_death.c game_player_movement.c game_player_next_level.c game_pyjama_party_ask_next.c game_show_text.c gc.c get_backtrace.c get_cave_level_count.c get_cave_names.c get_data_filename.c get_random_name.c get_save_filename.c gfx_2d.c gfx2d_draw_borders.c gfx2d_draw_image.c gfx_3d.c gfx_bind_texture.c gfxbuf.c gfx.c gfx_capture_screen.c gfx_colour.c gfx_draw_hline.c gfx_draw_rectangle.c gfx_draw_vline.c gfx_flip.c gfx_gl_log.c gfx_glyph.c gfx_image.c girl.c girl_free.c girl_load.c girl_save.c highscore.c image_blit.c image_duplicate.c image_expand.c image_free.c image_load.c image_load_from_surface.c image_new.c image_pack.c image_save.c image_to_greyscale.c image_to_sdl.c image_to_texture.c intro.c main.c map_animations_tick.c map_calculate_hash.c map_copy.c map_find_path.c map_free.c map_free_path.c map_get.c map_level_start_animation_start.c map_level_start_animation_stop.c map_level_start_animation_tick.c map_load.c map_load_state.c map_new.c map_physics_tick.c map_random.c map_save.c map_save_state.c map_scroll_to_girl.c maptarget_free.c map_thumbnail_image.c map_well.c parse_commandline_arguments.c playback_append_step.c playback_check.c playback_copy.c playback_free.c playback_get.c playback_load.c playback_new.c playback_next_step.c playback_save.c playback_step_initialize.c pyjama_party.c random.c read_file.c read_localized_text_file.c read_text_file.c round_to_power_of_two.c set_cave_selection.c set_language.c settings.c settings_default.c settings_read.c settings_read_v1.c settings_read_v2.c settings_write.c setup_user_homedir.c sfx.c sfx_load.c sfx_load_file.c sfx_load_ogg.c sfx_load_xm.c sfx_synth.c sfx_synth_load.c sfx_synth_to_buffer.c stack.c td_object_draw.c td_object_load.c td_object_unload.c texture_load.c texture_setup_from_image.c themes.c title.c title_game.c title_help_tick.c title_load_playback.c title_map_girl_movement.c title_map_tick.c trait_get_name.c traits.c traits_level_gives.c twinkle.c ui_bindings.c ui_bindings_default.c ui.c ui_command_name.c ui_draw.c ui_get_widget_at_coordinates.c ui_handlers.c ui_keyboard_key_name.c ui_keyboard_modifier_name.c ui_last_user_action.c ui_process_events.c ui_process_mouse_button.c ui_process_mouse_move.c ui_set_common_handlers.c ui_set_navigation_handlers.c widget_center.c widget_center_on_parent.c widget_draw.c widget_dump.c widget_enabled.c widget_factory.c widget_find.c widget_flags.c widget_get_navigation_down.c widget_get_navigation_left.c widget_get_navigation_right.c widget_get_navigation_up.c widget_new_button.c widget_new.c widget_new_checkbox.c widget_new_child.c widget_new_frame.c widget_new_game_help_window.c widget_new_game_window.c widget_new_grid.c widget_new_grid_row.c widget_new_highscore_einfo.c widget_new_legend.c widget_new_list.c widget_new_map.c widget_new_message_window.c widget_new_osd_volume.c widget_new_playback_controls.c widget_new_select.c widget_new_select_option.c widget_new_slider.c widget_new_spacer.c widget_new_text.c widget_new_theme_info_button.c widget_new_theme_info_window.c widget_new_title_credits.c widget_new_title_help.c widget_new_title_highscores.c widget_new_title_partystatus.c widget_new_tooltip.c widget_new_trait_button.c widget_new_trait_key_button.c widget_new_traits_available_window.c widget_new_window.c widget_resize_to_fit_children.c widget_set_enabled.c widget_set_navigation_down.c widget_set_navigation_left.c widget_set_navigation_leftright.c widget_set_navigation_right.c widget_set_navigation_up.c widget_set_navigation_updown.c widget_set_no_focus.c widget_set_select_options.c widget_set_tooltip.c widget_user_data.c write_file.c 
analyze_plists = ai_brains.plist ai_diamond_hunter.plist ai_free.plist ai_get_next_target.plist ai_get_possible_moves.plist ai_is_passable.plist ai_new.plist ai_plan_level.plist ai_playback.plist ai_pyjama_party.plist ai_set_engine_name.plist bundled_mtwist.plist cave_cleanup.plist cave_displayname.plist cave_filename.plist cave_get.plist cave_load.plist cave_save.plist check_installation.plist clear_area.plist draw_legend.plist draw_map_borders.plist draw_map.plist draw_title_logo.plist draw_title_starfield.plist editor.plist event.plist font.plist font_open.plist free_cave_names.plist game.plist gamedata_free.plist gamedata_new.plist game_move_girl.plist game_player_death.plist game_player_movement.plist game_player_next_level.plist game_pyjama_party_ask_next.plist game_show_text.plist gc.plist get_backtrace.plist get_cave_level_count.plist get_cave_names.plist get_data_filename.plist get_random_name.plist get_save_filename.plist gfx_2d.plist gfx2d_draw_borders.plist gfx2d_draw_image.plist gfx_3d.plist gfx_bind_texture.plist gfxbuf.plist gfx.plist gfx_capture_screen.plist gfx_colour.plist gfx_draw_hline.plist gfx_draw_rectangle.plist gfx_draw_vline.plist gfx_flip.plist gfx_gl_log.plist gfx_glyph.plist gfx_image.plist girl.plist girl_free.plist girl_load.plist girl_save.plist highscore.plist image_blit.plist image_duplicate.plist image_expand.plist image_free.plist image_load.plist image_load_from_surface.plist image_new.plist image_pack.plist image_save.plist image_to_greyscale.plist image_to_sdl.plist image_to_texture.plist intro.plist main.plist map_animations_tick.plist map_calculate_hash.plist map_copy.plist map_find_path.plist map_free.plist map_free_path.plist map_get.plist map_level_start_animation_start.plist map_level_start_animation_stop.plist map_level_start_animation_tick.plist map_load.plist map_load_state.plist map_new.plist map_physics_tick.plist map_random.plist map_save.plist map_save_state.plist map_scroll_to_girl.plist maptarget_free.plist map_thumbnail_image.plist map_well.plist parse_commandline_arguments.plist playback_append_step.plist playback_check.plist playback_copy.plist playback_free.plist playback_get.plist playback_load.plist playback_new.plist playback_next_step.plist playback_save.plist playback_step_initialize.plist pyjama_party.plist random.plist read_file.plist read_localized_text_file.plist read_text_file.plist round_to_power_of_two.plist set_cave_selection.plist set_language.plist settings.plist settings_default.plist settings_read.plist settings_read_v1.plist settings_read_v2.plist settings_write.plist setup_user_homedir.plist sfx.plist sfx_load.plist sfx_load_file.plist sfx_load_ogg.plist sfx_load_xm.plist sfx_synth.plist sfx_synth_load.plist sfx_synth_to_buffer.plist stack.plist td_object_draw.plist td_object_load.plist td_object_unload.plist texture_load.plist texture_setup_from_image.plist themes.plist title.plist title_game.plist title_help_tick.plist title_load_playback.plist title_map_girl_movement.plist title_map_tick.plist trait_get_name.plist traits.plist traits_level_gives.plist twinkle.plist ui_bindings.plist ui_bindings_default.plist ui.plist ui_command_name.plist ui_draw.plist ui_get_widget_at_coordinates.plist ui_handlers.plist ui_keyboard_key_name.plist ui_keyboard_modifier_name.plist ui_last_user_action.plist ui_process_events.plist ui_process_mouse_button.plist ui_process_mouse_move.plist ui_set_common_handlers.plist ui_set_navigation_handlers.plist widget_center.plist widget_center_on_parent.plist widget_draw.plist widget_dump.plist widget_enabled.plist widget_factory.plist widget_find.plist widget_flags.plist widget_get_navigation_down.plist widget_get_navigation_left.plist widget_get_navigation_right.plist widget_get_navigation_up.plist widget_new_button.plist widget_new.plist widget_new_checkbox.plist widget_new_child.plist widget_new_frame.plist widget_new_game_help_window.plist widget_new_game_window.plist widget_new_grid.plist widget_new_grid_row.plist widget_new_highscore_einfo.plist widget_new_legend.plist widget_new_list.plist widget_new_map.plist widget_new_message_window.plist widget_new_osd_volume.plist widget_new_playback_controls.plist widget_new_select.plist widget_new_select_option.plist widget_new_slider.plist widget_new_spacer.plist widget_new_text.plist widget_new_theme_info_button.plist widget_new_theme_info_window.plist widget_new_title_credits.plist widget_new_title_help.plist widget_new_title_highscores.plist widget_new_title_partystatus.plist widget_new_tooltip.plist widget_new_trait_button.plist widget_new_trait_key_button.plist widget_new_traits_available_window.plist widget_new_window.plist widget_resize_to_fit_children.plist widget_set_enabled.plist widget_set_navigation_down.plist widget_set_navigation_left.plist widget_set_navigation_leftright.plist widget_set_navigation_right.plist widget_set_navigation_up.plist widget_set_navigation_updown.plist widget_set_no_focus.plist widget_set_select_options.plist widget_set_tooltip.plist widget_user_data.plist write_file.plist 


MOSTLYCLEANFILES = $(analyze_plists)

$(analyze_plists): %.plist: %.c
	@echo "  CCSA  " $@
	@$(COMPILE) --analyze $< -o $@

analyze: $(analyze_plists)

.PHONY: analyze

# End of clang static analyzer support.

