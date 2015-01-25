#include <Data/StatsInfo.h>

namespace StatsInfo {

  /** Initialization of extern variables (with default values). */
  
  // 3D Engine variables.
  int WINDOW_X = 0;
  int WINDOW_Y = 0; 

  // Camera variables.
  Float3 Position = Float3();
  float Pitch = 0.0f;
  float Yaw = 0.0f;
  int Mode = -1;  

  // Execution logic / ALU. 
  long ExecTime = 0;
  int FPS = 0;
  long Ticks = 0;
}
