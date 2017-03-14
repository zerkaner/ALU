#pragma once
#include "../IControllerModule.h"
#include <Visualization/Engine.h>


/** This controller is intended for FPS-like camera movement. */
class EngineController : public IControllerModule {

  private:
    Engine* _engine;    // Reference to 3D engine.


  public:

    /** Create a controller for the 3D engine.
     * @param engine 3D engine reference. */
    EngineController(Engine* engine) {
      _engine = engine;
    }


    /** A key was pressed.
     * @param key The pressed key (Int32 enum value).
     * @param mod The used modifier (Ctrl, Shift...). */
    void KeyPressed(Key key, Modifier mod) {
      if (mod == ALT && key == KEY_ENTER) _engine->SetResolution(-1, -1, true);
      if (mod == ALT && key == KEY_1)     _engine->SetResolution(640, 480);
      if (mod == ALT && key == KEY_2)     _engine->SetResolution(800, 600);
      if (mod == ALT && key == KEY_3)     _engine->SetResolution(1024,600);
    }
};
