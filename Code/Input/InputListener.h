#pragma once

#include "InputController.h"

#include <SDL.h>



/** This class listens at user input and sends it to a processing unit. */
class InputListener {

  private:

    InputController _controller;    // Receives the parsed input.

  public:

    InputListener(InputController controller) {
      _controller = controller;
    }


    /** Evaluates the SDL events. */
    void EvaluateEvents() {
      
      SDL_Event event;                  // Storage for SDL input.
      while (SDL_PollEvent (&event)) {  // Get all messages.     
        switch (event.type) {           // Evaluate current event.

          // A mouse button was pressed.
          case SDL_MOUSEBUTTONDOWN: {
            _controller.MouseClick((InputController::MouseButton)event.button.button);
            break;
          }

          // The mouse was moved.
          case SDL_MOUSEMOTION: {
            _controller.MouseMove(0, 0);
            break;
          }

          // Some key was pressed.
          case SDL_KEYDOWN: {
            
            // Modifier evaluation.
            InputController::Modifier mod = InputController::NONE;
            if      (event.key.keysym.mod & KMOD_CTRL)  mod = InputController::CTRL;
            else if (event.key.keysym.mod & KMOD_SHIFT) mod = InputController::SHIFT;
            else if (event.key.keysym.mod & KMOD_ALT)   mod = InputController::ALT;

            _controller.KeyPressed((char) event.key.keysym.sym, mod);
            break;
          }

          // Irrelevant event type.
          default: break;
        }    
      }
    }
};
