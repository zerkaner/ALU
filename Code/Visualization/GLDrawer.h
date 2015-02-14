#pragma once
class Object3D;


/** GL rendering code for 3D objects. */
class GLDrawer {

  public:

    /** Draws this model. */
    static void Draw(Object3D* obj);
};
