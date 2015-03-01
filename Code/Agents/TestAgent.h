#pragma once
#include "SpatialAgent.h"
#include "Reasoning/IAgentLogic.h"
#include "Reasoning/IInteraction.h"
#include <Data/Textures/Texture.h>
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
      Data->Model = new Model3D("models/Cube.m4");
      Data->Model->ScaleModel(0.60f);
      AddToEnvironment();
    }


    IInteraction* Reason() {  
      Data->Heading.X += 1.0f;
      if (Data->Heading.X >= 360.0f) Data->Heading.X = 0.0f;
      return NULL; //new TestInteraction();
    }
};
