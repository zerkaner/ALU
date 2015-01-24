#pragma once
#include "GridTerrain.h"
#include <Visualization/IDrawable.h>


/** 3D environment representation. */
class Environment : public IDrawable {

  private:
    GridTerrain _terrain;

  public:

    Environment() : 
      _terrain(GridTerrain(30, 20)) {
    }


    /** Allows the execution of environment related functions. */
    void AdvanceEnvironment() {
    }


    /** Draw the environment. Calls the subroutines of the terrain and all objects. */
    void Draw() {
      _terrain.Draw();
    }
};
