#pragma once
struct Float3;
struct Float4;

class MathLib {

  public:

    /** Multiply two quaternions.
     * @param quat1 The first factor (left side).
     * @param quat2 The second factor (right side).
     * @return The resulting new quaternion. */
    static Float4 MultiplyQuads(Float4 quat1, Float4 quat2);


    /** Muliply two matrices.
     * @param m1 The first matrix (float[16]). 
     * @param m2 The second matrix (float[16]).
     * @param out Reserved array for output (also float[16]). */
    static void MultiplyMatrices(float* m1, float* m2, float* out);


    /** Perform normalized linear interpolation.
     * @param a First quaternion.
     * @param b Second quaternion. 
     * @param f Interploation factor.
     * @return The interpolated rotation. */
    static Float4 Interpolate_NLERP(Float4& a, Float4& b, float f);


    /** Apply a transformation matrix to a vector.
     * @param vecIn Input vector (position).
     * @param mat Transformation matrix. 
     * @return The transformed vector. */
    static Float3 TransformByMatrix(Float3 vecIn, float* mat);


    /** Creates a transformation matrix.
     * @param trans Vector representing the translation.
     * @param rot Quaternion specifying the rotation.
     * @param scaling Scaling factor. Set it to (1,1,1), if not needed!
     * @param pivot Pivot point for the transformation.
     * @param out Reserved array for matrix output (float[16]). */
    static void CreateRTSMatrix(Float3 trans, Float4 rot, Float3 scaling, Float3 pivot, float* out);
};
