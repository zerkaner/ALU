#pragma once

#include <stdio.h>


/** This controller class receives parsed user input and acts accordingly. */
class InputController {


  public:
    InputController() {
    }

    enum MouseButton {LEFT=1, MIDDLE, RIGHT, MOUSE4, MOUSE5};
    enum Modifier    {NONE, CTRL, SHIFT, ALT};
    enum SpecialKey  {};


    void MouseClick(MouseButton button) {
      printf("[Mouse click]: %d\n", button);
    }

    void MouseMove(int x, int y) {
      printf("[Mouse moved]: (%03d|%03d)\n", x, y);
    }

    void KeyPressed(char key, Modifier mod) {
      printf("[Key pressed]: %c  mod %d\n", key, mod);
    }

};
