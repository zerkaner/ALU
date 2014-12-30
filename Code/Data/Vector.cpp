#include "Vector.h"

#include <cmath>
#include <limits>
#include <stdio.h>
#include <stdlib.h>

using namespace std;


/** Initialize a two-dimensional vector (height is set to zero). */
Vector::Vector (float x, float y) {
  X = x;
  Y = y;
  Z = 0.0f;
  _is3D = false;
}


/** Initialize a three-dimensional vector. */
Vector::Vector (float x, float y, float z) {
  X = x;
  Y = y;
  Z = z;
  _is3D = true;
}


/** Destroy this vector. */
Vector::~Vector() {
  // Nothing to free here!
}


/** Calculate point-to-point distance.
 * @param pos The target point.
 * @return Euclidian distance value. */
float Vector::GetDistance(Vector pos) {
  return sqrt((X - pos.X)*(X - pos.X) +
              (Y - pos.Y)*(Y - pos.Y) +
              (Z - pos.Z)*(Z - pos.Z));      
}


/** Calculate the vector length.
 * @return Length of this vector. */
float Vector::GetLength() {
  return GetDistance(Vector(0.0f, 0.0f, 0.0f));
}


/** Calculate the normalized vector.
 * @return The normalized vector. */
Vector Vector::GetNormalVector() {
  float length = GetLength();
  return Vector(X/length, Y/length, Z/length);
}


/** Create normalized vectors orthogonal to this one.
 * @param nY Pointer for new y-axis normal vector.
 * @param nZ Same for z-axis (height) vector. */
void Vector::GetPlanarOrthogonalVectors(Vector& nY, Vector& nZ) {

  // [Y-Axis]: Create orthogonal vector to new x-axis laying in plane (x, y):
  // => Scalar product = 0.
  nY = Vector(-Y/X, 1.0f, 0.0f).GetNormalVector();

  // [Z-Axis / Height]: Build orthogonal vector with cross-product.
  float x3 = (Y * nY.Z  -  Z * nY.Y);  // x: a2b3 - a3b2
  float y3 = (Z * nY.X  -  X * nY.Z);  // y: a3b1 - a1b3
  float z3 = (X * nY.Y  -  Y * nY.X);  // z: a1b2 - a2b1
  nZ = Vector(x3, y3, z3).GetNormalVector();
}


/** Output the position.
 * @return String with component-based notation. */
char* Vector::ToString() {
  char* str = (char*) malloc(sizeof(char) * 80);
  if (_is3D) sprintf(str, "(%5.2f|%5.2f|%5.2f)", X,Y,Z);
  else       sprintf(str, "(%5.2f|%5.2f)"      , X,Y);       
  return str;
}


/** Compare this with another Vector object for equality.
 * @param vector Comparison vector.
 * @return Equals boolean. */
bool Vector::Equals(Vector vector) {
  return (fabs(X - vector.X) <= numeric_limits<float>::epsilon() &&
          fabs(Y - vector.Y) <= numeric_limits<float>::epsilon() &&
          fabs(Z - vector.Z) <= numeric_limits<float>::epsilon());
}


/** Calculate the dot (scalar) product of two vectors.
 * @param a First vector.
 * @param b Second vector.
 * @return The scalar [inner] product. */
float Vector::DotProduct(Vector a, Vector b) {
  return a.X*b.X + a.Y*b.Y + a.Z*b.Z;
}


/** Calcute the cross product of two [3D!] vectors.
 * @param a First vector.
 * @param b Second vector.
 * @return The cross [outer] product. */
Vector Vector::CrossProduct(Vector a, Vector b) {
  return Vector(a.Y*b.Z - a.Z*b.Y, a.Z*b.X - a.X*b.Z, a.X*b.Y - a.Y*b.X);
}


/** Addition with another vector.
 * @param right Second (right) vector.
 * @return Sum of both vectors. */
Vector Vector::operator+(Vector right) {
  if (_is3D || right._is3D) return Vector(X+right.X, Y+right.Y, Z+right.Z);
  else                      return Vector(X+right.X, Y+right.Y);
}


/** Addition of another vector.
  * @param right Second (right) vector. */
void Vector::operator+=(Vector right) {
  X += right.X;
  Y += right.Y; 
  Z += right.Z;
  if (!_is3D) _is3D = right._is3D;  // Set flag, if addition is 3D.
}


/** Substraction of another vector.
 * @param right Second (right) vector.
 * @return Difference of both vectors. */
Vector Vector::operator-(Vector right) {
  if (_is3D || right._is3D) return Vector(X-right.X, Y-right.Y, Z-right.Z);
  else                      return Vector(X-right.X, Y-right.Y);
}


/** Substraction of another vector.
  * @param right Second (right) vector. */
void Vector::operator-=(Vector right) {
  X -= right.X;
  Y -= right.Y; 
  Z -= right.Z;
  if (!_is3D) _is3D = right._is3D;  // Set flag, if substraction is 3D.
}


/** Inverse (* -1) of a vector.
 * @return Vector *(-1). */
Vector Vector::operator-() {
  if (_is3D) return Vector(-X, -Y, -Z);
  else       return Vector(-X, -Y);
}


/** Multiply a vector with a scalar.
 * @param factor Scaling factor.
 * @return The scaled vector. */
Vector Vector::operator*(float factor) {
  if (_is3D) return Vector(X*factor, Y*factor, Z*factor);
  else       return Vector(X*factor, Y*factor);
}


/** Multiply a vector with a scalar [reverse order].
 * @param factor Scaling factor.
 * @param vec The vector to scale.
 * @return The scaled vector. */
Vector operator*(float factor, Vector vec) {
  return vec * factor;
}
   

/** Divides a vector with a scalar.
 * (Multiplication with inversed divisor.) 
 * @param div Divisor scalar.
 * @return The scaled vector. */
Vector Vector::operator/(float div) {
  return operator* (1.0f/div);
}


/** Returns the pitch (lateral axis) of this vector.
  * @return The pitch value [-90° -> 90°]. (In 2D always zero.) */
float Vector::GetPitch() {
  return RadToDeg(asinf(Z / GetLength()));
}


/** Returns the yaw (vertical axis) of this vector.
  * @return The yaw value [0° -> <360°]. 0°: northbound (y-axis) */
float Vector::GetYaw() {
  float yaw = 0.0f;

  // Check 90° and 270° (arctan infinite) first. (-> x-axis only)      
  if (fabs(Y) <= numeric_limits<float>::epsilon()) {
    if      (X > 0.0f) yaw = 90.0f;
    else if (X < 0.0f) yaw = 270.0f;
  }

  // Arctan argument fine? Calculate heading then.    
  else {
    yaw = RadToDeg(atanf(X / Y));
    if (Y   < 0.0f) yaw += 180.0f;  // Range  90° to 270° correction. 
    if (yaw < 0.0f) yaw += 360.0f;  // Range 270° to 360° correction.        
  }
  return yaw;
}


/** Returns whether this vector is 3D or not. 
  * @return 'True' if 3D, 'false' otherwise. */
bool Vector::Is3D() {
  return _is3D;
}
