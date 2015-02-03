#include <Execution/ALU.h>
#include <Visualization/Camera.h>
#include <Visualization/Engine.h>
#include <stdio.h>

#include "InputController.h"
#include "InputListener.h"
#include "ControllerModules/ControllerModule.h"
#include "ControllerModules/CameraControllerFPS.h"
#include "ControllerModules/CameraControllerRTS.h"
#include "ControllerModules/EngineController.h"
#include "ControllerModules/ExecutionController.h"



InputController::InputController(ALU* runtime, Camera* camera, Engine* engine) {
  _controllers.push_back(new ExecutionController(runtime));
  _controllers.push_back(new CameraControllerRTS(camera, _listener));  
  _controllers.push_back(new EngineController(engine));
}
    


void InputController::SetListenerReference(InputListener* listener) {
  _listener = listener;
}



void InputController::MouseButtonPressed(MouseButton button) {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->MouseButtonPressed(button);
  }
}



void InputController::MouseButtonReleased(MouseButton button) {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->MouseButtonReleased(button);
  }
}



void InputController::MouseMove(int x, int y, MouseMode mode) {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->MouseMove(x, y, mode);
  }
}



void InputController::MouseWheelTurned(int steps) {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->MouseWheelTurned(steps);
  }
}



void InputController::KeyPressed(Key key, Modifier mod) {
  //printf ("Key: %d, %c\n",key, key);
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->KeyPressed(key, mod);
  }
}



void InputController::KeyReleased(Key key, Modifier mod) {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->KeyReleased(key, mod);
  }
}



void InputController::Process() {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->Process();
  }
}



void InputController::ExitCalled() {
  for (unsigned int i = 0; i < _controllers.size(); i ++) {
    _controllers[i]->ExitCalled();
  }
}
