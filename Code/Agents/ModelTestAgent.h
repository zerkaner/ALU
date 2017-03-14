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
    ModelTestAgent(World* world, Environment* env, Model* model, Float3 pos) : SpatialAgent(world, env) {
      RL = this;
      Data->Position = pos;
      Data->Model3D = model;
      Data->Model3D->_renderMode = 2;
      AddToEnvironment();
    }


    IInteraction* Reason() {
      //Data->Heading.X += 1.0f;
      if (Data->Heading.X >= 360.0f) Data->Heading.X = 0.0f;
      return NULL;
    }


    /** A key was pressed.
     * @param key The pressed key (Int32 enum value).
     * @param mod The used modifier (Ctrl, Shift...). */
    void KeyPressed(Key key, Modifier mod) {
      switch (key) {
        case KEY_SPACE:
          // Invert bit.
          Data->Model3D->Meshes[_selectedMesh].Enabled = !Data->Model3D->Meshes[_selectedMesh].Enabled;
          printf(" %s ", (Data->Model3D->Meshes[_selectedMesh].Enabled) ? "enabled" : "disabled");
          break;

        case KEY_N:
          _selectedMesh --;
          if (_selectedMesh < 0) _selectedMesh = Data->Model3D->Meshes.size() - 1;
          printf("\nGeoset %d [%s].", _selectedMesh, Data->Model3D->Meshes[_selectedMesh].ID);
          break;

        case KEY_M:
          _selectedMesh ++;
          if (_selectedMesh == Data->Model3D->Meshes.size()) _selectedMesh = 0;
          printf("\nGeoset %d [%s].", _selectedMesh, Data->Model3D->Meshes[_selectedMesh].ID);
          break;

        case KEY_R:
          Data->Model3D->_renderMode ++;
          if (Data->Model3D->_renderMode == 5) Data->Model3D->_renderMode = 0;
          break;
      }
    }
};
