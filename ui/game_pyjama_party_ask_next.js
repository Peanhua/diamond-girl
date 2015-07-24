{ "type":      "frame",
  "width":     320,
  "height":    170,
  "modal":     true,
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
                      "title": gettext("Manually control the next girl?")
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
                      "title":      gettext("Yes"),
                      "id":         "gppan_yes",
                      "x":          100,
                      "width":      100,
                      "focus":      true,
                      "on-release": "on_button_clicked",
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":          "button",
                      "title":         gettext("No"),
                      "id":            "gppan_no",
                      "x":             100,
                      "width":         100,
                      "on-release":    "on_button_clicked",
                      "navigation-up": [ "gppan_yes" ]
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":          "button",
                      "title":         gettext("Party run"),
                      "id":            "gppan_partyrun",
                      "x":             100,
                      "width":         100,
                      "on-release":    "on_button_clicked",
                      "navigation-up": [ "gppan_no" ]
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":   "spacer",
                      "height": 5
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":          "button",
                      "title":         gettext("Save"),
                      "id":            "gppan_save",
                      "x":             100,
                      "width":         100,
                      "on-release":    "on_save_clicked",
                      "navigation-up": [ "gppan_partyrun" ]
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":          "button",
                      "title":         gettext("Quit"),
                      "id":            "gppan_quit",
                      "x":             100,
                      "width":         100,
                      "on-release":    "on_quit_clicked",
                      "navigation-up": [ "gppan_save" ]
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
