#pragma once
#include <stdio.h>
#include <Data/Primitives.h>


void test() {
  printf("test fkt.\n");
}

// http://content.gpwiki.org/index.php/OpenGL:Tutorials:Basic_Bones_System
namespace AnimTest {


  struct Bone {
    int id;        // Bone identifier.
    Float3 pos1;   // Position value 1.
    Float3 pos2;   // 2nd position.
    Bone* parent;  // Parent bone. 'null' on root. 
    Bone* childs;  // Pointer to child nodes.
    int nrChilds;  // Number of child nodes.
    //TODO Vertex list.
  };

  struct Keyframe {
  };

  struct Animation {
  };

};



class AnimationManager {



};