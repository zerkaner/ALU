#pragma once
#include <Data/Primitives.h>
#include <vector>


/** 3D model class that defines the .mda model format. */
class Model3D {

  public:

    /** An enumeration of available rendering modes. */
    enum RenderingMode {OFF, POINTS, MESH, DIRECT, VBO};

    RenderingMode RenderingMode;      // Current rendering mode.
    std::vector<Geometry*> Triangles; // List with all triangles.


    Model3D() {
    }


    Model3D(const char* filepath) {
    }


    ~Model3D() {
    }


    void LoadFile(const char* filepath) {
    }


    void WriteFile(const char* filepath) {
    }
};