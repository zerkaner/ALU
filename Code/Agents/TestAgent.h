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
  
  private:
    int i = 0;


  public:
    TestAgent(World* world, Environment* env) : SpatialAgent(world, env) {
      RL = this;
      Data->Position = Vector(2.0f, 2.0f, 0.5f);
      Data->Model = new Model3D("Cube.m4");
      Data->Model->RenderingMode = Model3D::DIRECT;
      //Data->Model->Scale = 0.02f;

      AddToEnvironment();
      printf("[TestAgent] Constructor (SA) finished!\n");
    }


    IInteraction* Reason() {
      i ++;    
      if (i == 200) {
        printf("Killing myself!\n");
        IsAlive = false;
      }    

      int rnd = (rand() % 5) - 2;
      Data->Movement.X = (float) rnd;
      Data->Movement.Y = (float) rnd;

      return NULL; //new TestInteraction();
    }
};
