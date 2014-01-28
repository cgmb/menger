#include <iostream>
#include <GL/glut.h>

float vbuffer[] = {
  0.25, 0.25, 0.0,
  0.55, 0.25, 0.0,
  0.55, 0.55, 0.0,
  0.25, 0.55, 0.0
};

size_t ibuffer[] = {
  0, 1, 2, 3, 0
};

size_t ibuffer_size 
  = sizeof(ibuffer)/sizeof(ibuffer[0]);

float* index3f(size_t i) {
  return vbuffer + (3u * i);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_POLYGON);
  for (size_t i = 0; i < ibuffer_size; ++i) {
    glVertex3fv(index3f(ibuffer[i]));
  }
  glEnd();
  printf("I am redrawing!\n"); 

  glFlush();
}

void init() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void mouse(int button, int state, int x, int y) {
  if (state == GLUT_DOWN) {
    std::cout << "Button " << button << " pressed at "
      "x=" << x << ", y=" << y << std::endl;
  } else if (state == GLUT_UP) {
    std::cout << "Button " << button << " released at "
      "x=" << x << ", y=" << y << std::endl;
  }
}

void reshape(int w, int h) {
  glViewport(0,0, w, h);
  std::cout << "New window size "
    "w=" << w << ", h=" << h << std::endl;
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(250, 250); 
  glutInitWindowPosition(100, 100);
  glutCreateWindow ("RAWR!");
  init();
  display(); 
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMainLoop();
}
