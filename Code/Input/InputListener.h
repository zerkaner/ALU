#pragma once
#include "InputController.h"
#include <SDL.h>

using namespace InputSymbols;


/** This class listens at user input and sends it to a processing unit. */
class InputListener {

  private:
    SDL_Window** _window;           // Pointer to SDL window handler.
    InputController* _controller;   // Receiver of the parsed input.
    int _relOldX, _relOldY;         // Mouse position for rel->abs pointer reset.
    bool _relSkip = false;          // Skip flag for warp mouse event.      
    MouseMode _mouseMode;           // current mouse mode (normal, relative, trapped).
    bool _trapMouse;                // Mouse trap mode. If enabled, arrow cannot leave window.
    int _scrollDir[2];              // Last scroll direction. Used to determine scroll changes.


  public:   

    /** Create a new SDL2 input listener.
     * @param controller The controller to send events to. 
     * @param window Address of pointer to SDL window handler. */
    InputListener(InputController* controller, SDL_Window** window);


    /** Switch for relative mouse mouse (+/- instead of coordinates).
     * @param enabled On/off flag. */
    void SetRelativeMouseMode(bool enabled);


    /** Evaluates the SDL events. */
    void EvaluateEvents();
};
