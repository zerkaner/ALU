#pragma warning(disable: 4996) // Skip sprintf warnings.
#include <UserInterface/UserInterface.h>
#include <Visualization/Camera.h>
#include <stdio.h>


UserInterface::UserInterface(Camera* camera) : 
  _writer(TextWriter("..\\Output\\cali10.glf")),
  _camera(camera) { 
}


void UserInterface::Update() {

  char c1[30];
  char c2[20];
  char c3[20];
  sprintf(c1, "Position: (%d, %d, %d)", (int)_camera->Pos.X, (int)_camera->Pos.Y, (int)_camera->Pos.Z);
  sprintf(c2, "Yaw: %d'",   (int)_camera->Yaw);
  sprintf(c3, "Pitch: %d'", (int)_camera->Pitch);

  _writer.WriteText(c1, 10, 20);  
  _writer.WriteText(c2, 10, 35);  
  _writer.WriteText(c3, 10, 50);
}