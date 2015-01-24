#pragma once

#include <Visualization/IDrawable.h>
#include <SDL_opengl.h>

class GridTerrain : public IDrawable {

  private:

    int _width;   // Width of grid (x value).
    int _height;  // Height of grid (y value).

  public:
  
    GridTerrain(int width, int height) {
      _width = width;
      _height = height;
    }


    /** Draw the grid tiles. */
    void Draw() {
      glBegin(GL_LINES);
      for (int i = 0; i <= _height; i ++) {
        glVertex3i(0, i, 0);
        glVertex3i(_width, i, 0);
      }
      for (int i = 0; i <= _width; i++) {
        glVertex3i(i, 0, 0);
        glVertex3i(i, _height, 0);
      }
      glEnd();
    }
};
