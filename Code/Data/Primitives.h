#pragma once

/** Type definitions. */
typedef unsigned char  BYTE;  // 8 bit.
typedef unsigned short WORD;  // 16 bit.
typedef unsigned long DWORD;  // 32 bit.
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
    void operator+=(Float3 right) {
      X += right.X;
      Y += right.Y; 
      Z += right.Z;
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
