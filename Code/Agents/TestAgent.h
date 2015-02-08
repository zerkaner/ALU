#pragma once
#include "Agent.h"
#include "SpatialAgent.h"
#include <Environment/Cube.h>
#include <Environment/Environment.h>
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


class TestAgent : public SpatialAgent, public IAgentLogic {
  
  private:
    int i = 0;


  public:
    TestAgent(World* world, Environment* env) : SpatialAgent(world, env) {
      RL = this;
      Data = new Cube(Float3(0.5,0.5,0.5));
      Data->Position = Vector(2,2,2);
      AddToEnvironment();
      printf("[TestAgent] Constructor (SA) finished!\n");
    }


    IInteraction* Reason() {
      printf("[TestLogic] Inside Reason().\n");
      i ++;    
      if (i == 20) {
        printf("Killing myself!\n");
        IsAlive = false;
      }    

      int rnd = rand() % 4;
      rnd -= 2;
      Data->Position.X = rnd;

      return NULL; //new TestInteraction();
    }
};
