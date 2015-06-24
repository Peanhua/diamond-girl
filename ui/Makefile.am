extradatadir = $(pkgdatadir)/ui

extradata_DATA = game_player_death.json.bz2 game_player_next_level.json.bz2 game_pyjama_party_ask_next.json.bz2 game_pyjama_party_level_completed.json.bz2 settings.json.bz2 title_classic.json.bz2 title_common.json.bz2 title_pyjama_party.json.bz2 

extradata_SOURCEFILES = game_player_death.json game_player_next_level.json game_pyjama_party_ask_next.json game_pyjama_party_level_completed.json settings.json title_classic.json title_common.json title_pyjama_party.json 

game_player_death.json.bz2 game_player_next_level.json.bz2 game_pyjama_party_ask_next.json.bz2 game_pyjama_party_level_completed.json.bz2 settings.json.bz2 title_classic.json.bz2 title_common.json.bz2 title_pyjama_party.json.bz2  : %.bz2 : %
	$(AM_V_GEN) bzip2 --stdout --compress < $< > $@
EXTRA_DIST = $(extradata_SOURCEFILES)

CLEANFILES = game_player_death.json.bz2 game_player_next_level.json.bz2 game_pyjama_party_ask_next.json.bz2 game_pyjama_party_level_completed.json.bz2 settings.json.bz2 title_classic.json.bz2 title_common.json.bz2 title_pyjama_party.json.bz2 

