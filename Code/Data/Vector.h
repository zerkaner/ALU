#pragma once

// Skip sprintf warnings.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif


class Vector {

  private:
    bool _is3D;     // Dimension flag: false: 2D, true: 3D.


  public:
    float X, Y, Z;  // The vector's coordinates.

    /** Initialize an empty, three-dimensional(!) vector. */
    Vector();

    /** Initialize a two-dimensional vector (height is set to zero). */
    Vector(float x, float y);

    /** Initialize a three-dimensional vector. */
    Vector(float x, float y, float z);

    /** Destroy this vector. */
    ~Vector();

    /** Calculate point-to-point distance.
     * @param pos The target point.
     * @return Euclidian distance value. */
    float GetDistance(Vector pos);

    /** Calculate the vector length.
     * @return Length of this vector. */
    float GetLength();

    /** Calculate the normalized vector.
     * @return The normalized vector. */
    Vector GetNormalVector();

    /** Create normalized vectors orthogonal to this one.
     * @param nY Pointer for new y-axis normal vector.
     * @param nZ Same for z-axis (height) vector. */
    void GetPlanarOrthogonalVectors(Vector& nY, Vector& nZ);

    /** Output the position.
     * @return String with component-based notation. */
    char* ToString();

    /** Compare this with another Vector object for equality.
     * @param vector Comparison vector.
     * @return Equals boolean. */
    bool Equals(Vector vector);

    /** Calculate the dot (scalar) product of two vectors.
     * @param a First vector.
     * @param b Second vector.
     * @return The scalar [inner] product. */
    static float DotProduct(Vector a, Vector b);

    /** Calcute the cross product of two [3D!] vectors.
     * @param a First vector.
     * @param b Second vector.
     * @return The cross [outer] product. */
    static Vector CrossProduct(Vector a, Vector b);

    /** Addition with another vector.
     * @param right Second (right) vector.
     * @return Sum of both vectors. */
    Vector operator+(Vector right);

    /** Addition of another vector.
     * @param right Second (right) vector. */
    void operator+=(Vector right);

    /** Substraction of another vector.
     * @param right Second (right) vector.
     * @return Difference of both vectors. */
    Vector operator-(Vector right);

    /** Substraction of another vector.
     * @param right Second (right) vector. */
    void operator-=(Vector right);

    /** Inverse (* -1) of a vector.
     * @return Vector *(-1). */
    Vector operator-();

    /** Multiply a vector with a scalar.
     * @param factor Scaling factor.
     * @return The scaled vector. */
    Vector operator*(float factor);

    /** Divides a vector with a scalar.
     * (Multiplication with inversed divisor.) 
     * @param div Divisor scalar.
     * @return The scaled vector. */
    Vector operator/(float div);

    /** Returns the pitch (lateral axis) of this vector.
     * @return The pitch value [-90° -> 90°]. (In 2D always zero.) */
    float GetPitch();

    /** Returns the yaw (vertical axis) of this vector.
     * @return The yaw value [0° -> <360°]. 0°: northbound (y-axis) */
    float GetYaw();

    /** Returns whether this vector is 3D or not. 
     * @return 'True' if 3D, 'false' otherwise. */
    bool Is3D();
};



/** Multiply a vector with a scalar [reverse order].
 * @param factor Scaling factor.
 * @param vec The vector to scale.
 * @return The scaled vector. */
Vector operator*(float factor, Vector vec);


/** Degree to radians conversion (pi/180).
 * @param angle Angle in degree.
 * @return Angle in radians. */
static float DegToRad(float angle) {
  return angle*0.0174532925f;
}


/** Radians to degree conversion (180/pi).
 * @param angle Angle in radians.
 * @return Angle in degree. */
static float RadToDeg(float angle) {
  return angle*57.295779f;
}
