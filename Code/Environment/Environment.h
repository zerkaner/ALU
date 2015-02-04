#pragma once
#include "Cube.h"
#include "GridTerrain.h"
#include "HeightmapTerrain.h"
#include <Visualization/IDrawable.h>


/** 3D environment representation. */
class Environment : public IDrawable {

  private:
    IDrawable* _terrain;
    Cube* _obj;

  public:

    Environment() : 
      //_terrain(new HeightmapTerrain("heightfield2.raw")) {
      _terrain(new GridTerrain(10, 10)) {
      _obj = new Cube(Float3(0.5,0.5,0.5));
    }


    /** Allows the execution of environment related functions. */
    void AdvanceEnvironment() {
      _obj->Position.X += 0.01;
      _obj->Position.Y += 0.01;
      _obj->Position.Z += 0.002;
      _obj->Heading.X += 1;
    }


    /** Draw the environment. Calls the subroutines of the terrain and all objects. */
    void Draw() {
      _terrain->Draw();
      _obj->Draw();
    }
};
