#pragma once
#include <Data/Primitives.h>


class MathLib {

  public:

    /** Multiply a vector with a quaternion. 
     * @param vec The vector that shall be rotated.
     * @param quat The quaternion representing the rotation to perform.
     * @return A new vector with the applied rotation. */
    static Float3 RotateVector(Float3 vec, Float4 quat) {
      float 
        x = vec.X, y = vec.Y, z = vec.Z,                    // Vector component shortcuts.
        qx = quat.X, qy = quat.Y, qz = quat.Z, qw = quat.W, // Quaternion shortcuts.

        // Calculate quaternion * vector.
        ix = qw * x + qy * z - qz * y,
        iy = qw * y + qz * x - qx * z,
        iz = qw * z + qx * y - qy * x,
        iw = -qx * x - qy * y - qz * z;    
      
      // Calculate result * inverse quaternion and return.
      return Float3(
        ix * qw  +  iw * -qx  +  iy * -qz  -  iz * -qy,
        iy * qw  +  iw * -qy  +  iz * -qx  -  ix * -qz,
        iz * qw  +  iw * -qz  +  ix * -qy  -  iy * -qx        
      );
    }


    /** Multiply two quaternions.
     * @param quat1 The first factor (left side).
     * @param quat2 The second factor (right side).
     * @return The resulting new quaternion. */
    static Float4 MultiplyRotations(Float4 quat1, Float4 quat2) {
      float 
        qax = quat1.X, qay = quat1.Y, qaz = quat1.Z, qaw = quat1.W,  // Left quaternion.
        qbx = quat2.X, qby = quat2.Y, qbz = quat2.Z, qbw = quat2.W;  // Right quaternion.

      // Perform quaternion multiplcation.
      return Float4(
        qax * qbw  +  qaw * qbx  +  qay * qbz  -  qaz * qby,
        qay * qbw  +  qaw * qby  +  qaz * qbx  -  qax * qbz,
        qaz * qbw  +  qaw * qbz  +  qax * qby  -  qay * qbx,
        qaw * qbw  -  qax * qbx  -  qay * qby  -  qaz * qbz        
      );
    }



    /** Muliply two matrices.
     * @param m1 The first matrix (float[16]). 
     * @param m2 The second matrix (float[16]).
     * @param out Reserved array for output (also float[16]). */
    static void MultiplyMatrices(float* m1, float* m2, float* out) { 
      float
        a00 = m1[0],  a01 = m1[1],  a02 = m1[2],  a03 = m1[3],  // Cache the matrix  
        a10 = m1[4],  a11 = m1[5],  a12 = m1[6],  a13 = m1[7],  // values (makes for 
        a20 = m1[8],  a21 = m1[9],  a22 = m1[10], a23 = m1[11], // huge speed increases).
        a30 = m1[12], a31 = m1[13], a32 = m1[14], a33 = m1[15],

        b00 = m2[0],  b01 = m2[1],  b02 = m2[2],  b03 = m2[3],
        b10 = m2[4],  b11 = m2[5],  b12 = m2[6],  b13 = m2[7],
        b20 = m2[8],  b21 = m2[9],  b22 = m2[10], b23 = m2[11],
        b30 = m2[12], b31 = m2[13], b32 = m2[14], b33 = m2[15];

      // Do the multiplication.
      out[0] = b00 * a00 + b01 * a10 + b02 * a20 + b03 * a30;
      out[1] = b00 * a01 + b01 * a11 + b02 * a21 + b03 * a31;
      out[2] = b00 * a02 + b01 * a12 + b02 * a22 + b03 * a32;
      out[3] = b00 * a03 + b01 * a13 + b02 * a23 + b03 * a33;
      out[4] = b10 * a00 + b11 * a10 + b12 * a20 + b13 * a30;
      out[5] = b10 * a01 + b11 * a11 + b12 * a21 + b13 * a31;
      out[6] = b10 * a02 + b11 * a12 + b12 * a22 + b13 * a32;
      out[7] = b10 * a03 + b11 * a13 + b12 * a23 + b13 * a33;
      out[8] = b20 * a00 + b21 * a10 + b22 * a20 + b23 * a30;
      out[9] = b20 * a01 + b21 * a11 + b22 * a21 + b23 * a31;
      out[10] = b20 * a02 + b21 * a12 + b22 * a22 + b23 * a32;
      out[11] = b20 * a03 + b21 * a13 + b22 * a23 + b23 * a33;
      out[12] = b30 * a00 + b31 * a10 + b32 * a20 + b33 * a30;
      out[13] = b30 * a01 + b31 * a11 + b32 * a21 + b33 * a31;
      out[14] = b30 * a02 + b31 * a12 + b32 * a22 + b33 * a32;
      out[15] = b30 * a03 + b31 * a13 + b32 * a23 + b33 * a33;
    }



    /** Creates a matrix from a quaternion rotation and vector translation.
     * @param quat Quaternion specifying the rotation by.
     * @param vec Vector specifying the translation.
     * @param out Reserved array for output (also float[16]). */
    static void CreateRTMatrix(Float4 quat, Float3 vec, float* out) {
    
      float  // Quaternion math.
        x = quat.X, y = quat.Y, z = quat.Z, w = quat.W,
        x2 = x + x,    y2 = y + y,    z2 = z + z,
        xx = x * x2,   xy = x * y2,   xz = x * z2,
        yy = y * y2,   yz = y * z2,   zz = z * z2,
        wx = w * x2,   wy = w * y2,   wz = w * z2;

      out[0] = 1 - (yy + zz);
      out[1] = xy + wz;
      out[2] = xz - wy;
      out[3] = 0;
      out[4] = xy - wz;
      out[5] = 1 - (xx + zz);
      out[6] = yz + wx;
      out[7] = 0;
      out[8] = xz + wy;
      out[9] = yz - wx;
      out[10] = 1 - (xx + yy);
      out[11] = 0;
      out[12] = vec.X;
      out[13] = vec.Y;
      out[14] = vec.Z;
      out[15] = 1;
    }
};
