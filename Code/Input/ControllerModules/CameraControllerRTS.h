#pragma once
#include "../InputListener.h"
#include "ControllerModule.h"
#include <Data/StatsInfo.h>
#include <Visualization/Camera.h>

using namespace StatsInfo;


/** Controller for a RTS-like camera. */
class CameraControllerRTS : public ControllerModule {

  private:
    Camera* _camera;            // Camera reference.
    InputListener* _listener;   // The listener that feeds this controller.


    // Camera movement control. 
    bool _scrollActive;         // Scroll flag (used to determine toggle for reset).
    int  _moveInputs[2];        // Movement input signals (-1, 0, 1) for x and y. 
    float _moveSpeeds[2];       // Current movement speeds on these axes.

    const float _moveDelay     = 2.00f;  // Delay value for PT-1 equation.
    const float _speedMovement = 0.40f;  // Movement (displacement) factor.
    const float _speedZoom     = 0.25f;  // Zooming factor (mousewheel adjustment).
    const float _speedPitch    = 0.30f;  // Pitch adjustment factor.
    const float _speedYaw      = 0.45f;  // Turning (yaw) factor.
 

    // Camera inclination control.
    const bool _yawLock = false;    
    const bool _pitchLock = false;
    const float _pitchMin = -90.0f;
    const float _pitchMax =   0.0f;


  public:

    /** Create a RTS camera controller.
     * @param camera Camera reference.
     * @param listener Input listener reference. */
    CameraControllerRTS(Camera* camera, InputListener* listener) {
      _camera = camera;
      _listener = listener;    
      _moveInputs[0] = 0;     _moveInputs[1] = 0;
      _moveSpeeds[0] = 0.0f;  _moveSpeeds[1] = 0.0f;
      
      StatsInfo::Mode = 1; // Announce RTS camera. 
      //_camera->SetPosition(12, 10, 215, 40, -10); // For heightmap terrain.
      _camera->SetPosition(0, 0, 7, 50, -90);
    } 


    /** A mouse button was pressed.
     * @param button The pressed button. */
    void MouseButtonPressed(MouseButton button) {
      if (button == RIGHT) _listener->SetRelativeMouseMode(true);
    }
    

    /** A mouse button was released.
     * @param button The released button. */
    void MouseButtonReleased(MouseButton button) {
      if (button == RIGHT) _listener->SetRelativeMouseMode(false);
    }
    

    /** The mouse was moved.
     * @param x Either x coordinate (absolute) or difference (relative).
     * @param y Same for y-axis.
     * @param mode Current mouse mode (normal, relative, trapped). */
    void MouseMove(int x, int y, MouseMode mode) {         
      switch (mode) {
        case MODE_SCROLL: {
          _moveInputs[0] = x;   //| Note: (-y) is used, because 
          _moveInputs[1] = -y;  //| mouse up returns negative values.
          _scrollActive = true;
          break;
        }
        case MODE_NORMAL: {
          if (_scrollActive) {
            _moveInputs[0] = 0;   //| Normal mouse arrow movement. 
            _moveInputs[1] = 0;   //| Reset movement input. 
            _scrollActive = false;
          }
          break;
        }
        case MODE_RELATIVE: {
          if (_yawLock) x = 0;           //| If one axis [both axes] is locked, set
          if (_pitchLock) y = 0;         //| its input value to zero before rotating.
          _camera->RotateCamera(x*_speedYaw, -y*_speedPitch); 

          // Check, if boundary values are still valid.
          if      (Pitch < _pitchMin) _camera->SetPosition(Position.X, Position.Y, Position.Z, Yaw, _pitchMin);
          else if (Pitch > _pitchMax) _camera->SetPosition(Position.X, Position.Y, Position.Z, Yaw, _pitchMax);
          break;
        }
      }   
    }
    

    /** The mouse wheel was turned.
     * @param steps The amount of turns. */
    void MouseWheelTurned(int steps) {
      _camera->MoveCamera(0, 0, -steps*_speedZoom);
    }


    /** A key was pressed.
     * @param key The pressed key (Int32 enum value).
     * @param mod The used modifier (Ctrl, Shift...). */
    void KeyPressed(Key key, Modifier mod) {
      switch (key) {
        case KEY_LEFT:  _moveInputs[0] = -1;  break;
        case KEY_RIGHT: _moveInputs[0] =  1;  break;
        case KEY_DOWN:  _moveInputs[1] = -1;  break;
        case KEY_UP:    _moveInputs[1] =  1;  break;
      }
    }


    /** A key was released.
     * @param key The released key (Int32 enum value).
     * @param mod The used modifier (Ctrl, Shift...). */
    void KeyReleased(Key key, Modifier mod) {
      switch (key) {
        case KEY_LEFT: case KEY_RIGHT: _moveInputs[0] = 0;  break;
        case KEY_DOWN: case KEY_UP:    _moveInputs[1] = 0;  break;
      }
    }


    /** Input processing function. Is called after all events are sent. */
    void Process() {
      
      //TODO Use some funky math stuff for camera acceleration ...
      _moveSpeeds[0] = _moveInputs[0] * _speedMovement;
      _moveSpeeds[1] = _moveInputs[1] * _speedMovement;

      //printf ("RTS input: %2d ,%2d  => %5.2f ,%5.2f\n", 
      //         _moveInputs[0], _moveInputs[1], _moveSpeeds[0], _moveSpeeds[1]);
      _camera->MoveCamera(_moveSpeeds[0], _moveSpeeds[1], 0);
    }
};
