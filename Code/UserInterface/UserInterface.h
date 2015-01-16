#pragma once
#include <UserInterface/TextWriter.h>


/** The user interface is a 2D overlay on top of the 3D world. */
class UserInterface {

  private:
    TextWriter _writer;

  public:
    UserInterface();
    void Update();
};
