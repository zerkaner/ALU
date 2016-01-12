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
    int _selectedMesh = 0;

  public:
    ModelTestAgent(World* world, Environment* env, Model2* model, Float3 pos) : SpatialAgent(world, env) {
      RL = this;
      Data->Position = pos;
      Data->Model = model;
      Data->Model->_renderMode = 2;
      AddToEnvironment();
    }


    IInteraction* Reason() {  
      Data->Heading.X += 1.0f;
      if (Data->Heading.X >= 360.0f) Data->Heading.X = 0.0f;
      return NULL;
    }


    /** A key was pressed.
    * @param key The pressed key (Int32 enum value).
    * @param mod The used modifier (Ctrl, Shift...). */
    void KeyPressed(Key key, Modifier mod) {
      switch (key) {
        case KEY_SPACE:
          Data->Model->Meshes[_selectedMesh].Enabled = !Data->Model->Meshes[_selectedMesh].Enabled; // Invert bit. 
          printf(" %s ", (Data->Model->Meshes[_selectedMesh].Enabled) ? "enabled" : "disabled");
          break;

        case KEY_N:
          _selectedMesh --;
          if (_selectedMesh < 0) _selectedMesh = Data->Model->Meshes.size() - 1;
          printf("\nGeoset %d [%d].", _selectedMesh, Data->Model->Meshes[_selectedMesh].ID);
          break;

        case KEY_M:
          _selectedMesh ++;
          if (_selectedMesh == Data->Model->Meshes.size()) _selectedMesh = 0;
          printf("\nGeoset %d [%d].", _selectedMesh, Data->Model->Meshes[_selectedMesh].ID);
          break;

        case KEY_R:
          Data->Model->_renderMode ++;
          if (Data->Model->_renderMode == 4) Data->Model->_renderMode = 0;
          break;
      }
    }
};
