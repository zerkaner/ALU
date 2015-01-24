#pragma once
#include <vector>
class Agent;
class Environment;
class IDrawable;
class PhysicsEngine;


/** Superior class that holds all agents, the environment and physics system. 
 *  It represents the virtual world and is executed in discrete time steps. */
class World {

  private:
    Environment* _environment;    // The environment representation.
    std::vector<Agent*> _agents;  // List of all 'living' beings in this world.  
    std::vector<Agent*> _rndAgts; // Random agent execution list.
    PhysicsEngine* _physics;      // Link to the used physics system. 
    int _execIndex;               // Current agent execution list index.
    int _execSize;                // Current size of execution list.
    bool _randomExec;             // Random agent execution flag.
    long _ticks;                  // Execution counter.
    long _idCounter;              // ID distribution counter.


  public:

    /** Create a new world and instantiate all subcomponents. */
    World();
    

    /** Advance the world by one time step. Calls all subroutines in sequential order. */
    void AdvanceOneTick();


    /** Adds an agent to the world. It is appended to the  
     *  agent list and will be run in the next execution cycle.
     * @param agent The agent to add. */
    void AddAgent(Agent* agent);


    /** Removes an agent from the world.
     * @param agent The agent to remove. */
    void RemoveAgent(Agent* agent);
    

    /** ID distribution function.
     * @return An unused ID (beginning from 1). */
    long GetID();


    /** Returns a pointer to the drawable version.
     * @return Pointer to the environment as an IDrawable form. */
    IDrawable* GetDrawableWorld();
};
