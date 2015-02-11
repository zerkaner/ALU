#pragma once
class ALU;
class Camera;
class Engine;
class IControllerModule;
class InputListener; 
#include <vector>


/** Button mappings to be used for evaluation. */
namespace InputSymbols {
  enum MouseMode {MODE_NORMAL, MODE_RELATIVE, MODE_SCROLL};
  enum MouseButton {LEFT=1, MIDDLE, RIGHT, MOUSE4, MOUSE5};
  enum Modifier    {NONE, CTRL, SHIFT, ALT};
  enum Key {   
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

    KEY_TAB=9, KEY_ENTER=13, KEY_DELETE=127
  };
}

using namespace std;
using namespace InputSymbols;


/** This controller class receives parsed user input and acts accordingly. */
class InputController {

  private:
    InputListener* _listener;                // SDL input listener. 
    vector<IControllerModule*> _controllers; // List with attached controllers.   
 
  public:

    /** Create a new input controller and used modules.
     * @param runtime ALU reference for execution module.
     * @param camera Camera reference. 
     * @param engine 3D engine reference. */
    InputController(ALU* runtime, Camera* camera, Engine* engine);  


    /** Set the input listener reference (for mouse mode change).
     * @param listener Listener backlink. */
    void SetListenerReference(InputListener* listener);


    /** A mouse button was pressed.
     * @param button The pressed button. */
    void MouseButtonPressed(MouseButton button);


    /** A mouse button was released.
     * @param button The released button. */
    void MouseButtonReleased(MouseButton button);


    /** The mouse was moved.
     * @param x Either x coordinate (absolute) or difference (relative).
     * @param y Same for y-axis. 
     * @param mode Current mouse mode (normal, relative, trapped). */
    void MouseMove(int x, int y, MouseMode mode);


    /** The mouse wheel was turned.
     * @param steps The amount of turns. */
    void MouseWheelTurned(int steps);


    /** A key was pressed.
     * @param key The pressed key (Int32 enum value).
     * @param mod The used modifier (Ctrl, Shift...). */
    void KeyPressed(Key key, Modifier mod);


    /** A key was released.
     * @param key The released key (Int32 enum value).
     * @param mod The used modifier (Ctrl, Shift...). */
    void KeyReleased(Key key, Modifier mod);


    /** Input processing function. Is called after all events are sent. */
    void Process();


    /** The window exit event (e.g. click on "X") was triggered. */
    void ExitCalled();
};
