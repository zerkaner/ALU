attribute vec3 vert;
attribute vec2 vertTexCoord;
attribute vec4 weights;
attribute mat4 bones[4];

varying vec2 fragTexCoord;
 
mat4 weightingFcn(void) {
  mat4 result = weights.x * bones[0];
  if (weights.y > 0) {
    result = result + weights.y * bones[1];
    if (weights.z > 0) {
      result = result + weights.z * bones[2];
      if (weights.w > 0) {
        result = result + weights.w * bones[3];
      }
    }
  }
  return result;
}

void main(void) {
  fragTexCoord = vertTexCoord;
  gl_Position = gl_ModelViewProjectionMatrix * weightingFcn() * vec4(vert, 1);
}