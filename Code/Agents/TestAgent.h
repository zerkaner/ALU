#pragma once
#include "SpatialAgent.h"
#include "Reasoning/IAgentLogic.h"
#include "Reasoning/IInteraction.h"
#include <Data/Model.h>
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
    TestAgent(World* world, Environment* env, Float3 pos) : SpatialAgent(world, env) {
      RL = this;
      Data->Position = pos;
      Data->Model = ModelUtils::Load("M4/Cube.m4");
      ModelUtils::ScaleModel(Data->Model, 0.6f);
      AddToEnvironment();
    }


    IInteraction* Reason() {  
      Data->Heading.X += 1.0f;
      if (Data->Heading.X >= 360.0f) Data->Heading.X = 0.0f;
      return NULL; //new TestInteraction();
    }
};
