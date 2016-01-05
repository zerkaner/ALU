#pragma once
#include "Primitives.h"
#include "Model.h"


/** Base class for any physical 3D object. */
class Object3D {

  public:

    Float3 Position;      // Center point of object.
    Float3 Heading;       // Orientation (yaw, pitch, roll).
    Float3 Movement;      // The movement speeds as a vector (V).
    Float3 Acceleration;  // Change of movement speeds (A).
    Model2* Model;        // 3D model reference.

    // Parameters for physics simulation.
    float Mass;           // Mass of object.
    float Cw;             // The drag coefficient.
    float ProfileArea;    // Cross sectional area.
    float Thrust;         // Thrust (engine, muscles, ...).

    /** Create a new object. Initially, all values are zero! */
    Object3D() {
    }
};
