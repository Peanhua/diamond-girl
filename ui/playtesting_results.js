{ "type":       "frame",
  "width":      300,
  "height":     140,
  "no-shadow":  true,
  "no-borders": true,
  "children":   [
    { "type":       "grid",
      "children":   [
        { "type":     "row",
          "children": [
            { "type":  "text",
              "title": gettext("Score:")
            },
            { "type":   "text",
              "x":      200,
              "title":  "${score}"
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":  "text",
              "title": gettext("Diamonds collected")
            },
            { "type":   "text",
              "x":      200,
              "title":  "${diamonds_collected}"
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":  "text",
              "title": gettext("Diamonds left on map")
            },
            { "type":   "text",
              "x":      200,
              "title":  "${diamonds_left}"
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":  "text",
              "title": gettext("Enemy1 left on map")
            },
            { "type":   "text",
              "x":      200,
              "title":  "${enemy1_left}"
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":  "text",
              "title": gettext("Enemy2 left on map")
            },
            { "type":   "text",
              "x":      200,
              "title":  "${enemy2_left}"
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":  "text",
              "title": gettext("Time left")
            },
            { "type":   "text",
              "x":      200,
              "title":  "${time_left}"
            }
          ]
        }
      ]
    }
  ]
}
