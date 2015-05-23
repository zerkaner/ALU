#pragma once


/** Button mappings to be used for evaluation. */
namespace InputSymbols {

  enum MouseMode {MODE_NORMAL, MODE_RELATIVE, MODE_SCROLL};
  enum MouseButton {LEFT=1, MIDDLE, RIGHT, MOUSE4, MOUSE5};
  enum Modifier    {NONE, CTRL, SHIFT, ALT};
  enum Key {   

    // https://wiki.libsdl.org/SDLKeycodeLookup // SDL Keycode Lookup Table.
    KEY_0=48, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,   // Numeric input.

    KEY_A=97, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, //| Default character input.
    KEY_J,    KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, //| The case is determined by
    KEY_S,    KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,        //| additional modificator.
    KEY_SZ=223, KEY_AUML=228, KEY_OUML=246, KEY_UUML=252,             // German special keys.
    
    KEY_CAPSLOCK=1073741881,  
    KEY_F1, KEY_F2, KEY_F3, KEY_F4,  KEY_F5,  KEY_F6,  // Function keys.
    KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,  

    KEY_PRINT, KEY_SCROLL, KEY_PAUSE, KEY_INSERT, KEY_HOME, KEY_PAGEUP,      //| Control block.
    KEY_END=1073741901, KEY_PAGEDOWN, KEY_RIGHT, KEY_LEFT, KEY_DOWN, KEY_UP, //| and arrow keys.

    KEY_CTRL_L=1073742048, KEY_SHIFT_L, KEY_ALT_L,     // Modifier keys.
    KEY_WIN, KEY_CTRL_R, KEY_SHIFT_R, KEY_ALT_R,

    KEY_TAB=9, KEY_ENTER=13, KEY_SPACE=32, KEY_DELETE=127
  };
}
