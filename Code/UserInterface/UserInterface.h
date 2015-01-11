#pragma once
#include <UserInterface/TextWriter.h>
class Camera;


/** The user interface is a 2D overlay on top of the 3D world. */
class UserInterface {

  private:
    TextWriter _writer;
    Camera* _camera;

  public:
    UserInterface(Camera* camera);
    void Update();
};
