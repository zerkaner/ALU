#pragma once;
#include "IInteraction.h"


/** This interface specifies the agent reasoning function. */
class IAgentLogic {

  public:

    /** Abstract class destructor. */
    ~IAgentLogic() {}


    /** Reasoning logic method.
     * @return The interaction the agent shall execute. */
    virtual IInteraction* Reason() = 0;
};
