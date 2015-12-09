#include "Quaternion.h"
#include <Data/Primitives.h>
#include <math.h>

Quaternion::Quaternion(float x, float y, float z, float w) {
  X = x;
  Y = y;
  Z = z;
  W = w;
}


void Quaternion::Normalize() {  
  const float tolerance = 0.00001f;
  float mag = W*W + X*X + Y*Y + Z*Z; // Don't normalize if we don't have to.
  if (fabs(mag) > tolerance && fabs(mag - 1.0f) > tolerance) {
    float length = sqrt(mag);
    W /= length;
    X /= length;
    Y /= length;
    Z /= length;
  }
}


Quaternion Quaternion::GetConjugate() {
  return Quaternion(-X, -Y, -Z, W);
}


Quaternion Quaternion::operator* (const Quaternion& q2) const {
	return Quaternion(W * q2.X   +   X * q2.W   +   Y * q2.Z   -   Z * q2.Y,   // X
	                  W * q2.Y   +   Y * q2.W   +   Z * q2.X   -   X * q2.Z,   // Y
                    W * q2.Z   +   Z * q2.W   +   X * q2.Y   -   Y * q2.X,   // Z
	                  W * q2.W   -   X * q2.X   -   Y * q2.Y   -   Z * q2.Z);  // W
}


Float3 Quaternion::operator* (const Float3& vec) {
  float length = sqrt(vec.X*vec.X + vec.Y*vec.Y + vec.Z*vec.Z);
  Float3 vecN = Float3(vec.X/length, vec.Y/length, vec.Z/length);
  Quaternion vecQuat, resQuat;
  vecQuat.X = vecN.X;
  vecQuat.Y = vecN.Y;
  vecQuat.Z = vecN.Z;
  vecQuat.W = 0.0f;
  resQuat = vecQuat * GetConjugate();
  resQuat = *this * resQuat;
  return Float3(resQuat.X, resQuat.Y, resQuat.Z);
}


void Quaternion::FromAxis(const Float3& vec, float angle) {
  float length = sqrt(vec.X*vec.X + vec.Y*vec.Y + vec.Z*vec.Z);
  Float3 vecN = Float3(vec.X / length, vec.Y / length, vec.Z / length);
  angle *= 0.5f;
  float sinAngle = sinf(angle);
  X = (vecN.X * sinAngle);
  Y = (vecN.Y * sinAngle);
  Z = (vecN.Z * sinAngle);
  W = cosf(angle);
}


void Quaternion::FromEuler(float pitch, float yaw, float roll) {
  // Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll
  // and multiply those together. The calculation below does the same, just shorter.
  const float piOver360 = 0.00872665f;
  float p = pitch * piOver360;
  float y = yaw * piOver360;
  float r = roll * piOver360;
  float sinp = sinf(p);
  float siny = sinf(y);
  float sinr = sinf(r);
  float cosp = cosf(p);
  float cosy = cosf(y);
  float cosr = cosf(r);
  X = sinr * cosp * cosy  -  cosr * sinp * siny;
  Y = cosr * sinp * cosy  +  sinr * cosp * siny;
  Z = cosr * cosp * siny  -  sinr * sinp * cosy;
  W = cosr * cosp * cosy  +  sinr * sinp * siny;
  Normalize();
}


void Quaternion::GetAxisAngle(Float3* axis, float* angle) {
  float scale = sqrt(X*X + Y*Y + Z*Z);
  axis->X = X / scale;
  axis->Y = Y / scale;
  axis->Z = Z / scale;
  *angle = acosf(W) * 2.0f;
}


void Quaternion::GetMatrix(float* matrix) {
  float x2 = X * X;
  float y2 = Y * Y;
  float z2 = Z * Z;
  float xy = X * Y;
  float xz = X * Z;
  float yz = Y * Z;
  float wx = W * X;
  float wy = W * Y;
  float wz = W * Z;

  // This calculation would be a lot more complicated for non-unit length quaternions
  // Note: The constructor of Matrix4 expects the Matrix in column-major format like 
  // expected by OpenGL.
  matrix = new float[16]{
    1.0f - 2.0f * (y2 + z2),   2.0f * (xy - wz),        2.0f * (xz + wy),        0.0f,
    2.0f * (xy + wz),          1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx),        0.0f,
    2.0f * (xz - wy),          2.0f * (yz + wx),        1.0f - 2.0f * (x2 + y2), 0.0f,
    0.0f,                      0.0f,                    0.0f,                    1.0f};
}
