#pragma once
#include "GridTerrain.h"
#include "HeightmapTerrain.h"
#include "Quadtree.h"
#include <Visualization/IDrawable.h>


/** 3D environment representation. */
class Environment : public IDrawable {

  private:
    IDrawable* _terrain;

  public:

    Quadtree Quadtree;  // Quadtree to store objects in this environment.


    Environment() { 
      //_terrain = new HeightmapTerrain("heightfield2.raw"); 
      _terrain = new GridTerrain(10, 10);

    }


    /** Allows the execution of environment related functions. */
    void AdvanceEnvironment() {
    }


    /** Draw the environment. Calls the subroutines of the terrain and all objects. */
    void Draw() {
      _terrain->Draw();
      for (unsigned int i = 0; i < Quadtree.Objects.size(); i ++) Quadtree.Objects[i]->Draw();
    }
};
