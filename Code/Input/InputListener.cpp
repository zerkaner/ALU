#include "InputController.h"
#include "InputListener.h"

#include <SDL.h>


/** Create a new SDL2 input listener. 
 * @param controller The controller to send events to. */
InputListener::InputListener(InputController* controller)
 : _controller(controller) {
  _controller->SetListenerReference(this);
}


/** Switch for relative mouse mouse (+/- instead of coordinates).
  * @param enabled On/off flag. */
void InputListener::SetRelativeMouseMode(bool enabled) {    
  _relativeMouse = enabled;
  SDL_ShowCursor (!enabled);
  SDL_SetRelativeMouseMode((SDL_bool) enabled);
  if (enabled) SDL_GetMouseState (&_relOldX, &_relOldY);
  else {
    _relSkip = true;  // Skip flag for warp mouse event.
    SDL_WarpMouseInWindow(NULL, _relOldX, _relOldY);
  }
}


/** Evaluates the SDL events. */
void InputListener::EvaluateEvents() {
      
  SDL_Event event;                  // Storage for SDL input.
  while (SDL_PollEvent (&event)) {  // Get all messages.     
    switch (event.type) {           // Evaluate current event.

      // A mouse button was pressed.
      case SDL_MOUSEBUTTONDOWN: {
        _controller->MouseClick((InputController::MouseButton)event.button.button);
        break;
      }

      // The mouse was moved.
      case SDL_MOUSEMOTION: {
        if (_relSkip) { _relSkip = false; break; }  // Skip warp mouse event.
        if (_relativeMouse) _controller->MouseMove(event.motion.xrel, event.motion.yrel);
        else _controller->MouseMove(event.motion.x, event.motion.y);
        break;
      }

      // Some key was pressed.
      case SDL_KEYDOWN: {
            
        // Modifier evaluation.
        InputController::Modifier mod = InputController::NONE;
        if      (event.key.keysym.mod & KMOD_CTRL)  mod = InputController::CTRL;
        else if (event.key.keysym.mod & KMOD_SHIFT) mod = InputController::SHIFT;
        else if (event.key.keysym.mod & KMOD_ALT)   mod = InputController::ALT;
        _controller->KeyPressed((char) event.key.keysym.sym, mod);
        break;
      }


      // Delegate window quit command to controller for shutdown operations.
      case SDL_QUIT: {
        _controller->ExitCalled();
        break;
      }

      // Irrelevant event type.
      default: break;
    }    
  }
}
