#include <Data/Primitives.h>
#include <Visualization/Camera.h>

#include <SDL_opengl.h>
#include <cmath>
#include <stdio.h>


/** Initialize a camera at (0,0,0) facing straight north. */
Camera::Camera() {}


/** Initialize a camera at given position (x,y,z) and with optional pitch and yaw. */
Camera::Camera(float posX, float posY, float posZ, float yaw, float pitch) : 
  _pos(Float3(posX, posY, posZ)) {
  SetPitch(pitch);
  SetYaw(yaw);
}


/** Restrict pitch value to interval (-90° -> 90°). 
  * @param pitch New pitch value. */
void Camera::SetPitch(float pitch) {
  if (pitch > 90.0f) _pitch = 90.0f;
  else if (pitch < -90.0f) _pitch = -90.0f; //TODO [-89.99] Zeichenfehler bei -90°. Warum???
  else _pitch = pitch;
}


/** Limit yaw value to compass heading (0° -> <360°).
  * @param yaw New yaw value. */
void Camera::SetYaw(float yaw) {
  _yaw = yaw;
  if (_yaw < 0.0f) _yaw += 360.0f;
  if (_yaw >= 360.0f) _yaw -= 360.0f;
}


/** Moves the camera around. Calculates the new position based on the
  *  current heading and the supplied mouse/keyboard offset input.
  * @param xIn X delta input (mouse/kbd. x-axis, -: left, +: right).
  * @param yIn Y delta input (mouse/kbd, y-axis, -: down, +: up).
  * @param zIn Z delta input (zoom level,        -: out,  +: in). */
void Camera::MoveCamera(int xIn, int yIn, int zIn) {
      
  //TODO Kameragrenzen ?? [Anhand der Umweltdimensionen festlegen]

  // Horizontal displacement.
  if (xIn != 0 || yIn != 0) {
        
    /* Calculation of position with orientation (yaw) taken into account:
      * 
      *       0°  90° 180° 270°                     ATT!                           0    90   180   270 
      * xPos  x    y   -x   -y   : cos(x) + sin(y)      x*cos(yaw) + y*sin(yaw)   1,0  0,1  -1,0  0,-1
      * yPos  y   -x   -y    x   : sin(x) + cos(y)  (-) x*sin(yaw) + y*cos(yaw)   0,1  1,0  0,-1  -1,0      
      *
      * Attention: yDelta of event is inverse (up=negative values)!        
      */

    float yawRad = _yaw * 0.0174532925f;  // Degree to radians.
    _pos.X += (SpeedMovement*( xIn*cos(yawRad) + yIn*sin(yawRad)));
    _pos.Y += (SpeedMovement*(-xIn*sin(yawRad) + yIn*cos(yawRad)));
  }

  // Zooming mode.
  if (zIn != 0) {
    _pos.Z += zIn*SpeedZoom;
  }
}


/** Rotates the camera around lateral and vertical axes.
  * @param xIn X delta input (yaw axis).
  * @param yIn Y delta input (pitch axis). */
void Camera::RotateCamera(int xIn, int yIn) {
  SetYaw(_yaw + SpeedYaw*xIn);
  SetPitch(_pitch + SpeedPitch*yIn);    
}


/** Update the camera values. */
void Camera::Update () {

  printf("Pos: (%3d,%3d,%3d)  |  Yaw: %-3d | Pitch: %-3d\n", 
          (int)_pos.X, (int)_pos.Y, (int)_pos.Z, (int)_yaw, (int)_pitch);

  glRotatef (_pitch+90, -1.0f, 0.0f, 0.0f);  // Rotate on x-Axis (set height).
  glRotatef (_yaw,       0.0f, 0.0f, 1.0f);  // Rotate on y-Axis (set spin).
  glTranslatef (-_pos.X, -_pos.Y, -_pos.Z);  // Set the camera position.
}
