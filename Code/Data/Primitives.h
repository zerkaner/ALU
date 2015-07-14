#pragma once


/** Type definitions. */
typedef unsigned char  BYTE;      // 8 bit.
typedef unsigned short WORD;      // 16 bit.
typedef unsigned long DWORD;      // 32 bit [Langwort].
typedef unsigned long long QWORD; // 64 bit [Quadwort].
typedef unsigned int uint;


/** Character structure. */
struct Character {
	int width;
	int height;
	float tx1, ty1;
	float tx2, ty2;
};


/** Font structure. */
struct Font {
	unsigned int TextureID;
	int Width, Height;
	int Start, End;
	Character* Characters;
};


/* Four-dimensional float structure (e.g. for RGBA). */
struct Float4 {
  float X, Y, Z, W;
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
  int textureID;            // Texture of this geoset.
  long nrV, nrN, nrT, nrG;  // Number of vertices, normals, texture vectors and geometries.
  Float3* vertices;         // Coordinate of a vertex.
  Float3* normals;          // Normal vector that shows a vertex's orientation.
  Float2* texVects;         // Vectors to the texture slice of a point.  
  Geometry* geometries;     // Geoset geometries.
};


/** The mesh holds a number of geosets and thereby constitutes the look of the model. */
struct Mesh {
  int nrElements;    // Number of elements.
  Geoset** geosets;  // Geoset pointer array.
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



//TODO Work-in-Progress (Blizzard MDX data).

struct AnimSet {
  int Size;             // Number of values (array sizes). 
  short Interpolation;  // Interpolation type (0: none, 1: linear, 2: Hermite, 3: Bezier).
  int GlobalSeqID;      // Global sequence ID.
  long* Time;           // Timestep in animation.
  Float4* Values;       // Value: 3D vector (with bank angle on 4th for rotation spec [= quaternion]). 
  Float4* InTan;        //| Angles used for Hermite and 
  Float4* OutTan;       //| Bezier interpolation.
};


struct Bone {
  char Name[80];
  int ID;
  int ParentID;
  int GeosetID;
  AnimSet* Translation = 0;
  AnimSet* Rotation    = 0;
  AnimSet* Scaling     = 0;
};


struct Sequence {
  char Name[80];
  long IntervalStart;
  long IntervalEnd;
  bool Loop;  
  float MoveSpeed;
  float BoundsRadius;
  Float3 MinimumExtent;
  Float3 MaximumExtent;
};




/** Bone element. A bone hierarchy is used to express a model's skeleton. */
struct Bone_t {
  int ID;            // Sequential bone identifier to access transformation arrays.  
  char* Name;        // Bone name, maybe useful for text-based storage.
  Bone_t* Parent;    // Parent bone.
  Bone_t** Children; // Child bones following in the hierarchy below this one.
  long* VertexIdx;   // Array of vertices (indices) that are associated to this bone.
  uint NrChildren;   // Number of child bones.
  uint NrVertices;   // Number of vertex indices.
  //TODO Maybe min/max extents and bone position?
};


/** Transformation specification. Defines modifications to apply to a bone and its subnodes. */
struct Transformation {
  uint Frame;          // The frame this transformation belongs to. 
  Bone_t* Bone;        // Parent bone. 
  Float3 Translation;  //| Transformation
  Float4 Rotation;     //| directives to
  Float3 Scaling;      //| apply to bone.
  //TODO Flags to signalize used/unused directives.
};


/** Animation structure. Consists of a number of transformation rules to apply. */
struct Animation {
  char* Name;                       // Animation identifier.
  uint FrameCount;                  // Length of animation (number of frames). 
  int* NrTransformations;           // Number of transformations per bone.
  Transformation** Transformations; // [Bone][Transformation] double array pointer.
};
