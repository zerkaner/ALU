#pragma once
#include <Converter/MemoryStream.h>
#include <Data/Model3D.h>
#include <stdio.h>


/** Loading utility for Blizzard's M2 model format. */
class M2Loader {

  public:

    /** Load a Blizzard M2 model file.
     * @param m2 Model memory stream to parse.
     * @return The loaded model in M4 format. */
    Model3D* LoadM2(MemoryStream* m2);
};
