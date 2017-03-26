#pragma once
#include <math.h>


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
  Float4() {}
  Float4(float x, float y, float z, float w) {
    X = x;  Y = y;  Z = z;  W = w;
  }
  void operator+=(Float4 right) {
    X += right.X;
    Y += right.Y;
    Z += right.Z;
    W += right.W;
  }
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
    Float3 operator*(float val) {
      return Float3(X * val, Y * val, Z * val);
    }
    float Distance(Float3 other) {
      return sqrtf(
        (X - other.X)*(X - other.X) +
        (Y - other.Y)*(Y - other.Y) +
        (Z - other.Z)*(Z - other.Z));
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
