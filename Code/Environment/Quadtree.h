#pragma once
#include <Visualization/Model3D.h>
#include <vector>


/** Quad tree data structure to partition a two-dimensional space. */
class Quadtree {

  /*  ┌─────┬─────┐  Default
   *  │ II. │  I. │  quadrant
   *  ├─────┼─────┤  enumeration
   *  | III.│ IV. │  (counter clockwise).
   *  └─────┴─────┘  Array: 0-3.
   */

  private:

  public:
    // TBC ...
    std::vector<Model3D*> Objects;

};
