#pragma once
class World;


/** The abstract agent. This is the most generic agent form, it specifies the main execution
 *  cycle (SRA) and several extension points available for specialized agent implementations. */
class Agent {

  private:
    long _id;


  protected:
 
  
  public:    
    
    /** Constructor for an abstract agent. It serves as a base class that is extended with
     *  domain specific sensors, actions and reasoning, optionally containing a knowledge base. */
    Agent(World* world);

    
    /** This is the main function of the agent program. It executes all three steps, 
     *  calling the concrete functions of the domain-specific agent, respectively.
     *  The execution is triggered by the 'World' this agent is living in. */
    void Tick();

};
