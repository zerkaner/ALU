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
  float X, Y, Z;  
  public:
    Float3() {}
    Float3(float x, float y, float z) {
      X = x;
      Y = y;
      Z = z;
    }
};


/** Float structure for two-dimensional data. */
struct Float2 {
  float X, Y;
  public:
    Float2() {}
    Float2(float x, float y) {
      X = x;
      Y = y;
    }
};


/** Triangular size geometry ("face"). A geometry consists of three coordinate point, 
 *  texture- and normal vector. Because we don't want to store the same things over 
 *  and over again, we just save the indices to access the v/n/t arrays in the geoset. */
struct Geometry {
  bool symIndices; // Symmetrical indices. If 'true', n and t are not used.
  long vIdx[3];    // Indices to vertices (point coordinates).
  long nIdx[3];    // Normal vector indices.
  long tIdx[3];    // Texture slice vector indices.
};


/** Partial structure. A model consists of one or more geosets. It contains the vector
 *  arrays, an index table and a material IS,*/
struct Geoset {
  int id;
  bool enabled;
  int materialID;
  long nrV, nrN, nrT, nrG;
  Float3* vertices;     // Coordinate of a vertex.
  Float3* normals;      // Normal vector that shows a vertex's orientation.
  Float2* textures;     // Vector to the texture slice of a point.  
  Geometry* geometries;
};


/** Material structure, containing a texture and various reflection settings. */
struct Material {
  char  identifier[20];
  float ambientLight[3];
  float diffuseLight[3];
  float specularLight[3];
  long  textureSize;
  char* textureData;
  char  textureName[80];  // Later direct storage.
};



/*
/** Triangle class. Contains three vertices and draw methods.
struct Geometry {
      
  public:
    Vertex* Vertices[3];

    Geometry(Vertex* v1, Vertex* v2, Vertex* v3) {
      Vertices[0] = v1;
      Vertices[1] = v2;
      Vertices[2] = v3;
    }
};

/** 3D vertex, consisting of coordinate point, texture- and normal vector.
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

*/