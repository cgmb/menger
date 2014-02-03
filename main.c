#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>

float g_cube_vbuffer[] = {
  -0.5, -0.5,  0.5, // 0 back top left
   0.5, -0.5,  0.5, // 1 back top right
   0.5,  0.5,  0.5, // 2 back bottom right
  -0.5,  0.5,  0.5, // 3 back bottom left
  -0.5, -0.5, -0.5, // 4 front top left
   0.5, -0.5, -0.5, // 5 front top right
   0.5,  0.5, -0.5, // 6 front bottom right
  -0.5,  0.5, -0.5, // 7 front bottom left
};

size_t g_cube_ibuffer[] = {
  0, 1, 2, 0, 2, 3, // back
  5, 1, 0, 4, 5, 0, // top
  7, 4, 0, 3, 7, 0, // left
  1, 5, 6, 1, 6, 2, // right
  3, 2, 6, 3, 6, 7, // bottom
  6, 5, 4, 7, 6, 4, // front
};

size_t g_cube_ibuffer_size
  = sizeof(g_cube_ibuffer)/sizeof(g_cube_ibuffer[0]);

size_t* g_square_ibuffer = g_cube_ibuffer + 0;
size_t g_square_ibuffer_size = 6;

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
  for (i = 0; i < g_cube_ibuffer_size; ++i) {
    glColor3f(
      normal_scale(i/6, 0, 6),
      normal_scale(i/6, 2, 6),
      normal_scale(i/6, 4, 6));
    glVertex3fv(vertex3f(g_cube_vbuffer, g_cube_ibuffer[i]));
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

const unsigned MAX_DEPTH = 6u;

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

float g_camera_rotation_x = 0.0;
float g_camera_rotation_y = 0.0;
float g_temp_camera_scale = 1.0;
float g_perm_camera_scale = 1.0;

void setup_world_camera() {
  glRotatef(g_camera_rotation_x, 0, 1, 0);
  glRotatef(g_camera_rotation_y, 1, 0, 0);
  glScalef(g_temp_camera_scale, g_temp_camera_scale, g_temp_camera_scale);
  glScalef(g_perm_camera_scale, g_perm_camera_scale, g_perm_camera_scale);
}

unsigned g_recurse_depth = 1;

typedef void(*button_fn)();
enum { CALLBACK_COUNT = 256 };
// we can afford wasting a few kiB of memory for this
button_fn g_button_callbacks[CALLBACK_COUNT + 1] = {};
size_t g_pressed_button = CALLBACK_COUNT;

void draw_button(float r, float g, float b) {
  glBegin(GL_TRIANGLES);
  size_t i;
  for (i = 0; i < g_square_ibuffer_size; ++i) {
    glColor3f(r, g, b);
    glVertex3fv(vertex3f(g_cube_vbuffer, g_square_ibuffer[i]));
  }
  glEnd();
}

void draw_pressed_button(float r, float g, float b) {
  draw_button(r, g, b);
  glScalef(15.0/16, 15.0/16, 0);
  glTranslatef(0, 0, -0.25);
  draw_button(0.5*r, 0.5*g, 0.5*b);
  glScalef(16.0/15, 16.0/15, 0);
  glTranslatef(0, 0, 0.25);
}

void draw_button_at(size_t index, float r, float g, float b) {
  float x = index / 16;
  float y = index % 16;
  glTranslatef(x, y, 0);

  if (index == g_pressed_button) {
    draw_pressed_button(r, g, b);
  } else {
    draw_button(r, g, b);
  }

  glTranslatef(-x, -y, 0);
}

void draw_buttons() {
  glDisable(GL_DEPTH_TEST);

  glScalef(1.0/16, 1.0/16, 1.0/16);
  glTranslatef(1.0/2, 1.0/2, 0);

  draw_button_at(0, 1, 1, 1);
  draw_button_at(1, 0, 1, 1);
  draw_button_at(2, 1, 0, 1);
  draw_button_at(3, 1, 1, 0);

  draw_button_at(16, 1, 0, 0);
  draw_button_at(17, 0, 1, 0);
  draw_button_at(18, 0, 0, 1);

  draw_button_at(32, 0.5, 0.5, 0.5);
  draw_button_at(33, 0.5, 0.0, 0.5);

  draw_button_at(48, 0.0, 0.5, 0.5);

  glEnable(GL_DEPTH_TEST);
}

float g_aspect_scale_x = 1.0;
float g_aspect_scale_y = 1.0;

typedef enum { ORTHO, PERSPECTIVE } proj_t;
proj_t g_main_proj = ORTHO;

void reset_to(proj_t proj) {
  glLoadIdentity();
  if (proj == ORTHO) {
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  } else {
    // because the depth buffer uses a logorithmic scale
    // bad things happen when the frustum crosses z=0
    glFrustum(0.0, 1.0, 0.0, 1.0, 10.0, 100.0);
    glTranslatef(0, 0, -11.0);
  }
  glScalef(g_aspect_scale_x, g_aspect_scale_y, 1.0);
}

void setup_initial_transform() {
  reset_to(g_main_proj);
  glTranslatef(0.5/g_aspect_scale_x, 0.5/g_aspect_scale_y, 0.5);
  glScalef(1.0/16.0, 1.0/16.0, 1.0/16.0);
  glRotatef(30.0, 1.0, 1.0, 1.0);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  setup_initial_transform();

  setup_world_camera();
  draw_menger_sponge(g_recurse_depth);

  reset_to(ORTHO);
  draw_buttons();

  glutSwapBuffers();
}

void init() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glMatrixMode(GL_PROJECTION);
}

