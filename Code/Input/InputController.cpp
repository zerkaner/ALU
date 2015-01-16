#include <Execution/ALU.h>
#include <Input/InputController.h>
#include <Input/InputListener.h>
#include <Visualization/Camera.h>

#include <stdio.h>


InputController::InputController(ALU* runtime, Camera* camera) :
  _runtime(runtime), _camera(camera) {
}
    

void InputController::SetListenerReference(InputListener* listener) {
  _listener = listener;
}


void InputController::MouseButtonPressed(MouseButton mousebutton) {
  if (mousebutton == RIGHT) _cameraMode = ROTATE;
}


void InputController::MouseButtonReleased(MouseButton mousebutton) {
  if (mousebutton == RIGHT) _cameraMode = MOVE;
}


void InputController::MouseWheelTurned(int steps) {
  _camera->MoveCamera(0, 0, steps);
}


void InputController::MouseMove(int x, int y) {
  if      (_cameraMode == MOVE)   _camera->MoveCamera(x, -y, 0); //| (-y) is used, because mouse 
  else if (_cameraMode == ROTATE) _camera->RotateCamera(x, -y);  //| up returns negative values.
}


void InputController::KeyPressed(char key, Modifier mod) {
  printf("[Key pressed]: %c  mod %d\n", key, mod);
  if (key == 'l') _cameraMode = LOCK;
  if (key == 'm') _cameraMode = MOVE;
  if (key == 'r') _cameraMode = ROTATE;
  if (key == 'a') _listener->SetRelativeMouseMode((_relMouse = !_relMouse)); 
  if (key == 'c' && mod == CTRL) ExitCalled();
}


void InputController::ExitCalled() {
  _runtime->Stop();
}
