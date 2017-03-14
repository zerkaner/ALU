#pragma once
#include "../IControllerModule.h"
#include <Data/Object3D.h>
#include <vector>


/** Toggles the rendering mode for all objects. Mainly intended for engine debugging. */
class RenderModeController : public IControllerModule {

private:
  std::vector<Object3D*>* _worldObjects;


public:

  /** Create a display mode controller.
   * @param worldObjects Reference to world object vector. */
  RenderModeController(std::vector<Object3D*>* worldObjects) {
    _worldObjects = worldObjects;
  }


  /** A key was pressed.
   * @param key The pressed key (Int32 enum value).
   * @param mod The used modifier (Ctrl, Shift...). */
  void KeyPressed(Key key, Modifier mod) {
    if (mod == NONE) {
      int dpMode = -1;
      switch (key) {
        case KEY_0: dpMode = 0; break;
        case KEY_1: dpMode = 1; break;
        case KEY_2: dpMode = 2; break;
        case KEY_3: dpMode = 3; break;
        case KEY_4: dpMode = 4; break;
      }
      if (dpMode != -1) {
        for (unsigned int i = 0; i < _worldObjects->size(); i ++) {
          Model* model = (*_worldObjects)[i]->Model3D;
          if (model != NULL) model->_renderMode = dpMode;
        }
      }
    }
  }
};