int g_rotating = 0;
int g_scaling = 0;
int g_last_x = 0;
int g_last_y = 0;

int g_window_size_x = 480;
int g_window_size_y = 480;

size_t button_index_from_xy(int x, int y) {
  int reverse_y = g_window_size_y - y;
  size_t index =
    16u * (x / (int)(g_window_size_x * g_aspect_scale_x/16)) +
    (reverse_y / (int)(g_window_size_y * g_aspect_scale_y/16));
  // just in case...
  return bound_unsigned(0, index, CALLBACK_COUNT - 1);
}

void mouse_press(int button, int state, int x, int y) {
  if (state == GLUT_DOWN) {
    g_pressed_button = button_index_from_xy(x, y);
    if (g_button_callbacks[g_pressed_button]) {
      glutPostRedisplay();
    } else {
      g_last_x = x;
      g_last_y = y;
      if (button == GLUT_LEFT_BUTTON) {
        g_rotating = 1;
      } else if (button == GLUT_RIGHT_BUTTON) {
        g_scaling = 1;
      }
    }
  } else if (state == GLUT_UP) {
    button_fn callback = g_button_callbacks[g_pressed_button];
    g_pressed_button = CALLBACK_COUNT;
    if (callback) {
      glutPostRedisplay();
      callback();
    } else {
      g_rotating = 0;
      g_scaling = 0;
      g_perm_camera_scale *= g_temp_camera_scale;
      g_temp_camera_scale = 1.0;
    }
  }
}

void mouse_move(int x, int y) {
  if (g_rotating) {
    float diff_x = g_last_x - x;
    float diff_y = g_last_y - y;

    g_camera_rotation_x += 360.0 * (diff_x / g_window_size_x);
    g_camera_rotation_y += 360.0 * (diff_y / g_window_size_y);
    g_last_x = x;
    g_last_y = y;

    glutPostRedisplay();
  } else if (g_scaling) {
    float diff_y = g_last_y - y;

    g_temp_camera_scale = powf(2, (diff_y / g_window_size_y));

    glutPostRedisplay();
  }
}

enum {
  NORMAL = 27, // esc
  SCALE_ENTRY = 's',
  RECURSE_ENTRY = 'r',
  PROJECTION_ENTRY = 'p',
  QUIT_COMMAND = 'q',
  ENTER_COMMAND = 13, // enter
};

unsigned g_command_state = NORMAL;
enum { MAX_ARG_LENGTH = 5 };
char g_arg_text[MAX_ARG_LENGTH + 1] = {};
size_t g_arg_text_i = 0;

