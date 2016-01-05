#include "matrix.h"

#include <string.h>
#include <math.h>

//	0	4	 8 12
//	1	5	 9 13
//	2	6	10 14
//	3	7	11 15

Float3 Matrix4x4f::operator*(const Float3& right) {
  return Float3(m[0] * right.X + m[4] * right.Y + m[8] * right.Z + m[12],
	                m[1] * right.X + m[5] * right.Y + m[9]  * right.Z + m[13],
	                m[2] * right.X + m[6] * right.Y + m[10] * right.Z + m[14]
	               );
}

Matrix4x4f Matrix4x4f::operator*(const Matrix4x4f& right) {
	Matrix4x4f mout;
	
	mout.m[0]  = m[0] * right.m[0]  + m[4] * right.m[1]  + m[8] * right.m[2]   + m[12] * right.m[3];
	mout.m[1]  = m[1] * right.m[0]  + m[5] * right.m[1]  + m[9] * right.m[2]   + m[13] * right.m[3];
	mout.m[2]  = m[2] * right.m[0]  + m[6] * right.m[1]  + m[10] * right.m[2]  + m[14] * right.m[3];
	mout.m[3]  = m[3] * right.m[0]  + m[7] * right.m[1]  + m[11] * right.m[2]  + m[15] * right.m[3];

	mout.m[4]  = m[0] * right.m[4]  + m[4] * right.m[5]  + m[8] * right.m[6]   + m[12] * right.m[7];
	mout.m[5]  = m[1] * right.m[4]  + m[5] * right.m[5]  + m[9] * right.m[6]   + m[13] * right.m[7];
	mout.m[6]  = m[2] * right.m[4]  + m[6] * right.m[5]  + m[10] * right.m[6]  + m[14] * right.m[7];
	mout.m[7]  = m[3] * right.m[4]  + m[7] * right.m[5]  + m[11] * right.m[6]  + m[15] * right.m[7];

	mout.m[8]  = m[0] * right.m[8]  + m[4] * right.m[9]  + m[8] * right.m[10]  + m[12] * right.m[11];
	mout.m[9]  = m[1] * right.m[8]  + m[5] * right.m[9]  + m[9] * right.m[10]  + m[13] * right.m[11];
	mout.m[10] = m[2] * right.m[8]  + m[6] * right.m[9]  + m[10] * right.m[10] + m[14] * right.m[11];
	mout.m[11] = m[3] * right.m[8]  + m[7] * right.m[9]  + m[11] * right.m[10] + m[15] * right.m[11];

	mout.m[12] = m[0] * right.m[12] + m[4] * right.m[13] + m[8] * right.m[14]  + m[12] * right.m[15];
	mout.m[13] = m[1] * right.m[12] + m[5] * right.m[13] + m[9] * right.m[14]  + m[13] * right.m[15];
	mout.m[14] = m[2] * right.m[12] + m[6] * right.m[13] + m[10] * right.m[14] + m[14] * right.m[15];
	mout.m[15] = m[3] * right.m[12] + m[7] * right.m[13] + m[11] * right.m[14] + m[15] * right.m[15];
	
	return mout;
}

Matrix4x4f& Matrix4x4f::operator*=(const Matrix4x4f& right) {
	*this = *this * right;
	return *this;
}

void Matrix4x4f::loadIdentity() {
	m[0] = m[5] = m[10] = m[15] = 1.0;
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0;
}

void Matrix4x4f::loadTranslation(Float3& t) {
	m[0] = m[5] = m[10] = m[15] = 1.0;
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = 0.0;
	m[12] = t.X;
	m[13] = t.Y;
	m[14] = t.Z;
}

void Matrix4x4f::loadQuaternionRotation(Float4& r) {
	m[0]  = 1.0 - 2.0 * r.Y * r.Y - 2.0 * r.Z * r.Z;
	m[4]  = 2.0 * r.X * r.Y + 2.0 * r.W * r.Z;
	m[8]  = 2.0 * r.X * r.Z - 2.0 * r.W * r.Y;
	m[1]  = 2.0 * r.X * r.Y - 2.0 * r.W * r.Z;
	m[5]  = 1.0 - 2.0 * r.X * r.X - 2.0 * r.Z * r.Z;
	m[9]  = 2.0 * r.Y * r.Z + 2.0 * r.W * r.X;
	m[2]  = 2.0 * r.X * r.Z + 2.0 * r.W * r.Y;
	m[6]  = 2.0 * r.Y * r.Z - 2.0 * r.W * r.X;
	m[10] = 1.0 - 2.0 * r.X * r.X - 2.0 * r.Y * r.Y;
	m[12] = m[13] = m[14] = m[3] = m[7] = m[11] = 0.0;
	m[15] = 1.0;
}

void Matrix4x4f::loadScale(Float3& s) {
	m[0]  = s.X;
	m[5]  = s.Y;
	m[10] = s.Z;
	m[15] = 1.0;
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0;
}

void Matrix4x4f::translate(Float3& t) {
	Matrix4x4f temp;
	temp.loadTranslation(t);
	*this *= temp;
}
	
void Matrix4x4f::quaternionRotate(Float4& r) {
	Matrix4x4f temp;
	temp.loadQuaternionRotation(r);
	*this *= temp;
}

void Matrix4x4f::scale(Float3& s) {
	Matrix4x4f temp;
	temp.loadScale(s);
	*this *= temp;
}
	
