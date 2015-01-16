#pragma once
#include <Data/Primitives.h>


/** This namespace holds data copies that shall be accessible throughout the program. 
 *  The originating classes set the values, all other may read them (for example UI). */ 
namespace StatsInfo {

  // 3D Engine variables.
  extern int WINDOW_X;      // Window width.
  extern int WINDOW_Y;      // Window height. 

  // Camera variables.
  extern Float3 Position;   // Camera position.
  extern float Pitch;       // Camera pitch.
  extern float Yaw;         // Camera yaw.

  // Execution logic / ALU. 
  extern long ExecTime;     // Execution time of last cycle.
  extern int FPS;           // Targeted frames per second.
}
