in vec3 vert;
in vec2 vertTexCoord;

varying vec2 fragTexCoord;

void main() {
  fragTexCoord = vertTexCoord;    
  gl_Position = gl_ModelViewProjectionMatrix * vec4(vert, 1); 
}