#pragma warning(disable: 4996) // Skip sprintf warnings.
#include <Data/StatsInfo.h>
#include <UserInterface/UserInterface.h>
#include <stdio.h>


UserInterface::UserInterface() : 
  _writer(TextWriter("..\\Output\\cali10.glf")) { 
}


void UserInterface::Update() {
  char c0[30];
  char c1[30];
  char c2[20];
  char c3[20];
  char c4[20];

  sprintf(c0, "%dx%d, %d FPS (%ldms)",
          StatsInfo::WINDOW_X, StatsInfo::WINDOW_Y, StatsInfo::FPS, StatsInfo::ExecTime);
  sprintf(c1, "Position: (%d, %d, %d)", 
          (int)StatsInfo::Position.X, 
          (int)StatsInfo::Position.Y, 
          (int)StatsInfo::Position.Z);
  sprintf(c2, "Yaw: %d'",   (int)StatsInfo::Yaw);
  sprintf(c3, "Pitch: %d'", (int)StatsInfo::Pitch);
  sprintf(c4, "Camera: %s", (StatsInfo::Mode == 1)? "RTS" : "FPS");

  _writer.WriteText(c0, 10, 05);  
  _writer.WriteText(c1, 10, 20);  
  _writer.WriteText(c2, 10, 35);  
  _writer.WriteText(c3, 10, 50);
  _writer.WriteText(c4, 10, 65);
}