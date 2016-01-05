#pragma once

#include <Data/Primitives.h>

struct Matrix4x4f {
	float m[16];
	
  Float3 operator*(const Float3& right);
	Matrix4x4f operator*(const Matrix4x4f& right);
	Matrix4x4f& operator*=(const Matrix4x4f& right);
	
	void loadIdentity();
	
  void loadTranslation(Float3& t);
  void loadQuaternionRotation(Float4& r);
  void loadScale(Float3& s);
	
  void translate(Float3& t);
	void quaternionRotate(Float4& r);
  void scale(Float3& s);
};

// phasing these out in favor of the above structure

void mat4d_LoadTranslation(double* t, double* mout);
void mat4d_LoadLookAt(double *f, double* u, double* r, double* mout);
void mat4d_LoadPerspective(double fov_radians, double aspect, double zNear, double zFar, double* mout);
void mat4d_LoadOrtho(double left, double right, double bottom, double top, double near, double far, double* mout);
void mat4d_MultiplyMat4d(double* a, double* b, double* mout);
