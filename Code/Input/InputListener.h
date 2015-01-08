#pragma once
class InputController;


/** This class listens at user input and sends it to a processing unit. */
class InputListener {

  private:

    InputController* _controller;   // Receiver of the parsed input.
    bool _relativeMouse = false;    // Flag for relative mouse movement.
    int _relOldX, _relOldY;         // Mouse position for rel->abs pointer reset.
    bool _relSkip = false;          // Skip flag for warp mouse event.

  public:

    /** Create a new SDL2 input listener.
     * @param controller The controller to send events to. */
    InputListener(InputController* controller);


    /** Switch for relative mouse mouse (+/- instead of coordinates).
     * @param enabled On/off flag. */
    void SetRelativeMouseMode(bool enabled);


    /** Evaluates the SDL events. */
    void EvaluateEvents();
};
