#pragma once
#include "Agent.h"
#include <Execution/World.h>
#include "Reasoning/IAgentLogic.h"
#include "Reasoning/IInteraction.h"
#include <stdio.h>

/** Some basic implementations to test agent framework. */


class TestInteraction : public IInteraction {
  public:
    void Execute() {
      printf("[TestInteraction] Execute called.\n");
    }
};


class TestLogic : public IAgentLogic {
  public:
    IInteraction* Reason() {
      printf("[TestLogic] Inside Reason().\n");
      return NULL; //new TestInteraction();
    }
};


class TestAgent : public Agent {
  public:
    TestAgent(World* world) : Agent(world) {
      RL = new TestLogic();
      printf("[TestAgent] Constructor finished!\n");
    }
};
