#version 110

attribute vec3 posAttr;
attribute vec3 norAttr;
attribute vec2 texAttr;

uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat3 uNMatrix;

varying vec2 vTextureCoord;
varying vec3 vTransformedNormal;
varying vec4 vPosition;


void main(void) {
  vPosition = uMVMatrix * vec4(posAttr, 1.0);
  gl_Position = uPMatrix * vPosition;
  vTextureCoord = texAttr;
  vTransformedNormal = uNMatrix * norAttr;
}