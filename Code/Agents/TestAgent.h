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
        Data->Model = new Model3D("models/Cat.m4");
        Data->Model->ScaleModel(0.04f);
      }
      else { 
        Data->Model = new Model3D("models/Cube.m4");
        Data->Model->ScaleModel(0.60f);
      }
      Data->Model->RenderingMode = Model3D::DIRECT;

      AddToEnvironment();
      printf("[TestAgent] Constructor (SA) finished!\n");
    }


    IInteraction* Reason() { 
      if (Ticks == 200) {
        //printf("Killing myself!\n");
        //IsAlive = false;
      }    
      Data->Heading.X += 1.0f;
      if (Data->Heading.X >= 360.0f) Data->Heading.X = 0.0f;
      return NULL; //new TestInteraction();
    }
};
