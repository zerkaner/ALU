#pragma once
#include <Data/Primitives.h>

/* Axis definition in OpenGL:
  x-Axis: (-)  left  |  right  (+)
  y-Axis: (-)  down  |     up  (+)
  z-Axis: (-) farer  |  nearer (+)
  [right-handed coordinate system]
  // not to be confused with pixels on screen [(0,0) = (top,left)]
*/


/** The camera represents and adjusts a viewing area in the 3D environment. */
class Camera {

  private:
    Float3 _pos;           // Coordinates of the camera position.
    float _pitch, _yaw;    // Pitch (up/down) and yaw (turn around).
    
    const float SpeedMovement = 0.05f;  // Movement (displacement) factor.
    const float SpeedZoom     = 0.20f;  // Zooming factor (mousewheel adjustment).
    const float SpeedPitch    = 0.30f;  // Pitch adjustment factor.
    const float SpeedYaw      = 0.45f;  // Turning (yaw) factor.


    /** Restrict pitch value to interval (-90° -> 90°). 
     * @param pitch New pitch value. */
    void SetPitch(float pitch);


    /** Limit yaw value to compass heading (0° -> <360°).
     * @param yaw New yaw value. */
    void SetYaw(float yaw);


  public:

    /** Initialize a camera at (0,0,0) facing straight north. */
    Camera();


    /** Initialize a camera at given position (x,y,z) and with optional pitch and yaw. */
    Camera(float posX, float posY, float posZ, float yaw=0.0f, float pitch=0.0f);


    /** Moves the camera around. Calculates the new position based on the
     *  current heading and the supplied mouse/keyboard offset input.
     * @param xIn X delta input (mouse/kbd. x-axis, -: left, +: right).
     * @param yIn Y delta input (mouse/kbd, y-axis, -: down, +: up).
     * @param zIn Z delta input (zoom level,        -: out,  +: in). */
    void MoveCamera(int xIn, int yIn, int zIn);


    /** Rotates the camera around lateral and vertical axes.
     * @param xIn X delta input (yaw axis).
     * @param yIn Y delta input (pitch axis). */
    void RotateCamera(int xIn, int yIn);


    /** Update the camera values. */
    void Update ();
};
