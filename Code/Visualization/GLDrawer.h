#pragma once
class Object3D;
class Model3D;


/** GL rendering code for 3D objects. */
class GLDrawer {

  public:

    /** Draws an object's model at the appropriate position.
     * @param obj Object reference. */
    static void Draw(Object3D* obj);
};
