#version 330

in vec4 position;
uniform mat4 mvp;

const vec3 pattern[20] = vec3[](
  vec3(0, 0, 0),
  vec3(0, 0, 1),
  vec3(0, 0, 2),
  vec3(0, 1, 2),
  vec3(0, 2, 2),
  vec3(0, 2, 1),
  vec3(0, 2, 0),
  vec3(0, 1, 0),

  vec3(1, 0, 0),
     //1, 0, 1
  vec3(1, 0, 2),
     //1, 1, 2
  vec3(1, 2, 2),
     //1, 2, 1
  vec3(1, 2, 0),
     //1, 1, 0

  vec3(2, 0, 0),
  vec3(2, 0, 1),
  vec3(2, 0, 2),
  vec3(2, 1, 2),
  vec3(2, 2, 2),
  vec3(2, 2, 1),
  vec3(2, 2, 0),
  vec3(2, 1, 0)
);

void main() {
  vec3 offset = vec3(0);
  int div = 1;
  int i = 0;
  int q;
  do {
    ++i;
    q = gl_InstanceID / div;
    offset += pow(3, i) * pattern[q % 20];
    div *= 20;
  } while (q > 0);
  gl_Position = mvp * (position + vec4(offset, 0));
}
