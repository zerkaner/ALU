#pragma once
#include "Primitives.h"
#include "Model3D.h"
#include "Vector.h"


/** Base class for any physical 3D object. */
class Object3D {

  public:

    Vector Position;      // Center point of object.
    Vector Heading;       // Orientation (yaw, pitch, roll).
    Vector Movement;      // The movement speeds as a vector (V).
    Vector Acceleration;  // Change of movement speeds (A).
    Model3D* Model;       // 3D model reference.

    // Parameters for physics simulation.
    float Mass;           // Mass of object.
    float Cw;             // The drag coefficient.
    float ProfileArea;    // Cross sectional area.
    float Thrust;         // Thrust (engine, muscles, ...).

    /** Create a new object. Initially, all values are zero! */
    Object3D() {
    }
};