void mat4d_LoadTranslation(double* v, double* mout) {
	mout[0] = 1.0;
	mout[1] = 0.0;
	mout[2] = 0.0;
	mout[3] = 0.0;
	
	mout[4] = 0.0;
	mout[5] = 1.0;
	mout[6] = 0.0;
	mout[7] = 0.0;
	
	mout[8] = 0.0;
	mout[9] = 0.0;
	mout[10] = 1.0;
	mout[11] = 0.0;	
	
	mout[12] = v[0];
	mout[13] = v[1];
	mout[14] = v[2];
	mout[15] = 1.0;
}

void mat4d_LoadLookAt(double *f, double* u, double* r, double* mout) {
	mout[0] = r[0];
	mout[1] = u[0];
	mout[2] = -f[0];
	mout[3] = 0.0;

	mout[4] = r[1];
	mout[5] = u[1];
	mout[6] = -f[1];
	mout[7] = 0.0;

	mout[8] = r[2];
	mout[9] = u[2];
	mout[10] = -f[2];
	mout[11] = 0.0;

	mout[12] = 0.0;
	mout[13] = 0.0;
	mout[14] = 0.0;
	mout[15] = 1.0;
}

void mat4d_LoadPerspective(double fov_radians, double aspect, double zNear, double zFar, double* mout)
{
	double d = 1.0 / tan(fov_radians / 2.0);
	
	mout[0] = d / aspect;
	mout[1] = 0.0;
	mout[2] = 0.0;
	mout[3] = 0.0;
	
	mout[4] = 0.0;
	mout[5] = d;
	mout[6] = 0.0;
	mout[7] = 0.0;
	
	mout[8] = 0.0;
	mout[9] = 0.0;
	mout[10] = (zFar+zNear) / (zNear-zFar);
	mout[11] = -1.0;
	
	mout[12] = 0.0;
	mout[13] = 0.0;
	mout[14] = 2 * zFar * zNear / (zNear-zFar);
	mout[15] = 0.0;
}

void mat4d_LoadOrtho(double left, double right, double bottom, double top, double near, double far, double* mout)
{
	double r_l = right - left;
	double t_b = top - bottom;
	double f_n = far - near;
	double tx = - (right + left) / (right - left);
	double ty = - (top + bottom) / (top - bottom);
	double tz = - (far + near) / (far - near);

	mout[0] = 2.0f / r_l;
	mout[1] = 0.0f;
	mout[2] = 0.0f;
	mout[3] = 0.0f;
	
	mout[4] = 0.0f;
	mout[5] = 2.0f / t_b;
	mout[6] = 0.0f;
	mout[7] = 0.0f;
	
	mout[8] = 0.0f;
	mout[9] = 0.0f;
	mout[10] = -2.0f / f_n;
	mout[11] = 0.0f;
	
	mout[12] = tx;
	mout[13] = ty;
	mout[14] = tz;
	mout[15] = 1.0f;
}

void mat4d_MultiplyMat4d(double* a, double* b, double* mout)
{
	double ta[16], tb[16];
	memcpy(ta, a, 16 * sizeof(double));
	memcpy(tb, b, 16 * sizeof(double));
	
	mout[0]  = ta[0] * tb[0]  + ta[4] * tb[1]  + ta[8] * tb[2]   + ta[12] * tb[3];
	mout[1]  = ta[1] * tb[0]  + ta[5] * tb[1]  + ta[9] * tb[2]   + ta[13] * tb[3];
	mout[2]  = ta[2] * tb[0]  + ta[6] * tb[1]  + ta[10] * tb[2]  + ta[14] * tb[3];
	mout[3]  = ta[3] * tb[0]  + ta[7] * tb[1]  + ta[11] * tb[2]  + ta[15] * tb[3];

	mout[4]  = ta[0] * tb[4]  + ta[4] * tb[5]  + ta[8] * tb[6]   + ta[12] * tb[7];
	mout[5]  = ta[1] * tb[4]  + ta[5] * tb[5]  + ta[9] * tb[6]   + ta[13] * tb[7];
	mout[6]  = ta[2] * tb[4]  + ta[6] * tb[5]  + ta[10] * tb[6]  + ta[14] * tb[7];
	mout[7]  = ta[3] * tb[4]  + ta[7] * tb[5]  + ta[11] * tb[6]  + ta[15] * tb[7];

	mout[8]  = ta[0] * tb[8]  + ta[4] * tb[9]  + ta[8] * tb[10]  + ta[12] * tb[11];
	mout[9]  = ta[1] * tb[8]  + ta[5] * tb[9]  + ta[9] * tb[10]  + ta[13] * tb[11];
	mout[10] = ta[2] * tb[8]  + ta[6] * tb[9]  + ta[10] * tb[10] + ta[14] * tb[11];
	mout[11] = ta[3] * tb[8]  + ta[7] * tb[9]  + ta[11] * tb[10] + ta[15] * tb[11];

	mout[12] = ta[0] * tb[12] + ta[4] * tb[13] + ta[8] * tb[14]  + ta[12] * tb[15];
	mout[13] = ta[1] * tb[12] + ta[5] * tb[13] + ta[9] * tb[14]  + ta[13] * tb[15];
	mout[14] = ta[2] * tb[12] + ta[6] * tb[13] + ta[10] * tb[14] + ta[14] * tb[15];
	mout[15] = ta[3] * tb[12] + ta[7] * tb[13] + ta[11] * tb[14] + ta[15] * tb[15];
}