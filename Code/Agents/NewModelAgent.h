#pragma once
#include "SpatialAgent.h"
#include "Reasoning/IAgentLogic.h"
#include "Reasoning/IInteraction.h"
#include <Data/Textures/Texture.h>
#include <Input/IControllerModule.h>
#include <stdio.h>

/** Second model test agent. */
class NewModelAgent : public SpatialAgent, public IAgentLogic, public IControllerModule {

private:
  Model2* _model;
  int _selectedMesh = 0;

public:
  NewModelAgent(World* world, Environment* env, Model2* model) : SpatialAgent(world, env) {
    RL = this;
    _model = model;
    env->AddModel2(_model);
  }


  IInteraction* Reason() {
    //Data->Heading.X += 1.0f;
    //if (Data->Heading.X >= 360.0f) Data->Heading.X = 0.0f;
    return NULL;
  }


  /** A key was pressed.
  * @param key The pressed key (Int32 enum value).
  * @param mod The used modifier (Ctrl, Shift...). */
  void KeyPressed(Key key, Modifier mod) {
    switch (key) {
      case KEY_SPACE:
        _model->Meshes[_selectedMesh]._enabled = !_model->Meshes[_selectedMesh]._enabled; // Invert bit. 
        printf(" %s ", (_model->Meshes[_selectedMesh]._enabled) ? "enabled" : "disabled");
        break;

      case KEY_N:
        _selectedMesh --;
        if (_selectedMesh < 0) _selectedMesh = _model->Meshes.size() - 1;
        printf("\nGeoset %d [%d].", _selectedMesh, _model->Meshes[_selectedMesh].ID);
        break;

      case KEY_M:
        _selectedMesh ++;
        if (_selectedMesh == _model->Meshes.size()) _selectedMesh = 0;
        printf("\nGeoset %d [%d].", _selectedMesh, _model->Meshes[_selectedMesh].ID);
        break;

      case KEY_R:
        _model->_renderMode ++;
        if (_model->_renderMode == 4) _model->_renderMode = 0;
        break;
    }
  }
};
