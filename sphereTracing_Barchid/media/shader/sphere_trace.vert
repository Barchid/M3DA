#version 150

in vec2 vertex;
out vec2 fVertex;

void main() {
  fVertex=vertex;
  gl_Position=vec4(vertex,0,1);
}
