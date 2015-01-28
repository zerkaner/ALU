#pragma once
#include "Cube.h"
#include "GridTerrain.h"
#include "HeightmapTerrain.h"
#include <Visualization/IDrawable.h>


/** 3D environment representation. */
class Environment : public IDrawable {

  private:
    IDrawable* _terrain;
    IDrawable* _obj;

  public:

    Environment() : 
      _terrain(new HeightmapTerrain("heightfield2.raw")) {
      //_terrain(new GridTerrain(30, 20)) {
      //_obj = new Cube(Float3(0,0,0));
    }


    /** Allows the execution of environment related functions. */
    void AdvanceEnvironment() {
    }


    /** Draw the environment. Calls the subroutines of the terrain and all objects. */
    void Draw() {
      _terrain->Draw();
//      _obj->Draw();
    }
};
