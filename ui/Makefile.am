extradatadir = $(pkgdatadir)/ui

extradata_DATA = game_player_death.json.bz2 game_player_next_level.json.bz2 game_pyjama_party_ask_next.json.bz2 game_pyjama_party_level_completed.json.bz2 playtesting_results.json.bz2 settings.json.bz2 title_classic.json.bz2 title_common.json.bz2 title_pyjama_party.json.bz2 

extradata_SOURCEFILES = game_player_death.js game_player_next_level.js game_pyjama_party_ask_next.js game_pyjama_party_level_completed.js playtesting_results.js settings.js title_classic.js title_common.js title_pyjama_party.js 

game_player_death.json game_player_next_level.json game_pyjama_party_ask_next.json game_pyjama_party_level_completed.json playtesting_results.json settings.json title_classic.json title_common.json title_pyjama_party.json  : %.json : %.js
	$(AM_V_GEN) sed 's/gettext(\(.*\))/\1/g' $< > $@

game_player_death.json.bz2 game_player_next_level.json.bz2 game_pyjama_party_ask_next.json.bz2 game_pyjama_party_level_completed.json.bz2 playtesting_results.json.bz2 settings.json.bz2 title_classic.json.bz2 title_common.json.bz2 title_pyjama_party.json.bz2  : %.bz2 : %
	$(AM_V_GEN) bzip2 --stdout --compress < $< > $@

EXTRA_DIST = $(extradata_SOURCEFILES)

CLEANFILES = game_player_death.json game_player_next_level.json game_pyjama_party_ask_next.json game_pyjama_party_level_completed.json playtesting_results.json settings.json title_classic.json title_common.json title_pyjama_party.json  game_player_death.json.bz2 game_player_next_level.json.bz2 game_pyjama_party_ask_next.json.bz2 game_pyjama_party_level_completed.json.bz2 playtesting_results.json.bz2 settings.json.bz2 title_classic.json.bz2 title_common.json.bz2 title_pyjama_party.json.bz2 

