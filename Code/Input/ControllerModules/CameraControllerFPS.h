#pragma once
#include "../InputListener.h"
#include "../IControllerModule.h"
#include <Data/StatsInfo.h>
#include <Visualization/Camera.h>

using namespace StatsInfo;


/** This controller is intended for FPS-like camera movement. */
class CameraControllerFPS : public IControllerModule {
  
  private:
    Camera* _camera;        // Camera reference.
    int _moveInputs[2];     // Movement input signals (-1, 0, 1) for x and y. 
    int _strafe;            // Diagonal movement (forward and left/right).
    float _moveSpeeds[2];   // Current movement speeds on these axes.

    const bool  _strafeEnabled = true;    // Is diagonal movement allowed?
    const bool  _heightLock    = true;    // Is camera height change forbidden?
    const bool  _turnOnLR      = false;   // If set, the camera is turned (RP like).
    const float _pitchMax      =  90.0f;  // Maximum +pitch angle: Look to the stars! 
    const float _pitchMin      = -90.0f;  // Maximum -pitch angle: Bottom view.
    const float _speedMovement =  0.40f;  // Movement (displacement) factor.
    const float _speedPitch    =  0.30f;  // Pitch adjustment factor.
    const float _speedYawMouse =  0.45f;  // Turning (yaw) factor.
    const float _speedYawKbd   =  2.75f;  // Turning factor for keyboard input.
 

  public:

    /** Create a FPS camera controller.
     * @param camera Camera reference.
     * @param listener Input listener reference. */
    CameraControllerFPS(Camera* camera, InputListener* listener) {
      _camera = camera;
      listener->SetRelativeMouseMode(true);   
      _moveInputs[0] = 0;     _moveInputs[1] = 0;
      _moveSpeeds[0] = 0.0f;  _moveSpeeds[1] = 0.0f;
      StatsInfo::Mode = 2;
    } 


    /** The mouse was moved.
     * @param x Either x coordinate (absolute) or difference (relative).
     * @param y Same for y-axis.
     * @param mode Current mouse mode (normal, relative, trapped). */
    void MouseMove(int x, int y, MouseMode mode) {    
      if (mode == MODE_RELATIVE) {
        _camera->RotateCamera(x*_speedYawMouse, -y*_speedPitch); // Check, if boundary values are still valid.
        if      (Pitch < _pitchMin) _camera->SetPosition(Position.X, Position.Y, Position.Z, Yaw, _pitchMin);
        else if (Pitch > _pitchMax) _camera->SetPosition(Position.X, Position.Y, Position.Z, Yaw, _pitchMax);
      }
    }


    /** A key was pressed.
     * @param key The pressed key (Int32 enum value).
     * @param mod The used modifier (Ctrl, Shift...). */
    void KeyPressed(Key key, Modifier mod) {
      switch (key) {
        case KEY_LEFT:  case KEY_A: _moveInputs[0] = -1;  break;
        case KEY_RIGHT: case KEY_D: _moveInputs[0] =  1;  break;
        
        // Strafe movement.
        case KEY_Q: if (_strafeEnabled) _strafe = -1;  break;
        case KEY_E: if (_strafeEnabled) _strafe =  1;  break;

        // Forward / backward input.
        case KEY_UP:   case KEY_W: _moveInputs[1] =  1;  break;
        case KEY_DOWN: case KEY_S: _moveInputs[1] = -1;  break;
        
      }
    }


    /** A key was released.
     * @param key The released key (Int32 enum value).
     * @param mod The used modifier (Ctrl, Shift...). */
    void KeyReleased(Key key, Modifier mod) {
      switch (key) {
        case KEY_LEFT: case KEY_RIGHT: case KEY_A: case KEY_D: _moveInputs[0] = 0;  break;
        case KEY_DOWN: case KEY_UP: case KEY_W: case KEY_S: _moveInputs[1] = 0;  break;
        case KEY_Q: case KEY_E: _strafe = 0;  break;
      }
    }


    /** Input processing function. Is called after all events are sent. */
    void Process() {

      // If we use strafing, split the full speed for both axes (Pythagoras, *0.7).
      if (_strafe != 0) {
        _moveSpeeds[0] = 0.70f * _speedMovement * _strafe;
        _moveSpeeds[1] = 0.70f * _speedMovement;
      }

      else {
        //TODO Use some funky math stuff for camera acceleration ...
        _moveSpeeds[0] = _moveInputs[0] * _speedMovement;
        _moveSpeeds[1] = _moveInputs[1] * _speedMovement;      
      }


      // Camera turn mode.
      if (_turnOnLR) {
        _camera->RotateCamera(_moveInputs[0] * _speedYawKbd, 0);
         if (_strafe == 0) _moveSpeeds[0] = 0.0f;
      }

      _camera->MoveCamera(_moveSpeeds[0], _moveSpeeds[1], 0);
    }
};
