#include <Execution/ALU.h>
#include <Visualization/Camera.h>
#include <stdio.h>

#include "InputController.h"
#include "InputListener.h"
#include "ControllerModules/ControllerModule.h"
#include "ControllerModules/CameraControllerFPS.h"
#include "ControllerModules/CameraControllerRTS.h"
#include "ControllerModules/ExecutionController.h"


/** Create a new input controller and used modules.
 * @param runtime ALU reference for execution module.
 * @param camera Camera reference. */
InputController::InputController(ALU* runtime, Camera* camera) {
  _controllers.push_back(new ExecutionController(runtime));
  _controllers.push_back(new CameraControllerRTS(camera, _listener));  
}
    

/** Set the input listener reference (for mouse mode change).
 * @param listener Listener backlink. */
void InputController::SetListenerReference(InputListener* listener) {
  _listener = listener;
}


/** A mouse button was pressed.
 * @param button The pressed button. */
void InputController::MouseButtonPressed(MouseButton button) {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->MouseButtonPressed(button);
  }
}


/** A mouse button was released.
 * @param button The released button. */
void InputController::MouseButtonReleased(MouseButton button) {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->MouseButtonReleased(button);
  }
}


/** The mouse was moved.
 * @param x Either x coordinate (absolute) or difference (relative).
 * @param y Same for y-axis. 
 * @param mode Current mouse mode (normal, relative, trapped). */
void InputController::MouseMove(int x, int y, MouseMode mode) {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->MouseMove(x, y, mode);
  }
}


/** The mouse wheel was turned.
 * @param steps The amount of turns. */
void InputController::MouseWheelTurned(int steps) {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->MouseWheelTurned(steps);
  }
}


/** A key was pressed.
 * @param key The pressed key (Int32 enum value).
 * @param mod The used modifier (Ctrl, Shift...). */
void InputController::KeyPressed(Key key, Modifier mod) {
  //printf ("Key: %d, %c\n",key, key);
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->KeyPressed(key, mod);
  }
}


/** A key was released.
 * @param key The released key (Int32 enum value).
 * @param mod The used modifier (Ctrl, Shift...). */
void InputController::KeyReleased(Key key, Modifier mod) {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->KeyReleased(key, mod);
  }
}


/** Input processing function. Is called after all events are sent. */
void InputController::Process() {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->Process();
  }
}


/** The exit event was triggered. */
void InputController::ExitCalled() {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->ExitCalled();
  }
}
