#include "InputListener.h"
#include <Data/StatsInfo.h>
using namespace StatsInfo;


/** Create a new SDL2 input listener. 
 * @param controller The controller to send events to.
 * @param window Address of pointer to SDL window handler. */
InputListener::InputListener(InputController* controller, SDL_Window** window) :
  _controller(controller), _window(window) {
  _controller->SetListenerReference(this);
  _scrollDir[0] = 0;  _scrollDir[1] = 0;
  _trapMouse = true;
}


/** Switch for relative mouse mouse (+/- instead of coordinates).
  * @param enabled On/off flag. */
void InputListener::SetRelativeMouseMode(bool enabled) {    
  if (enabled) _mouseMode = MODE_RELATIVE;
  else         _mouseMode = MODE_NORMAL;
  SDL_SetRelativeMouseMode((SDL_bool) enabled);
  if (enabled) SDL_GetMouseState (&_relOldX, &_relOldY);
  else {
    _relSkip = true;  // Skip flag for warp mouse event.
    SDL_WarpMouseInWindow((*_window), _relOldX, _relOldY);
  }
}


/** Evaluates the SDL events. */
void InputListener::EvaluateEvents() {
      
  SDL_Event event;                  // Storage for SDL input.
  while (SDL_PollEvent (&event)) {  // Get all messages.     
    switch (event.type) {           // Evaluate current event.

      // A mouse button was pressed.
      case SDL_MOUSEBUTTONDOWN: {
        _controller->MouseButtonPressed((MouseButton)event.button.button);
        break;
      }


      // The mouse button was released.
      case SDL_MOUSEBUTTONUP: {
        _controller->MouseButtonReleased((MouseButton)event.button.button);
        break;
      }


      // A window event was fired. If in focus, trap mouse in window, release otherwise.
      case SDL_WINDOWEVENT: {
        switch (event.window.event) {
          case SDL_WINDOWEVENT_ENTER:
          case SDL_WINDOWEVENT_FOCUS_GAINED: {
            if (_trapMouse) SDL_SetWindowGrab((*_window), (SDL_bool) true);
            break;
          }          
          case SDL_WINDOWEVENT_LEAVE:
          case SDL_WINDOWEVENT_FOCUS_LOST: {
            if (_trapMouse) SDL_SetWindowGrab((*_window), (SDL_bool) false);
            break;
          }
        }
        break;
      }


      // The mouse was moved.
      case SDL_MOUSEMOTION: {
        if (_relSkip) { _relSkip = false; break; }  // Skip warp mouse event.     
        
        // Relative mouse movement (the easy case ...)
        if (_mouseMode == MODE_RELATIVE) {
          _controller->MouseMove(event.motion.xrel, event.motion.yrel, MODE_RELATIVE);
          break;
        }

        // If mouse trap enabled, check for boundary exceedance.
        if (_trapMouse) { 
          int limits[2] = {0, 0};  // Default assignment.
          if      (event.motion.x ==          0) limits[0] = -1;  // left
          else if (event.motion.x == WINDOW_X-1) limits[0] =  1;  // right
          if      (event.motion.y ==          0) limits[1] = -1;  // top
          else if (event.motion.y == WINDOW_Y-1) limits[1] =  1;  // bottom
        
          // Mouse arrow within the boundaries.
          if (limits[0] == 0 && limits[1] == 0) {  
            _controller->MouseMove(event.motion.x, event.motion.y, MODE_NORMAL);
            _scrollDir[0] = 0;  _scrollDir[1] = 0;
          }

          // Boundary mode. Check for changes here.
          else if (limits[0] != _scrollDir[0] || limits[1] != _scrollDir[1]) {

            // Send scroll event and take current values as new reference.
            _controller->MouseMove(limits[0], limits[1], MODE_SCROLL);
            _scrollDir[0] = limits[0];  _scrollDir[1] = limits[1]; 
          }        
        }

        // We have no mouse trap enabled. Just send event.
        else _controller->MouseMove(event.motion.x, event.motion.y, MODE_NORMAL);
        break;
      }


      // Mouse wheel was turned (only up/down currently supported).
      case SDL_MOUSEWHEEL: {
        _controller->MouseWheelTurned(event.wheel.y);
        break;
      }


      // Some key was pressed (or released).
      case SDL_KEYDOWN:
      case SDL_KEYUP: {
            
        // Modifier evaluation.
        InputSymbols::Modifier mod = InputSymbols::NONE;
        if      (event.key.keysym.mod & KMOD_CTRL)  mod = InputSymbols::CTRL;
        else if (event.key.keysym.mod & KMOD_SHIFT) mod = InputSymbols::SHIFT;
        else if (event.key.keysym.mod & KMOD_ALT)   mod = InputSymbols::ALT;
        if (event.type == SDL_KEYDOWN) _controller->KeyPressed((Key) event.key.keysym.sym, mod);
        else _controller->KeyReleased((Key) event.key.keysym.sym, mod);
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

  // All events are done. Send Process() call to controller modules.
  _controller->Process();
}
