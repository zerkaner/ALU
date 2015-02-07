#pragma once
class IAgentLogic;
class PerceptionUnit;
class World;


/** The abstract agent. This is the most generic agent form, it specifies the main execution
 *  cycle (SRA) and several extension points available for specialized agent implementations. */
class Agent {

  private:
    long _id;         // Unique identifier.    
    World* _world;    // Execution reference for add and remove queries.     

    /** The removal method stops external triggering of the agent. */
    void Remove();


  protected:
    bool IsAlive;       // Alive flag for execution and deletion checks.
    PerceptionUnit* PU; // Sensor container and input gathering. 
    IAgentLogic* RL;    // The agent's reasoning logic.


    /** Constructor for an abstract agent. It serves as a base class that is extended with
     *  domain specific sensors, actions and reasoning, optionally containing a knowledge base. */
    Agent(World* world); 

  
  public:    
    
    /** This is the main function of the agent program. It executes all three steps, 
     *  calling the concrete functions of the domain-specific agent, respectively.
     *  The execution is triggered by the 'World' this agent is living in. */
    void Tick();

};
