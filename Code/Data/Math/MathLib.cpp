#include <Data/Math/MathLib.h>
#include <Data/Primitives.h>


/** Multiply two quaternions.
 * @param quat1 The first factor (left side).
 * @param quat2 The second factor (right side).
 * @return The resulting new quaternion. */
Float4 MathLib::MultiplyQuads(Float4 quat1, Float4 quat2) {
  float 
    qax = quat1.X, qay = quat1.Y, qaz = quat1.Z, qaw = quat1.W,  // Left quaternion.
    qbx = quat2.X, qby = quat2.Y, qbz = quat2.Z, qbw = quat2.W;  // Right quaternion.

  // Perform quaternion multiplication.
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
void MathLib::MultiplyMatrices(float* m1, float* m2, float* out) {
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


/** Perform normalized linear interpolation.
 * @param a First quaternion.
 * @param b Second quaternion.
 * @param f Interpolation factor.
 * @return The interpolated rotation. */
Float4 MathLib::Interpolate_NLERP(Float4& a, Float4& b, float f) {
  float dot = (a.X*b.X + a.Y*b.Y + a.Z*b.Z + a.W*b.W);
  float inverseFactor = 1.0f - f;
  if (dot < 0) f = -f;
  Float4 r = Float4(
    inverseFactor * a.X + f * b.X,
    inverseFactor * a.Y + f * b.Y,
    inverseFactor * a.Z + f * b.Z,
    inverseFactor * a.W + f * b.W
  );
  float len = r.X*r.X + r.Y*r.Y + r.Z*r.Z + r.W*r.W;
  if (len > 0) { // Normalize the result.
    len = 1.0f / sqrtf(len);
    r.X *= len;
    r.Y *= len;
    r.Z *= len;
    r.W *= len;
  }
  return r;
}


/** Apply a transformation matrix to a vector.
 * @param vecIn Input vector (position).
 * @param mat Transformation matrix. 
 * @return The transformed vector. */
Float3 MathLib::TransformByMatrix(Float3 vecIn, float* mat) {
  float x = vecIn.X, y = vecIn.Y, z = vecIn.Z;
  float w = mat[3] * x + mat[7] * y + mat[11] * z + mat[15];
  w = w || 1.0f;
  return Float3(
    (mat[0] * x + mat[4] * y + mat[8] * z + mat[12]) / w,
    (mat[1] * x + mat[5] * y + mat[9] * z + mat[13]) / w,
    (mat[2] * x + mat[6] * y + mat[10] * z + mat[14]) / w
  );
}


/** Creates a transformation matrix.
 * @param trans Vector representing the translation.
 * @param rot Quaternion specifying the rotation.
 * @param scaling Scaling factor. Set it to (1,1,1), if not needed!
 * @param pivot Pivot point for the transformation.
 * @param out Reserved array for matrix output (float[16]). */
void MathLib::CreateRTSMatrix(Float3 trans, Float4 rot, Float3 scaling, Float3 pivot, float* out) {
  float 
    x = rot.X, y = rot.Y, z = rot.Z, w = rot.W,
    x2 = x + x, y2 = y + y, z2 = z + z,
    xx = x * x2, xy = x * y2, xz = x * z2,
    yy = y * y2, yz = y * z2, zz = z * z2,
    wx = w * x2, wy = w * y2, wz = w * z2,
        
    sx = scaling.X, sy = scaling.Y, sz = scaling.Z,
    ox = pivot.X,   oy = pivot.Y,   oz = pivot.Z;

  out[0] = (1 - (yy + zz)) * sx;
  out[1] = (xy + wz) * sx;
  out[2] = (xz - wy) * sx;
  out[3] = 0;
  out[4] = (xy - wz) * sy;
  out[5] = (1 - (xx + zz)) * sy;
  out[6] = (yz + wx) * sy;
  out[7] = 0;
  out[8] = (xz + wy) * sz;
  out[9] = (yz - wx) * sz;
  out[10] = (1 - (xx + yy)) * sz;
  out[11] = 0;
  out[12] = trans.X + ox - (out[0] * ox + out[4] * oy + out[8] * oz);
  out[13] = trans.Y + oy - (out[1] * ox + out[5] * oy + out[9] * oz);
  out[14] = trans.Z + oz - (out[2] * ox + out[6] * oy + out[10] * oz);
  out[15] = 1;
};
