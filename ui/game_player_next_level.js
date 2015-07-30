{ "type":      "frame",
  "width":     440,
  "height":    320,
  "modal":     true,
  "on-unload": "on_window_unload",
  "children":  [
    { "type":     "grid",
      "x":        10,
      "y":        10,
      "children": [
        
        { "type":     "row",
          "children": [
            { "type":     "grid",
              "children": [
                { "type":     "row",
                  "children": [
                    { "type":    "text",
                      "title":   gettext("Level completed!"),
                      "width":   430,
                      "h-align": true
                    }
                  ]
                }
              ]
            }
          ]
        },

        { "type":     "row",
          "children": [
            { "type":     "grid",
              "children": [
                { "type":     "row",
                  "children": [
                    { "type": "text"
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":  "text",
                      "title": gettext("Score:")
                    }
                  ]
                },
                { "type":     "row",
                  "id":       "score_diamonds",
                  "hidden":   true,
                  "children": [
                    { "type":  "text",
                      "x":     10,
                      "title": gettext("diamonds")
                    },
                    { "type":   "text",
                      "x":      125,
                      "title":  "${score_diamonds}"
                    }
                  ]
                },
                { "type":     "row",
                  "id":       "score_time",
                  "hidden":   true,
                  "children": [
                    { "type":  "text",
                      "x":     10,
                      "title": gettext("time left")
                    },
                    { "type":   "text",
                      "x":      125,
                      "title":  "${score_time}"
                    }
                  ]
                },
                { "type":     "row",
                  "id":       "score_greedy",
                  "hidden":   true,
                  "children": [
                    { "type":  "text",
                      "x":     10,
                      "title": gettext("greedy")
                    },
                    { "type":   "text",
                      "x":      125,
                      "title":  "${score_greedy}"
                    }
                  ]
                },
                { "type":     "row",
                  "id":       "score_irongirl",
                  "hidden":   true,
                  "children": [
                    { "type":  "text",
                      "x":     10,
                      "title": gettext("iron girl")
                    },
                    { "type":   "text",
                      "x":      125,
                      "title":  "${irongirl}"
                    }
                  ]
                },
                { "type":     "row",
                  "id":       "score_total",
                  "hidden":   true,
                  "children": [
                    { "type":  "text",
                      "x":     10,
                      "title": gettext("total")
                    },
                    { "type":   "text",
                      "x":      125,
                      "title":  "${score_total}"
                    }
                  ]
                }
              ]
            },
            
            { "type":     "grid",
              "x":        230,
              "children": [
                { "type":     "row",
                  "children": [
                    { "type": "text"
                    }
                  ]
                },
                { "type":     "row",
                  "children": [
                    { "type":  "text",
                      "title": gettext("Diamond score:")
                    }
                  ]
                },
                { "type":     "row",
                  "id":       "diamond_score_alive",
                  "hidden":   true,
                  "children": [
                    { "type":  "text",
                      "x":     10,
                      "title": gettext("alive")
                    },
                    { "type":   "text",
                      "x":      125,
                      "title":  "${diamond_score_alive}"
                    }
                  ]
                },
                { "type":     "row",
                  "id":       "diamond_score_diamonds",
                  "hidden":   true,
                  "children": [
                    { "type":  "text",
                      "x":     10,
                      "title": gettext("diamonds")
                    },
                    { "type":   "text",
                      "x":      125,
                      "title":  "${diamond_score_diamonds}"
                    }
                  ]
                },
                { "type":     "row",
                  "id":       "diamond_score_greedy",
                  "hidden":   true,
                  "children": [
                    { "type":  "text",
                      "x":     10,
                      "title": gettext("greedy")
                    },
                    { "type":   "text",
                      "x":      125,
                      "title":  "${diamond_score_greedy}"
                    }
                  ]
                },
                { "type":     "row",
                  "id":       "diamond_score_irongirl",
                  "hidden":   true,
                  "children": [
                    { "type":  "text",
                      "x":     10,
                      "title": gettext("iron girl")
                    },
                    { "type":   "text",
                      "x":      125,
                      "title":  "${irongirl}"
                    }
                  ]
                },
                { "type":     "row",
                  "id":       "diamond_score",
                  "hidden":   true,
                  "children": [
                    { "type":  "text",
                      "x":     10,
                      "title": gettext("total")
                    },
                    { "type":   "text",
                      "x":      125,
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
            { "type": "text"
            }
          ]
        },

        { "type":     "row",
          "children": [
            { "type":    "text",
              "id":      "other",
              "hidden":  true,
              "width":   430,
              "h-align": true
            }
          ]
        },
            
            
        { "type":     "row",
          "children": [
            { "type": "text"
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
                      "x":          150,
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
                      "x":             150,
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
                      "x":             150,
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
