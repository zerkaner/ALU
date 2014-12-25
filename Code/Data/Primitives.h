#pragma once


/** Float structure for three-dimensional data. */
struct Float3 {
    
  public:
    float X, Y, Z;

    Float3() {}

    Float3(float x, float y, float z) {
      X = x;
      Y = y;
      Z = z;
    }
};



/** Float structure for two-dimensional data. */
struct Float2 {
  
  public:
    float X, Y;
  
    Float2() {}

    Float2(float x, float y) {
      X = x;
      Y = y;
    }
};



/** 3D vertex, consisting of coordinate point, texture- and normal vector. */
struct Vertex {

  public:
    Float3 Point;    // Coordinate of this vertex.
    Float2 Texture;  // Vector to the texture slice of this point.  
    Float3 Normal;   // Normal vector that shows this vertex' orientation.

  
    Vertex(Float3 point) {
      Point = point;
      Texture = Float2();
      Normal = Float3();
    }

    Vertex(Float3 point, Float2 texture) {
      Point = point;
      Texture = texture;
      Normal = Float3();
    }

    Vertex(Float3 point, Float2 texture, Float3 normal) {
      Point = point;
      Texture = texture;
      Normal = normal;
    }
};
