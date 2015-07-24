{ "type":     "window",
  "width":    600,
  "height":   320,
  "title":    gettext("SETTINGS"),
  "modal":    true,
  "children": [
    { "type":      "grid",
      "x":         10,
      "y":         40,
      "padding-y":  2,
      "children": [
        { "type":     "row",
          "children": [
            { "type":  "select",
              "id":    "controllers",
              "x":     0,
              "width": 230
            },
            { "type":  "text",
              "x":     290,
              "title": gettext("Options:")
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":  "text",
              "x":     0,
              "title": gettext("Up")
            },
            { "type":  "text",
              "x":     300,
              "title": gettext("Sound effects")
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":  "text",
              "x":     0,
              "title": gettext("Left")
            },
            { "type":  "text",
              "x":     300,
              "title": gettext("Music")
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":  "text",
              "x":     0,
              "title": gettext("Right")
            },
            { "type":  "text",
              "x":     300,
              "title": gettext("Volume")
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":  "text",
              "x":     0,
              "title": gettext("Down")
            },
            { "type":  "text",
              "x":     300,
              "title": gettext("OpenGL")
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":  "text",
              "x":     0,
              "title": gettext("Manipulate")
            },
            { "type":  "text",
              "x":     300,
              "title": gettext("Full screen")
            }
          ]
        },
        { "type":     "row",
          "children": [
            { "type":  "text",
              "x":     0,
              "title": gettext("Alt.manip")
            },
            { "type":  "text",
              "x":     300,
              "title": gettext("Special days")
            }
          ]
        }
      ]
    },
    { "type":       "button",
      "title":      gettext("  Close  "),
      "id":         "close",
      "y":          268,
      "on-release": "on_window_close_clicked"
    }
  ]
}
