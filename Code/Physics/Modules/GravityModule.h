#pragma once
#include "../IPhysicsModule.h"


/** This module applies Earth gravity force to an object. */
class GravityModule : public IPhysicsModule {

  private:
    const float Gravity = 9.81f;  // Earth gravity constant. [m/s^2]

  public:

    /** Adds a gravity force to an object.
     * @param obj Pointer to concrete object. */
    void AddEffect(Object3D* obj) {
      obj->Acceleration.Z -= (Gravity * Factor);  
    }
};
