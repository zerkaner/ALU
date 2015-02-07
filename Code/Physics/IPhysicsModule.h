#pragma once
#include <Data/Object3D.h>
#include <Visualization/Model3D.h>
#include <vector>


/** Base interface class for all concrete modules. */
class IPhysicsModule {

  public:
    float Factor;  // Scaling factor (resolution). Has to be set by physics engine. 


    /** Interface class needs virtual destructor. */
    virtual ~IPhysicsModule() {}


   /** Adds the physical effect of this module to all objects.
    * @param objects List of all objects to apply this effect to. */
    void AddEffectToAllObjects(std::vector<Model3D*> objects) {
      for (unsigned int i = 0; i < objects.size(); i ++) {
        AddEffect(objects[i]);
      }
    }


    /** Apply special effect to one object. [virtual]
     * @param obj Pointer to concrete object. */
    virtual void AddEffect(Object3D* obj) = 0;

};
