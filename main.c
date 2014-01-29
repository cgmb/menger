#include <GL/glut.h>

float vbuffer[] = {
  -0.5, -0.5,  0.5, // 0 back top left
   0.5, -0.5,  0.5, // 1 back top right
   0.5,  0.5,  0.5, // 2 back bottom right
  -0.5,  0.5,  0.5, // 3 back bottom left
  -0.5, -0.5, -0.5, // 4 front top left
   0.5, -0.5, -0.5, // 5 front top right
   0.5,  0.5, -0.5, // 6 front bottom right
  -0.5,  0.5, -0.5, // 7 front bottom left
};

size_t ibuffer[] = {
  0, 1, 2, 0, 2, 3, // back
  5, 1, 0, 4, 5, 0, // top
  7, 4, 0, 3, 7, 0, // left
  1, 5, 6, 1, 6, 2, // right
  3, 2, 6, 3, 6, 7, // bottom
  6, 5, 4, 7, 6, 4, // front
};

size_t ibuffer_size 
  = sizeof(ibuffer)/sizeof(ibuffer[0]);

// lookup a vertex within the given float buffer
float* vertex3f(float* buffer, size_t index) {
  return buffer + (3u * index);
}

// scale to range [0,1]
float normal_scale(int value, int offset, int steps) {
  return ((value + offset) % steps) / (float)steps;
}

void draw_cube() {
  glBegin(GL_TRIANGLES);
  size_t i;
  for (i = 0; i < ibuffer_size; ++i) {
    glColor3f(
      normal_scale(ibuffer[i], 6, 8), 
      normal_scale(ibuffer[i], 0, 8), 
      normal_scale(ibuffer[i], 3, 8));
    glVertex3fv(vertex3f(vbuffer, ibuffer[i]));
  }
  glEnd();
}

typedef void(*draw_fn)();

// draws the Menger sponge pattern using blocks drawn 
// by the draw function. The size of each block is
// specified by s.
void draw_pattern(draw_fn draw, float s) {
  // move to bottom corner and draw ring
  glTranslatef(-s, -s, -s);
  draw();
  glTranslatef(s, 0, 0);
  draw();
  glTranslatef(s, 0, 0);
  draw();
  glTranslatef(0, s, 0);
  draw();
  glTranslatef(0, s, 0);
  draw();
  glTranslatef(-s, 0, 0);
  draw();
  glTranslatef(-s, 0, 0);
  draw();
  glTranslatef(0, -s, 0);
  draw();

  // move to next row and draw ring with holes
  glTranslatef(0, -s, s);
  draw();

  glTranslatef(s, 0, 0);
//  draw();
  glTranslatef(s, 0, 0);
  draw();
  glTranslatef(0, s, 0);
//  draw();
  glTranslatef(0, s, 0);
  draw();
  glTranslatef(-s, 0, 0);
//  draw();
  glTranslatef(-s, 0, 0);
  draw();
  glTranslatef(0, -s, 0);
//  draw();
  
  // move to next row and draw ring again
  glTranslatef(0, -s, s);
  draw();

  glTranslatef(s, 0, 0);
  draw();
  glTranslatef(s, 0, 0);
  draw();
  glTranslatef(0, s, 0);
  draw();
  glTranslatef(0, s, 0);
  draw();
  glTranslatef(-s, 0, 0);
  draw();
  glTranslatef(-s, 0, 0);
  draw();
  glTranslatef(0, -s, 0);
  draw();

  // move back to center
  glTranslatef(s, 0, -s);
}

void draw_unit() {
  draw_pattern(draw_cube, 1);
}

const unsigned MAX_DEPTH = 4u;

unsigned bound_unsigned(unsigned min, unsigned value, unsigned max) {
  if (value > max) {
    value = max;
  } else if (value < min) {
    value = min;
  }
  return value;
}

float power_fu(float x, unsigned y) {
  float value = 1.0;
  unsigned i;
  for (i = 0; i < y; ++i) {
    value *= x;
  }
  return value;
}

// global state for draw_pattern_recursive
unsigned g_current_depth;

// a tricky recursiive function used to supply draw_pattern back to itself.
// to match the signature of draw_fn, the scale is based on g_current_depth.
void draw_pattern_recursive() {
  unsigned frame_depth = g_current_depth;
  if (frame_depth > 0u) {
    --g_current_depth;
    draw_pattern(draw_pattern_recursive, power_fu(3.0, frame_depth));
    ++g_current_depth;
  } else {
    draw_pattern(draw_cube, 1);
  }
}

void draw_menger_sponge(unsigned depth) {
  g_current_depth = bound_unsigned(0, depth, MAX_DEPTH);
  draw_pattern_recursive();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  draw_menger_sponge(1);

  glutSwapBuffers();
}

void init() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  glTranslatef(0.5, 0.5, 0.5);
  glScalef(1.0/16.0, 1.0/16.0, 1.0/16.0);
  glRotatef(30.0, 1.0, 1.0, 1.0);
}

void mouse_press(int button, int state, int x, int y) {
  if (state == GLUT_DOWN) {
    glRotatef(1, 1.0, 1.0, 1.0);
    glutPostRedisplay();
  }
}

void mouse_move(int x, int y) {
  glRotatef(1, 1.0, 1.0, 1.0);
  glutPostRedisplay();
}

void reshape(int w, int h) {
  glViewport(0, 0, w, h);
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
  glutInitWindowSize(480, 480); 
  glutInitWindowPosition(100, 100);
  glutCreateWindow ("C Menger Sponge");
  init();
  display(); 
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse_press);
  glutMotionFunc(mouse_move);
  glutMainLoop();
  return 0;
}
