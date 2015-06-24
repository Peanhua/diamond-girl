/*
  Diamond Girl - Game where player collects diamonds.
  Copyright (C) 2005-2015  Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


  Complete license can be found in the LICENSE file.
*/

#include "ui.h"

#include <string.h>

char * ui_keyboard_key_name(SDLKey key)
{
  static char name[256];

  switch(key)
    {
    case SDLK_BACKSPACE:    strcpy(name, "BACKSPACE");    break;
    case SDLK_TAB:          strcpy(name, "TAB");          break;
    case SDLK_CLEAR:        strcpy(name, "CLEAR");        break;
    case SDLK_RETURN:       strcpy(name, "RETURN");       break;
    case SDLK_PAUSE:        strcpy(name, "PAUSE");        break;
    case SDLK_ESCAPE:       strcpy(name, "ESCAPE");       break;
    case SDLK_SPACE:        strcpy(name, "SPACE");        break;
    case SDLK_EXCLAIM:      strcpy(name, "EXCLAIM");      break;
    case SDLK_QUOTEDBL:     strcpy(name, "QUOTEDBL");     break;
    case SDLK_HASH:         strcpy(name, "HASH");         break;
    case SDLK_DOLLAR:       strcpy(name, "DOLLAR");       break;
    case SDLK_AMPERSAND:    strcpy(name, "AMPERSAND");    break;
    case SDLK_QUOTE:        strcpy(name, "QUOTE");        break;
    case SDLK_LEFTPAREN:    strcpy(name, "LEFTPAREN");    break;
    case SDLK_RIGHTPAREN:   strcpy(name, "RIGHTPAREN");   break;
    case SDLK_ASTERISK:     strcpy(name, "ASTERISK");     break;
    case SDLK_PLUS:         strcpy(name, "PLUS");         break;
    case SDLK_COMMA:        strcpy(name, "COMMA");        break;
    case SDLK_MINUS:        strcpy(name, "MINUS");        break;
    case SDLK_PERIOD:       strcpy(name, "PERIOD");       break;
    case SDLK_SLASH:        strcpy(name, "SLASH");        break;
    case SDLK_0:            strcpy(name, "0");            break;
    case SDLK_1:            strcpy(name, "1");            break;
    case SDLK_2:            strcpy(name, "2");            break;
    case SDLK_3:            strcpy(name, "3");            break;
    case SDLK_4:            strcpy(name, "4");            break;
    case SDLK_5:            strcpy(name, "5");            break;
    case SDLK_6:            strcpy(name, "6");            break;
    case SDLK_7:            strcpy(name, "7");            break;
    case SDLK_8:            strcpy(name, "8");            break;
    case SDLK_9:            strcpy(name, "9");            break;
    case SDLK_COLON:        strcpy(name, "COLON");        break;
    case SDLK_SEMICOLON:    strcpy(name, "SEMICOLON");    break;
    case SDLK_LESS:         strcpy(name, "LESS");         break;
    case SDLK_EQUALS:       strcpy(name, "EQUALS");       break;
    case SDLK_GREATER:      strcpy(name, "GREATER");      break;
    case SDLK_QUESTION:     strcpy(name, "QUESTION");     break;
    case SDLK_AT:           strcpy(name, "AT");           break;
    case SDLK_LEFTBRACKET:  strcpy(name, "LEFTBRACKET");  break;
    case SDLK_BACKSLASH:    strcpy(name, "BACKSLASH");    break;
    case SDLK_RIGHTBRACKET: strcpy(name, "RIGHTBRACKET"); break;
    case SDLK_CARET:        strcpy(name, "CARET");        break;
    case SDLK_UNDERSCORE:   strcpy(name, "UNDERSCORE");   break;
    case SDLK_BACKQUOTE:    strcpy(name, "BACKQUOTE");    break;
    case SDLK_a:            strcpy(name, "a");            break;
    case SDLK_b:            strcpy(name, "b");            break;
    case SDLK_c:            strcpy(name, "c");            break;
    case SDLK_d:            strcpy(name, "d");            break;
    case SDLK_e:            strcpy(name, "e");            break;
    case SDLK_f:            strcpy(name, "f");            break;
    case SDLK_g:            strcpy(name, "g");            break;
    case SDLK_h:            strcpy(name, "h");            break;
    case SDLK_i:            strcpy(name, "i");            break;
    case SDLK_j:            strcpy(name, "j");            break;
    case SDLK_k:            strcpy(name, "k");            break;
    case SDLK_l:            strcpy(name, "l");            break;
    case SDLK_m:            strcpy(name, "m");            break;
    case SDLK_n:            strcpy(name, "n");            break;
    case SDLK_o:            strcpy(name, "o");            break;
    case SDLK_p:            strcpy(name, "p");            break;
    case SDLK_q:            strcpy(name, "q");            break;
    case SDLK_r:            strcpy(name, "r");            break;
    case SDLK_s:            strcpy(name, "s");            break;
    case SDLK_t:            strcpy(name, "t");            break;
    case SDLK_u:            strcpy(name, "u");            break;
    case SDLK_v:            strcpy(name, "v");            break;
    case SDLK_w:            strcpy(name, "w");            break;
    case SDLK_x:            strcpy(name, "x");            break;
    case SDLK_y:            strcpy(name, "y");            break;
    case SDLK_z:            strcpy(name, "z");            break;
    case SDLK_DELETE:       strcpy(name, "DELETE");       break;
    case SDLK_WORLD_0:      strcpy(name, "WORLD_0");      break;
    case SDLK_WORLD_1:      strcpy(name, "WORLD_1");      break;
    case SDLK_WORLD_2:      strcpy(name, "WORLD_2");      break;
    case SDLK_WORLD_3:      strcpy(name, "WORLD_3");      break;
    case SDLK_WORLD_4:      strcpy(name, "WORLD_4");      break;
    case SDLK_WORLD_5:      strcpy(name, "WORLD_5");      break;
    case SDLK_WORLD_6:      strcpy(name, "WORLD_6");      break;
    case SDLK_WORLD_7:      strcpy(name, "WORLD_7");      break;
    case SDLK_WORLD_8:      strcpy(name, "WORLD_8");      break;
    case SDLK_WORLD_9:      strcpy(name, "WORLD_9");      break;
    case SDLK_WORLD_10:     strcpy(name, "WORLD_10");     break;
    case SDLK_WORLD_11:     strcpy(name, "WORLD_11");     break;
    case SDLK_WORLD_12:     strcpy(name, "WORLD_12");     break;
    case SDLK_WORLD_13:     strcpy(name, "WORLD_13");     break;
    case SDLK_WORLD_14:     strcpy(name, "WORLD_14");     break;
    case SDLK_WORLD_15:     strcpy(name, "WORLD_15");     break;
    case SDLK_WORLD_16:     strcpy(name, "WORLD_16");     break;
    case SDLK_WORLD_17:     strcpy(name, "WORLD_17");     break;
    case SDLK_WORLD_18:     strcpy(name, "WORLD_18");     break;
    case SDLK_WORLD_19:     strcpy(name, "WORLD_19");     break;
    case SDLK_WORLD_20:     strcpy(name, "WORLD_20");     break;
    case SDLK_WORLD_21:     strcpy(name, "WORLD_21");     break;
    case SDLK_WORLD_22:     strcpy(name, "WORLD_22");     break;
    case SDLK_WORLD_23:     strcpy(name, "WORLD_23");     break;
    case SDLK_WORLD_24:     strcpy(name, "WORLD_24");     break;
    case SDLK_WORLD_25:     strcpy(name, "WORLD_25");     break;
    case SDLK_WORLD_26:     strcpy(name, "WORLD_26");     break;
    case SDLK_WORLD_27:     strcpy(name, "WORLD_27");     break;
    case SDLK_WORLD_28:     strcpy(name, "WORLD_28");     break;
    case SDLK_WORLD_29:     strcpy(name, "WORLD_29");     break;
    case SDLK_WORLD_30:     strcpy(name, "WORLD_30");     break;
    case SDLK_WORLD_31:     strcpy(name, "WORLD_31");     break;
    case SDLK_WORLD_32:     strcpy(name, "WORLD_32");     break;
    case SDLK_WORLD_33:     strcpy(name, "WORLD_33");     break;
    case SDLK_WORLD_34:     strcpy(name, "WORLD_34");     break;
    case SDLK_WORLD_35:     strcpy(name, "WORLD_35");     break;
    case SDLK_WORLD_36:     strcpy(name, "WORLD_36");     break;
    case SDLK_WORLD_37:     strcpy(name, "WORLD_37");     break;
    case SDLK_WORLD_38:     strcpy(name, "WORLD_38");     break;
    case SDLK_WORLD_39:     strcpy(name, "WORLD_39");     break;
    case SDLK_WORLD_40:     strcpy(name, "WORLD_40");     break;
    case SDLK_WORLD_41:     strcpy(name, "WORLD_41");     break;
    case SDLK_WORLD_42:     strcpy(name, "WORLD_42");     break;
    case SDLK_WORLD_43:     strcpy(name, "WORLD_43");     break;
    case SDLK_WORLD_44:     strcpy(name, "WORLD_44");     break;
    case SDLK_WORLD_45:     strcpy(name, "WORLD_45");     break;
    case SDLK_WORLD_46:     strcpy(name, "WORLD_46");     break;
    case SDLK_WORLD_47:     strcpy(name, "WORLD_47");     break;
    case SDLK_WORLD_48:     strcpy(name, "WORLD_48");     break;
    case SDLK_WORLD_49:     strcpy(name, "WORLD_49");     break;
    case SDLK_WORLD_50:     strcpy(name, "WORLD_50");     break;
    case SDLK_WORLD_51:     strcpy(name, "WORLD_51");     break;
    case SDLK_WORLD_52:     strcpy(name, "WORLD_52");     break;
    case SDLK_WORLD_53:     strcpy(name, "WORLD_53");     break;
    case SDLK_WORLD_54:     strcpy(name, "WORLD_54");     break;
    case SDLK_WORLD_55:     strcpy(name, "WORLD_55");     break;
    case SDLK_WORLD_56:     strcpy(name, "WORLD_56");     break;
    case SDLK_WORLD_57:     strcpy(name, "WORLD_57");     break;
    case SDLK_WORLD_58:     strcpy(name, "WORLD_58");     break;
    case SDLK_WORLD_59:     strcpy(name, "WORLD_59");     break;
    case SDLK_WORLD_60:     strcpy(name, "WORLD_60");     break;
    case SDLK_WORLD_61:     strcpy(name, "WORLD_61");     break;
    case SDLK_WORLD_62:     strcpy(name, "WORLD_62");     break;
    case SDLK_WORLD_63:     strcpy(name, "WORLD_63");     break;
    case SDLK_WORLD_64:     strcpy(name, "WORLD_64");     break;
    case SDLK_WORLD_65:     strcpy(name, "WORLD_65");     break;
    case SDLK_WORLD_66:     strcpy(name, "WORLD_66");     break;
    case SDLK_WORLD_67:     strcpy(name, "WORLD_67");     break;
    case SDLK_WORLD_68:     strcpy(name, "WORLD_68");     break;
    case SDLK_WORLD_69:     strcpy(name, "WORLD_69");     break;
    case SDLK_WORLD_70:     strcpy(name, "WORLD_70");     break;
    case SDLK_WORLD_71:     strcpy(name, "WORLD_71");     break;
    case SDLK_WORLD_72:     strcpy(name, "WORLD_72");     break;
    case SDLK_WORLD_73:     strcpy(name, "WORLD_73");     break;
    case SDLK_WORLD_74:     strcpy(name, "WORLD_74");     break;
    case SDLK_WORLD_75:     strcpy(name, "WORLD_75");     break;
    case SDLK_WORLD_76:     strcpy(name, "WORLD_76");     break;
    case SDLK_WORLD_77:     strcpy(name, "WORLD_77");     break;
    case SDLK_WORLD_78:     strcpy(name, "WORLD_78");     break;
    case SDLK_WORLD_79:     strcpy(name, "WORLD_79");     break;
    case SDLK_WORLD_80:     strcpy(name, "WORLD_80");     break;
    case SDLK_WORLD_81:     strcpy(name, "WORLD_81");     break;
    case SDLK_WORLD_82:     strcpy(name, "WORLD_82");     break;
    case SDLK_WORLD_83:     strcpy(name, "WORLD_83");     break;
    case SDLK_WORLD_84:     strcpy(name, "WORLD_84");     break;
    case SDLK_WORLD_85:     strcpy(name, "WORLD_85");     break;
    case SDLK_WORLD_86:     strcpy(name, "WORLD_86");     break;
    case SDLK_WORLD_87:     strcpy(name, "WORLD_87");     break;
    case SDLK_WORLD_88:     strcpy(name, "WORLD_88");     break;
    case SDLK_WORLD_89:     strcpy(name, "WORLD_89");     break;
    case SDLK_WORLD_90:     strcpy(name, "WORLD_90");     break;
    case SDLK_WORLD_91:     strcpy(name, "WORLD_91");     break;
    case SDLK_WORLD_92:     strcpy(name, "WORLD_92");     break;
    case SDLK_WORLD_93:     strcpy(name, "WORLD_93");     break;
    case SDLK_WORLD_94:     strcpy(name, "WORLD_94");     break;
    case SDLK_WORLD_95:     strcpy(name, "WORLD_95");     break;
    case SDLK_KP0:          strcpy(name, "KP0");          break;
    case SDLK_KP1:          strcpy(name, "KP1");          break;
    case SDLK_KP2:          strcpy(name, "KP2");          break;
    case SDLK_KP3:          strcpy(name, "KP3");          break;
    case SDLK_KP4:          strcpy(name, "KP4");          break;
    case SDLK_KP5:          strcpy(name, "KP5");          break;
    case SDLK_KP6:          strcpy(name, "KP6");          break;
    case SDLK_KP7:          strcpy(name, "KP7");          break;
    case SDLK_KP8:          strcpy(name, "KP8");          break;
    case SDLK_KP9:          strcpy(name, "KP9");          break;
    case SDLK_KP_PERIOD:    strcpy(name, "KP_PERIOD");    break;
    case SDLK_KP_DIVIDE:    strcpy(name, "KP_DIVIDE");    break;
    case SDLK_KP_MULTIPLY:  strcpy(name, "KP_MULTIPLY");  break;
    case SDLK_KP_MINUS:     strcpy(name, "KP_MINUS");     break;
    case SDLK_KP_PLUS:      strcpy(name, "KP_PLUS");      break;
    case SDLK_KP_ENTER:     strcpy(name, "KP_ENTER");     break;
    case SDLK_KP_EQUALS:    strcpy(name, "KP_EQUALS");    break;
    case SDLK_UP:           strcpy(name, "UP");           break;
    case SDLK_DOWN:         strcpy(name, "DOWN");         break;
    case SDLK_RIGHT:        strcpy(name, "RIGHT");        break;
    case SDLK_LEFT:         strcpy(name, "LEFT");         break;
    case SDLK_INSERT:       strcpy(name, "INSERT");       break;
    case SDLK_HOME:         strcpy(name, "HOME");         break;
    case SDLK_END:          strcpy(name, "END");          break;
    case SDLK_PAGEUP:       strcpy(name, "PAGEUP");       break;
    case SDLK_PAGEDOWN:     strcpy(name, "PAGEDOWN");     break;
    case SDLK_F1:           strcpy(name, "F1");           break;
    case SDLK_F2:           strcpy(name, "F2");           break;
    case SDLK_F3:           strcpy(name, "F3");           break;
    case SDLK_F4:           strcpy(name, "F4");           break;
    case SDLK_F5:           strcpy(name, "F5");           break;
    case SDLK_F6:           strcpy(name, "F6");           break;
    case SDLK_F7:           strcpy(name, "F7");           break;
    case SDLK_F8:           strcpy(name, "F8");           break;
    case SDLK_F9:           strcpy(name, "F9");           break;
    case SDLK_F10:          strcpy(name, "F10");          break;
    case SDLK_F11:          strcpy(name, "F11");          break;
    case SDLK_F12:          strcpy(name, "F12");          break;
    case SDLK_F13:          strcpy(name, "F13");          break;
    case SDLK_F14:          strcpy(name, "F14");          break;
    case SDLK_F15:          strcpy(name, "F15");          break;
    case SDLK_NUMLOCK:      strcpy(name, "NUMLOCK");      break;
    case SDLK_CAPSLOCK:     strcpy(name, "CAPSLOCK");     break;
    case SDLK_SCROLLOCK:    strcpy(name, "SCROLLOCK");    break;
    case SDLK_RSHIFT:       strcpy(name, "RSHIFT");       break;
    case SDLK_LSHIFT:       strcpy(name, "LSHIFT");       break;
    case SDLK_RCTRL:        strcpy(name, "RCTRL");        break;
    case SDLK_LCTRL:        strcpy(name, "LCTRL");        break;
    case SDLK_RALT:         strcpy(name, "RALT");         break;
    case SDLK_LALT:         strcpy(name, "LALT");         break;
    case SDLK_RMETA:        strcpy(name, "RMETA");        break;
    case SDLK_LMETA:        strcpy(name, "LMETA");        break;
    case SDLK_LSUPER:       strcpy(name, "LSUPER");       break;
    case SDLK_RSUPER:       strcpy(name, "RSUPER");       break;
    case SDLK_MODE:         strcpy(name, "MODE");         break;
    case SDLK_COMPOSE:      strcpy(name, "COMPOSE");      break;
    case SDLK_HELP:         strcpy(name, "HELP");         break;
    case SDLK_PRINT:        strcpy(name, "PRINT");        break;
    case SDLK_SYSREQ:       strcpy(name, "SYSREQ");       break;
    case SDLK_BREAK:        strcpy(name, "BREAK");        break;
    case SDLK_MENU:         strcpy(name, "MENU");         break;
    case SDLK_POWER:        strcpy(name, "POWER");        break;
    case SDLK_EURO:         strcpy(name, "EURO");         break;
    case SDLK_UNDO:         strcpy(name, "UNDO");         break;
    default:                strcpy(name, "");             break;
    }

  return name;
}
