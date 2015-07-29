{ "type":      "frame",
  "width":     200,
  "height":    160,
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
                      "title": gettext("Level completed.")
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":  "text",
                      "title": gettext("Diamond score:")
                    },
                    { "type":  "text",
                      "id":    "gpnl_diamond_score",
                      "x":     135,
                      "title": "${diamond_score}"
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":  "text",
                      "id":    "gpnl_other"
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
                      "id":         "gpnl_play",
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
                      "id":            "gpnl_save",
                      "x":             50,
                      "width":         120,
                      "on-release":    "on_save_clicked",
                      "navigation-up": [ "gpnl_play" ]
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":          "button",
                      "title":         gettext("Quit"),
                      "id":            "gpnl_quit",
                      "x":             50,
                      "width":         120,
                      "on-release":    "on_quit_clicked",
                      "navigation-up": [ "gpnl_save" ]
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
