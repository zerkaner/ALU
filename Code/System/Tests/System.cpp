#include <stdio.h>
#include "..\Execution\Executor.h"
#include "..\..\Data\Vector.h"
#include "..\..\Data\Object3D.h"

const float Gravity    = 9.81f;  // Earth gravity constant. [m/s^2]
const float AirDensity = 1.29f;  // Density of air.        [kg/m^3]


void UpdateObject(Object3D* obj) {
  const float factor = 0.10f;  // Fraction of second.


  float v = obj->Movement.GetLength();
  float resistance = 0.5f * obj->Cw * obj->ProfileArea * AirDensity * (v*v);
  obj->Acceleration = (obj->Heading * (obj->Thrust - resistance)) * factor;



  // Apply gravity, if acceleration vector is three-dimensional.
  if (obj->Acceleration.Is3D()) obj->Acceleration.Z -= (Gravity * factor);


  // Update movement vector.
  obj->Movement += (obj->Acceleration * factor);

  // Set new position.
  obj->Position += (obj->Movement * factor);
  
  // Output new values.
  printf ("V: %6.2f km/h | Res: %7.2f N | Acc: %5.2f m/s^2", 
          v*3.6f, resistance, obj->Acceleration.GetLength());
  getchar();
  //obj->Echo();
}


int main (int argc, char** argv) {

  Object3D obj = Object3D(Vector(0,0));
  obj.Cw =          0.8f;
  obj.ProfileArea = 1.0f;
  obj.Mass =      200.0f;
  obj.Thrust =   1000.0f;

  for (int i = 0; i < 24; i++) UpdateObject(&obj);


  //new Engine("Testfenster", 640, 480, false);

  getchar();
  return 0;
}