void key_press(unsigned char key, int x, int y) {
  switch (g_command_state) {
    case SCALE_ENTRY:
      if (key == ENTER_COMMAND) {
        char* end;
        float number = strtof(g_arg_text, &end);
        if (*end == '\0' && errno != EINVAL &&
            errno != ERANGE && number != 0.0) {
          g_perm_camera_scale = 1/number;
          glutPostRedisplay();
        } else {
           printf("\ninvalid scale");
        }
        g_command_state = NORMAL;
        printf("\n");
      } else if (key == 27) {
        g_command_state = NORMAL;
        printf(" - cancelled\n");
      } else if (g_arg_text_i < MAX_ARG_LENGTH) {
        g_arg_text[g_arg_text_i++] = key;
        printf("%c", key);
        fflush(stdout);
      }
      break;
    case RECURSE_ENTRY:
      if (key == ENTER_COMMAND) {
        char* end;
        long number = strtol(g_arg_text, &end, 10);
        if (*end == '\0' && errno != EINVAL &&
            errno != ERANGE && number >= 0 && number <= (long)MAX_DEPTH) {
          g_recurse_depth = (unsigned)number;
          glutPostRedisplay();
        } else {
           printf("\ninvalid recursive depth");
        }
        g_command_state = NORMAL;
        printf("\n");
      } else if (key == 27) {
        g_command_state = NORMAL;
        printf(" - cancelled\n");
      } else if (g_arg_text_i < MAX_ARG_LENGTH) {
        g_arg_text[g_arg_text_i++] = key;
        printf("%c", key);
        fflush(stdout);
      }
      break;
    case PROJECTION_ENTRY:
      if (key == 'o') {
        g_main_proj = ORTHO;
        glutPostRedisplay();
        printf("%c\n", key);
      } else if (key == 'p') {
        g_main_proj = PERSPECTIVE;
        glutPostRedisplay();
        printf("%c\n", key);
      } else if (key == 27) {
        g_command_state = NORMAL;
        printf(" - cancelled\n");
      } else {
        printf("\nunknown projection\n");
        printf("known projections are:\n"
          "  o - orthographic\n"
          "  p - perspective\n");
      }
      g_command_state = NORMAL;
      break;
    default: // enter state
      g_command_state = key;
      switch(g_command_state) {
        case SCALE_ENTRY:
        case RECURSE_ENTRY:
        case PROJECTION_ENTRY:
          memset(g_arg_text, 0, MAX_ARG_LENGTH);
          g_arg_text_i = 0;
          printf("%c:", key);
          fflush(stdout);
          break;
        case QUIT_COMMAND:
          exit(0);
        case NORMAL:
          break;
        default:
	  printf("unknown command: %c\n", key);
          printf("known commands are:\n"
            "  s - scale\n"
            "  r - recursion depth [0,%u]\n"
            "  p - perspective {o,p}\n"
            "  q - fast quit\n", MAX_DEPTH);
      }
  }
}

void reshape(int w, int h) {
  glViewport(0, 0, w, h);
  if (w > h) {
    g_aspect_scale_x = h / (float)w;
    g_aspect_scale_y = 1.0;
  } else {
    g_aspect_scale_x = 1.0;
    g_aspect_scale_y = w / (float)h;
  }
  g_window_size_x = w;
  g_window_size_y = h;
}

void set_recursive_depth(unsigned depth) {
  g_recurse_depth = depth;
  g_perm_camera_scale = 3.0 / power_fu(3.0, g_recurse_depth);
  glutPostRedisplay();
}

void set_recursive_depth_0() {
  set_recursive_depth(0);
}

void set_recursive_depth_1() {
  set_recursive_depth(1);
}

void set_recursive_depth_2() {
  set_recursive_depth(2);
}

void set_recursive_depth_3() {
  set_recursive_depth(3);
}

void set_scale(float s) {
  g_perm_camera_scale = s;
  glutPostRedisplay();
}

void set_scale_1() {
  set_scale(1.0/1);
}

void set_scale_2() {
  set_scale(1.0/2);
}

void set_scale_5() {
  set_scale(1.0/5);
}

void set_projection_ortho() {
  g_main_proj = ORTHO;
  glutPostRedisplay();
}

void set_projection_perspective() {
  g_main_proj = PERSPECTIVE;
  glutPostRedisplay();
}

void setup_callbacks() {
  g_button_callbacks[0] = set_recursive_depth_0;
  g_button_callbacks[1] = set_recursive_depth_1;
  g_button_callbacks[2] = set_recursive_depth_2;
  g_button_callbacks[3] = set_recursive_depth_3;
  g_button_callbacks[16] = set_scale_1;
  g_button_callbacks[17] = set_scale_2;
  g_button_callbacks[18] = set_scale_5;
  g_button_callbacks[32] = set_projection_ortho;
  g_button_callbacks[33] = set_projection_perspective;
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
  glutInitWindowSize(g_window_size_x, g_window_size_y);
  glutInitWindowPosition(100, 100);
  glutCreateWindow ("C Menger Sponge");
  init();
  setup_callbacks();
  display();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse_press);
  glutMotionFunc(mouse_move);
  glutKeyboardFunc(key_press);
  glutMainLoop();
  return 0;
}
