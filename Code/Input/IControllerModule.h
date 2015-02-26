#pragma once
#include "InputSymbols.h"
using namespace InputSymbols;


/** Base module, specifies called functions (to be overwritten by concrete controller). */
class IControllerModule {

  protected:

    /** Only allow creation out of inherited classes. */
    IControllerModule() {}

  public:

    /** Virtual destructor. */
    virtual ~IControllerModule() {}


    /** A mouse button was pressed.
     * @param button The pressed button. */
    virtual void MouseButtonPressed(MouseButton button) {}
    

    /** A mouse button was released.
     * @param button The released button. */
    virtual void MouseButtonReleased(MouseButton button) {}
    

    /** The mouse was moved.
     * @param x Either x coordinate (absolute) or difference (relative).
     * @param y Same for y-axis.
     * @param mode Current mouse mode (normal, relative, trapped). */
    virtual void MouseMove(int x, int y, MouseMode mode) {}
    

    /** The mouse wheel was turned.
     * @param steps The amount of turns. */
    virtual void MouseWheelTurned(int steps) {}


    /** A key was pressed.
     * @param key The pressed key (Int32 enum value).
     * @param mod The used modifier (Ctrl, Shift...). */
    virtual void KeyPressed(Key key, Modifier mod) {}


    /** A key was released.
     * @param key The released key (Int32 enum value).
     * @param mod The used modifier (Ctrl, Shift...). */
    virtual void KeyReleased(Key key, Modifier mod) {}


    /** Input processing function. Is called after all events are sent. */
    virtual void Process() {}


    /** The exit event was triggered. */
    virtual void ExitCalled() {}
};
