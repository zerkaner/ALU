#pragma once
#include <stdio.h>
#include <Data/Primitives.h>


void test() {
  printf("test fkt!\n");
}


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
};

