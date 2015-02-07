#pragma once
#include <Data/Object3D.h>
#include <Environment/Environment.h>
#include <Environment/Quadtree.h>
#include <Physics/IPhysicsModule.h>
#include <Physics/Modules/GravityModule.h>
#include <vector>


/** The physics engine aggregates several modules with in turn apply physical effects to the objects. */
class PhysicsEngine {

  private:
    Quadtree* _objects;                    // Quadtree with objects that are influenced by physics.
    std::vector<IPhysicsModule*> _modules; // List of physics modules to apply.
    
    float _resolution;  //| Scaling factor in respect to one "time unit". If that corresponds to 1 
                        //| sec. and the system is triggered with 25 FPS, the factor would be 0.04. 

  public:

    /** Create the physics engine. 
     * @param objects Quadtree with environmental objects. */
    PhysicsEngine(Quadtree* objects) {
      _objects = objects;
      _resolution = 0.04f;
    }
    

    /** Adds a new module to the physics stack.
     * @param module Physics module to add. */
    void AddModule(IPhysicsModule* module) {
      module->Factor = _resolution;
      _modules.push_back(module);
    }


    /** Run the physics engine for the current step. */
    void Execute() {
      
      // Applies physical influences to all objects.
      for (unsigned int i = 0; i < _modules.size(); i ++) {
        _modules[i]->AddEffectToAllObjects(_objects->Objects);
      }

      // Perform time-dependant value changes.
      for (unsigned int i = 0; i < _objects->Objects.size(); i ++) {
        _objects->Objects[i]->Movement += (_resolution * _objects->Objects[i]->Acceleration);
        _objects->Objects[i]->Position += (_resolution * _objects->Objects[i]->Movement);
      }
    }
};
