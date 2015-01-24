#include "Agent.h"
#include <Execution/World.h>
#include <stdio.h>


Agent::Agent(World* world) {
  _id = world->GetID();
}


void Agent::Tick() {
  // Phase 1: Perception
  // Phase 2: Reasoning      
  // Phase 3: Execution
  // Check for agent deletion. 
  printf("Agent %d\n", _id);
}