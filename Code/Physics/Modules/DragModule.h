#pragma once
#include "../IPhysicsModule.h"


/** Adds drag and friction as negative acceleration to the objects. */
class GravityModule : public IPhysicsModule {

  private:
    const float AirDensity = 1.20f;  // Density of air. [kg/m^3]

  public:

    /** Adds air drag and friction to an object.
     * @param obj Pointer to concrete object. */
    void AddEffect(Object3D* obj) {
    }
};






/*

void UpdateObject(Object3D* obj) {
  const float factor = 0.10f;  // Fraction of second.

  float v = obj->Movement.GetLength();
  float resistance = 0.5f * obj->Cw * obj->ProfileArea * AirDensity * (v*v);
  obj->Acceleration = (obj->Heading * (obj->Thrust - resistance)) * factor;

  // Apply gravity, if acceleration vector is three-dimensional.
  if (obj->Acceleration.Is3D()) obj->Acceleration.Z -= (Gravity * factor);

}

/*  
  Object3D obj = Object3D(Vector(0,0));
  obj.Cw =          0.8f;
  obj.ProfileArea = 1.0f;
  obj.Mass =      200.0f;
  obj.Thrust =   1000.0f;
 
  //for (int i = 0; i < 24; i++) UpdateObject(&obj);
*/ 
