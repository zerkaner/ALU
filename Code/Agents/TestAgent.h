#pragma once
#include "SpatialAgent.h"
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

  public:
    TestAgent(World* world, Environment* env, Vector pos) : SpatialAgent(world, env) {
      RL = this;
      Data->Position = pos;
      if (pos.X >= 4) {
        Data->Model = new Model3D("Bear.m4");
        Data->Model->Scale = 0.02f;
        //Data->Heading.X = -45.0f;
        //Data->Heading.Y = 90.0f;
      }
      else Data->Model = new Model3D("Cube.m4");
      Data->Model->RenderingMode = Model3D::DIRECT;

      AddToEnvironment();
      printf("[TestAgent] Constructor (SA) finished!\n");
    }


    IInteraction* Reason() { 
      if (Ticks == 200) {
        printf("Killing myself!\n");
        IsAlive = false;
      }    

      Data->Movement.X = (float) (rand() % 5) - 2;
      Data->Movement.Y = (float) (rand() % 5) - 2;

      return NULL; //new TestInteraction();
    }
};
