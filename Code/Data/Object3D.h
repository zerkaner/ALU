#pragma once

#include "Vector.h"

#include <stdio.h>


class Object3D {

  private:

  public:

    float Mass;
    float Cw;
    float ProfileArea;
    float Thrust;

    Vector Position     = Vector(0,0);
    Vector Heading      = Vector(1,0);
    Vector Movement     = Vector(0,0);
    Vector Acceleration = Vector(0,0);

    Object3D(Vector pos) {
      Position = pos;
    }


    void Echo() {
      printf("Object3D output:\n"
             " - Position: %s\n"
             " - Speed   : %s  (%5.2f)\n"
             " - Acceler.: %s  (%5.2f)\n\n",
             Position.ToString(),
             Movement.ToString(), Movement.GetLength(),
             Acceleration.ToString(), Acceleration.GetLength());
    }
};
