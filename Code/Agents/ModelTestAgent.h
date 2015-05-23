#pragma once
#include "SpatialAgent.h"
#include "Reasoning/IAgentLogic.h"
#include "Reasoning/IInteraction.h"
#include <Data/Textures/Texture.h>
#include <Input/IControllerModule.h>
#include <stdio.h>

/** M4 component / anim test agent. */
class ModelTestAgent : public SpatialAgent, public IAgentLogic, public IControllerModule {

  private:
    int geoset = 0; 

  public:
    ModelTestAgent(World* world, Environment* env, Model3D* model, Vector pos) : SpatialAgent(world, env) {
      RL = this;
      Data->Position = pos;
      Data->Model = model;
      Data->Model->Geosets[0]->enabled = true;
      AddToEnvironment();
    }


    IInteraction* Reason() {  

      Data->Heading.X += 1.0f;
      if (Data->Heading.X >= 360.0f) Data->Heading.X = 0.0f;
      return NULL; //new TestInteraction();
    }


    /** A key was pressed.
    * @param key The pressed key (Int32 enum value).
    * @param mod The used modifier (Ctrl, Shift...). */
    void KeyPressed(Key key, Modifier mod) {
      switch (key) {
        case KEY_SPACE:  
          Data->Model->Geosets[geoset]->enabled = !Data->Model->Geosets[geoset]->enabled; // Invert bit. 
          printf(" %s ", (Data->Model->Geosets[geoset]->enabled)? "enabled" : "disabled");
          break;

        case KEY_N: 
          geoset --;
          if (geoset < 0) geoset = Data->Model->Geosets.size() - 1;
          printf("Switching to geoset %d\n", geoset);
          break;        
        
        case KEY_M: 
          geoset ++;
          if (geoset == Data->Model->Geosets.size()) geoset = 0;
          printf("Switching to geoset %d\n", geoset);
          break;
      }
    }
};
