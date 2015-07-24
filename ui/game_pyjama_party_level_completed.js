{ "type":      "frame",
  "width":     320,
  "height":    140,
  "modal":     true,
  //"on-unload": "on_window_unload",
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
                    { "type":     "text",
                      "title":    gettext("The party has passed this level."),
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
                      "id":         "gpplc_play",
                      "x":          100,
                      "width":      100,
                      "focus":      true,
                      "on-release": "on_play_clicked"
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":          "button",
                      "title":         gettext("Save"),
                      "id":            "gpplc_save",
                      "x":             100,
                      "width":         100,
                      "on-release":    "on_save_clicked",
                      "navigation-up": [ "gpplc_play" ]
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":          "button",
                      "title":         gettext("Quit"),
                      "id":            "gpplc_quit",
                      "x":             100,
                      "width":         100,
                      "on-release":    "on_quit_clicked",
                      "navigation-up": [ "gpplc_save" ]
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
