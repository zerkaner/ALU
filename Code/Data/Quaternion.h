struct Float3;

struct Quaternion {
  
  float X, Y, Z;
  float W;

  public:

    /** Default constructor. */
    Quaternion() {}

    /** Create a new quaternion. */
    Quaternion(float x, float y, float z, float w);


    /** Normalising a quaternion works similar to a vector. This method will not
     *  do anything if the quaternion is close enough to being unit-length. */
    void Normalize();


    /** We need to get the inverse of a quaternion to properly apply a 
     *  quaternion-rotation to a vector. The conjugate of a quaternion is 
     *  the same as the inverse, as long as the quaternion is unit-length. */
    Quaternion GetConjugate();


    /** Multiplying this quaternion with q2 applies the rotation q2 to this. */
    Quaternion operator* (const Quaternion& q2) const;


    /** Multiplying a quaternion q with a vector v applies the q-rotation to v. */
    Float3 Quaternion::operator* (const Float3& vec);


    /** Convert from axis angle. */
    void FromAxis(const Float3& vec, float angle);


    /** Convert from Euler angles. */
    void FromEuler(float pitch, float yaw, float roll);


    /** Convert to axis angles. */
    void GetAxisAngle(Float3* axis, float* angle);


    /** Convert to Matrix. */
    void GetMatrix(float* matrix);
};
