{ "type":      "frame",
  "width":     200,
  "height":    140,
  "modal":     true,
  "on-unload": "on_window_unload",
  "children":  [
    { "type":     "grid",
      "children": [
        { "type":     "row",
          "children": [
            { "type":     "grid",
              "x":        10,
              "y":        10,
              "children": [
                { "type":     "row",
                  "children": [
                    { "type":  "text",
                      "x":     30,
                      "title": gettext("You are dead.")
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":  "text",
                      "title": gettext("Diamond score:")
                    },
                    { "type":   "text",
                      "id":     "diamond_score",
                      "hidden": true,
                      "x":      135,
                      "title":  "${diamond_score}"
                    }
                  ]
                }
              ]
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":      "grid",
              "y":         10,
              "padding-y": 2,
              "children":  [
                { "type":     "row",
                  "children": [
                    { "type":       "button",
                      "title":      gettext("Continue"),
                      "id":         "game_player_death_play",
                      "x":          50,
                      "width":      120,
                      "focus":      true,
                      "on-release": "on_play_clicked"
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":          "button",
                      "title":         gettext("Save"),
                      "id":            "game_player_death_save",
                      "x":             50,
                      "width":         120,
                      "on-release":    "on_save_clicked",
                      "navigation-up": [ "game_player_death_play" ]
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":          "button",
                      "title":         gettext("Quit"),
                      "id":            "game_player_death_quit",
                      "x":             50,
                      "width":         120,
                      "on-release":    "on_quit_clicked",
                      "navigation-up": [ "game_player_death_save" ]
                    }
                  ]
                }
              ]
            }
          ]
        }
      ]
    }
  ]
}
