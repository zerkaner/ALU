#include "Agent.h"
#include "Perception/PerceptionUnit.h"
#include "Reasoning/IAgentLogic.h"
#include "Reasoning/IInteraction.h"
#include <Execution/World.h>
#include <stdio.h>


Agent::Agent(World* world) {
  ID = world->GetID();        // Get new agent ID. 
  _world = world;             // Save reference for later use.
  Ticks = 0;                  // Reset tick counter.
  IsAlive = true;             // Set alive flag.
  PU = new PerceptionUnit();  // Create new (empty) perception unit.
  _world->AddAgent(this);     // Register for execution.
}


void Agent::Tick() {
  PU->SenseAll();                                   // Phase 1: Perception
  IInteraction* action = RL->Reason();              // Phase 2: Reasoning      
  if (IsAlive && action != NULL) action->Execute(); // Phase 3: Execution
  else if (!IsAlive) Remove();                      // Agent deletion. 
  Ticks ++;
}


void Agent::Remove() {
  _world->RemoveAgent(this);
}
