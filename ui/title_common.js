{ "type":     "map",
  "x":        0,
  "y":        0,
  "width":    "100%",
  "height":   "100%",
  "children": [
    { "type":   "quest_menu",
      "id":     "quest_menu",
      "x":      1000,
      "y":       110,
      "navigation-down": [ "newgame" ]
    },
    { "type":   "title_help",
      "id":     "help",
      "x":      400,
      "y":       81,
      "width":  400,
      "height": 404
    },
    { "type":   "title_credits",
      "id":     "credits",
      "x":      "10%",
      "y":      582,
      "width":  "80%"
    },
    { "type":      "grid",
      "x":         275,
      "y":         485,
      "padding-y": 3,
      "children":  [
        { "type":     "row",
          "children": [
            { "type":       "button",
              "title":      gettext("Play"),
              "id":         "newgame",
              "width":      120,
              "on-release": "button_newgame"
            },
            { "type":            "button",
              "title":           gettext("<"),
              "id":              "cave_left",
              "x":               130,
              "width":           30,
              "on-release":      "button_cave_left",
              "navigation-left": [ "newgame" ]
            },
            { "type":            "button",
              "title":           gettext(">"),
              "id":              "cave_right",
              "x":               160,
              "width":           30,
              "on-release":      "button_cave_right",
              "navigation-left": [ "cave_left" ]
            },
            { "type":       "text",
              "id":         "cave",
              "x":          190,
              "width":      400
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":          "button",
              "title":         gettext("To quests"),
              "id":            "quests",
              "width":         120,
              "on-release":    "button_quests",
              "navigation-up": [ "newgame" ]
            },
            { "type":            "button",
              "title":           gettext("<"),
              "id":              "level_left",
              "x":               130,
              "width":           30,
              "on-release":      "button_level_left",
              "navigation-up":   [ "cave_left" ],
              "navigation-left": [ "quests" ]
            },
            { "type":            "button",
              "title":           gettext(">"),
              "id":              "level_right",
              "x":               160,
              "width":           30,
              "on-release":      "button_level_right",
              "navigation-up":   [ "cave_right" ],
              "navigation-left": [ "level_left" ]
            },
            { "type":       "text",
              "id":         "level",
              "x":          190,
              "width":      200
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":          "button",
              "title":         gettext("Settings"),
              "id":            "settings",
              "width":         120,
              "on-release":    "button_config",
              "navigation-up": [ "quests" ]
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":          "button",
              "title":         gettext("Quit"),
              "id":            "quit",
              "width":         120,
              "on-release":    "button_quit",
              "navigation-up": [ "settings" ]
            }
          ]
        }
      ]
    }
  ]
}
