#pragma once
#include <Data/Object3D.h>
#include <vector>


/** Base interface class for all concrete modules. */
class IPhysicsModule {

  public:
    float Factor;      // Scaling factor (resolution). Has to be set by physics engine. 
    Quadtree* Objects; // Quadtree with objects that are influenced by physics.

    /** Interface class needs virtual destructor. */
    virtual ~IPhysicsModule() {}


   /** Adds the physical effect of this module to all objects. */
    void AddEffectToAllObjects() {
      for (unsigned int i = 0; i < Objects->Objects.size(); i ++) {
        AddEffect(Objects->Objects[i]);
      }
    }


    /** Apply special effect to one object. [virtual]
     * @param obj Pointer to concrete object. */
    virtual void AddEffect(Object3D* obj) = 0;

};
