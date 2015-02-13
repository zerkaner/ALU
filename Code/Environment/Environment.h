#pragma once
#include "GridTerrain.h"
#include "HeightmapTerrain.h"
#include "Quadtree.h"
#include <Visualization/IDrawable.h>
#include <Visualization/GLDrawer.h>
#include <stdio.h>


/** 3D environment representation. */
class Environment : public IDrawable {

  private:
    IDrawable* _terrain;

  public:

    Quadtree Objects;  // Quadtree to store objects in this environment.


    Environment() { 
      //_terrain = new HeightmapTerrain("heightfield2.raw"); 
      _terrain = new GridTerrain(10, 10);

    }


    /** Allows the execution of environment related functions. */
    void AdvanceEnvironment() {
    }


    /** Add an object to the environment.
     * @param obj The object to add. */
    void AddObject(Object3D* obj) {
      Objects.Objects.push_back(obj);
    }


    /** Remove an object from the environment.
     * @param obj The object to remove. */
    void RemoveObject(Object3D* obj) {
      //TODO ...
    }


    /** Draw the environment. Calls the subroutines of the terrain and all objects. */
    void Draw() {
      _terrain->Draw();
      for (unsigned int i = 0; i < Objects.Objects.size(); i ++) {
        GLDrawer::Draw(Objects.Objects[i]);
      }
    }
};
