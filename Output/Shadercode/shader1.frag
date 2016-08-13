uniform sampler2D tex;
varying vec2 fragTexCoord;

void main() {
  gl_FragColor = texture(tex, fragTexCoord);
}