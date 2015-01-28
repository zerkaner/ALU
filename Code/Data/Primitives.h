#pragma once

/** Type definitions. */
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned int   uint;


/** Character structure. */
struct Character {
	int width;
	int height;
	float tx1, ty1;
	float tx2, ty2;
};


/** Font structure. */
struct Font {
	unsigned int Texture;
	int Width, Height;
	int Start, End;
	Character* Characters;
};


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



/** Triangle class. Contains three vertices and draw methods. */
struct Geometry {
      
  public:
    Vertex* Vertices[3];

    Geometry(Vertex* v1, Vertex* v2, Vertex* v3) {
      Vertices[0] = v1;
      Vertices[1] = v2;
      Vertices[2] = v3;
    }
};
