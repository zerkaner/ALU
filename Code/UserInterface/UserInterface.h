#pragma once

/** The user interface is a 2D overlay on top of the 3D world. */
class UserInterface {


  public:
    void Update() {
      for (int i = 0; i < 2000000; i ++) {
        int j = i % 34;
        j += 7;
      }
    }

};
