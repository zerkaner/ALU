#pragma once


/** Interaction interface. Just ensures executability, needed by the abstract base agent. */  
class IInteraction {

  public:

    /** Abstract class deconstructor. */
    ~IInteraction() {}


    /** Execute the interaction object. */
    virtual void Execute() = 0;
};
