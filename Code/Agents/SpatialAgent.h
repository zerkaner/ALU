#pragma once
#include "Agent.h"
#include <Data/Object3D.h>
#include <Execution/World.h>
#include <Environment/Environment.h>


/** This agent is part of a spatial world. It has a position and 
 *  is registered at an environment to provide collision detection. */
class SpatialAgent : public Agent {
  
  private:
    Environment* _env;  // Environment implementation reference.

  protected:
    Object3D* Data;  // Spatial data and model container. 


    /** Instantiate a new agent with spatial data. Only available for specializations.
     * @param world Execution reference, delegated to base class.
     * @param env The environment, used to add (and remove) agent model.*/
    SpatialAgent(World* world, Environment* env) : Agent(world) {
      _env = env;     
    }

   
    /** Add agent to environment. Call this function is the concrete agent 
     *  constructor, after the spatial data have been initialized! */
    void AddToEnvironment() {
      _env->AddObject(Data); 
    }


    /** This function unbinds the agent from the environment.
     *  It is triggered by the base agent, when alive flag on 'false'. */
    void Remove() {
      Agent::Remove();
      _env->RemoveObject(Data);
    }
};
