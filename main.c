#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "vector_math.h"

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#else
#error BUFFER_OFFSET already defined!
#endif

const float g_ortho[] = {
  1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 1,
};

const float g_identity[] = {
  1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 1, 0,
  0, 0, 0, 1,
};

float g_proj[16];
float g_view[16];
float g_mvp_matrix[16];

void m4f_print(const float* m) {
  printf("[% f % f % f % f]\n"
         "[% f % f % f % f]\n"
         "[% f % f % f % f]\n"
         "[% f % f % f % f]\n",
    m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7],
    m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
}

GLuint g_mvp_id;
GLuint g_color_id;

const float g_slow_cube_vbuffer[] = {
  -0.5, -0.5,  0.5, // 0
   0.5, -0.5,  0.5, // 1
   0.5,  0.5,  0.5, // 2
  -0.5, -0.5,  0.5, // 0
   0.5,  0.5,  0.5, // 2
  -0.5,  0.5,  0.5, // 3

   0.5, -0.5, -0.5, // 5
   0.5, -0.5,  0.5, // 1
  -0.5, -0.5,  0.5, // 0
  -0.5, -0.5, -0.5, // 4
   0.5, -0.5, -0.5, // 5
  -0.5, -0.5,  0.5, // 0

  -0.5,  0.5, -0.5, // 7
  -0.5, -0.5, -0.5, // 4
  -0.5, -0.5,  0.5, // 0
  -0.5,  0.5,  0.5, // 3
  -0.5,  0.5, -0.5, // 7
  -0.5, -0.5,  0.5, // 0

   0.5, -0.5,  0.5, // 1
   0.5, -0.5, -0.5, // 5
   0.5,  0.5, -0.5, // 6
   0.5, -0.5,  0.5, // 1
   0.5,  0.5, -0.5, // 6
   0.5,  0.5,  0.5, // 2

  -0.5,  0.5,  0.5, // 3
   0.5,  0.5,  0.5, // 2
   0.5,  0.5, -0.5, // 6
  -0.5,  0.5,  0.5, // 3
   0.5,  0.5, -0.5, // 6
  -0.5,  0.5, -0.5, // 7

   0.5,  0.5, -0.5, // 6
   0.5, -0.5, -0.5, // 5
  -0.5, -0.5, -0.5, // 4
  -0.5,  0.5, -0.5, // 7
   0.5,  0.5, -0.5, // 6
  -0.5, -0.5, -0.5, // 4
};

const float g_cube_vbuffer[] = {
  -0.5, -0.5,  0.5, // 0 back top left
   0.5, -0.5,  0.5, // 1 back top right
   0.5,  0.5,  0.5, // 2 back bottom right
  -0.5,  0.5,  0.5, // 3 back bottom left
  -0.5, -0.5, -0.5, // 4 front top left
   0.5, -0.5, -0.5, // 5 front top right
   0.5,  0.5, -0.5, // 6 front bottom right
  -0.5,  0.5, -0.5, // 7 front bottom left
};

const size_t g_cube_ibuffer[] = {
  0, 1, 2, 0, 2, 3, // back
  5, 1, 0, 4, 5, 0, // top
  7, 4, 0, 3, 7, 0, // left
  1, 5, 6, 1, 6, 2, // right
  3, 2, 6, 3, 6, 7, // bottom
  6, 5, 4, 7, 6, 4, // front
};

const size_t g_cube_ibuffer_size
  = sizeof(g_cube_ibuffer)/sizeof(g_cube_ibuffer[0]);

GLuint g_cube_vbuffer_id;
GLuint g_cube_ibuffer_id;

float g_cube_normal_buffer[36];

GLuint g_cube_normal_buffer_id;

const size_t* g_square_ibuffer = g_cube_ibuffer + 0;
const size_t g_square_ibuffer_size = 6;

// lookup a vertex within the given float buffer
const float* vertex3f(const float* buffer, size_t index) {
  return buffer + (3u * index);
}

// scale to range [0,1]
float normal_scale(int value, int offset, int steps) {
  return ((value + offset) % steps) / (float)steps;
}

void triangle_normal_3fo(
const float* a, const float* b, const float* c, float* result) {
  float ab[3];
  difference_v3fo(b, a, ab);
  float ac[3];
  difference_v3fo(c, a, ac);
  v3f_cross_v3fo(ab, ac, result);
  normalize_v3f(result);
}

int g_use_lighting = 0;

void draw_cube() {
    glUniform4f(g_color_id, 1.0, 1.0, 1.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glUniform4f(g_color_id, 0.0, 1.0, 1.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 6, 6);
    glUniform4f(g_color_id, 1.0, 0.0, 1.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 12, 6);
    glUniform4f(g_color_id, 1.0, 1.0, 0.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 18, 6);
    glUniform4f(g_color_id, 0.0, 0.0, 1.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 24, 6);
    glUniform4f(g_color_id, 0.0, 1.0, 0.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 30, 6);
    glUniform4f(g_color_id, 1.0, 0.0, 0.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 36, 6);
    glUniform4f(g_color_id, 0.25, 0.25, 0.25, 1.0);
    glDrawArrays(GL_TRIANGLES, 42, 6);
/*  glBegin(GL_TRIANGLES);
  size_t i;
  for (i = 0; i < g_cube_ibuffer_size; ++i) {
    if (g_use_lighting) {
      glNormal3fv(vertex3f(g_cube_normal_buffer, i/3u));
    }
    glColor3f(
      normal_scale(i/6, 0, 6),
      normal_scale(i/6, 2, 6),
      normal_scale(i/6, 4, 6));
    glVertex3fv(vertex3f(g_cube_vbuffer, g_cube_ibuffer[i]));
  }
  glEnd();*/
}

typedef void(*draw_fn)();

void evil_translate(float x, float y, float z) {
  float m[16];
  m4f_fill_trans_m4fo(x, y, z, m);

  float evil[16];
  m4f_mul_m4fo(g_mvp_matrix, m, evil);
  m4f_copy_m4fo(evil, g_mvp_matrix);

  glUniformMatrix4fv(g_mvp_id, 1, GL_TRUE, g_mvp_matrix);
}

// draws the Menger sponge pattern using blocks drawn
// by the draw function. The size of each block is
// specified by s.
void draw_pattern(draw_fn draw, float s) {
  // move to bottom corner and draw ring
  evil_translate(-s, -s, -s);
  draw();
  evil_translate(s, 0, 0);
  draw();
  evil_translate(s, 0, 0);
  draw();
  evil_translate(0, s, 0);
  draw();
  evil_translate(0, s, 0);
  draw();
  evil_translate(-s, 0, 0);
  draw();
  evil_translate(-s, 0, 0);
  draw();
  evil_translate(0, -s, 0);
  draw();

  // move to next row and draw ring with holes
  evil_translate(0, -s, s);
  draw();

  evil_translate(s, 0, 0);
//  draw();
  evil_translate(s, 0, 0);
  draw();
  evil_translate(0, s, 0);
//  draw();
  evil_translate(0, s, 0);
  draw();
  evil_translate(-s, 0, 0);
//  draw();
  evil_translate(-s, 0, 0);
  draw();
  evil_translate(0, -s, 0);
//  draw();

  // move to next row and draw ring again
  evil_translate(0, -s, s);
  draw();

  evil_translate(s, 0, 0);
  draw();
  evil_translate(s, 0, 0);
  draw();
  evil_translate(0, s, 0);
  draw();
  evil_translate(0, s, 0);
  draw();
  evil_translate(-s, 0, 0);
  draw();
  evil_translate(-s, 0, 0);
  draw();
  evil_translate(0, -s, 0);
  draw();

  // move back to center
  evil_translate(s, 0, -s);
}

void draw_unit() {
  draw_pattern(draw_cube, 1);
}

unsigned bound_unsigned(unsigned min, unsigned value, unsigned max) {
  if (value > max) {
    value = max;
  } else if (value < min) {
    value = min;
  }
  return value;
}

// global state for draw_pattern_recursive
unsigned g_current_depth;

enum { MAX_DEPTH = 6u };
const float g_power_of_3[MAX_DEPTH + 1] = {
  1, 3, 9, 27, 81, 243, 729,
};

// a tricky recursiive function used to supply draw_pattern back to itself.
// to match the signature of draw_fn, the scale is based on g_current_depth.
void draw_pattern_recursive() {
  unsigned frame_depth = g_current_depth;
  if (frame_depth > 0u) {
    --g_current_depth;
    draw_pattern(draw_pattern_recursive, g_power_of_3[frame_depth]);
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
  float rotx[16];
  float roty[16];
  float scale_temp[16];
  float scale_perm[16];

  m4f_fill_rotx_m4fo(g_camera_rotation_x, rotx);
  m4f_fill_roty_m4fo(g_camera_rotation_y, roty);
  m4f_fill_scale_m4fo(g_temp_camera_scale,
    g_temp_camera_scale, g_temp_camera_scale, scale_temp);
  m4f_fill_scale_m4fo(g_perm_camera_scale,
    g_perm_camera_scale, g_perm_camera_scale, scale_perm);

  float rotyx[16];
  m4f_mul_m4fo(roty, rotx, rotyx);
  float scale[16];
  m4f_mul_m4fo(scale_perm, scale_temp, scale);
  m4f_mul_m4fo(scale, rotyx, g_view);
/*
  glRotatef(g_camera_rotation_x, 0, 1, 0);
  glRotatef(g_camera_rotation_y, 1, 0, 0);
  glScalef(g_temp_camera_scale, g_temp_camera_scale, g_temp_camera_scale);
  glScalef(g_perm_camera_scale, g_perm_camera_scale, g_perm_camera_scale);
*/
}

unsigned g_recurse_depth = 1;
unsigned g_last_recurse_depth = 0;

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
  m4f_copy_m4fo(g_identity, g_proj);
  if (proj == ORTHO) {
//    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  } else {
    float perspective[] = {
      1, 0, 0,       0,
      0, 1, 0,       0,
      0, 0, 1,       0,
      0, 0, 1.0/2.0, 1,
    };
    m4f_copy_m4fo(perspective, g_proj);
    // because the depth buffer uses a logorithmic scale
    // bad things happen when the frustum crosses z=0
//    glFrustum(0.0, 1.0, 0.0, 1.0, 3.0, 50.0);
//    glTranslatef(0, 0, -4.0);
  }
  float scale[16];
  m4f_fill_scale_m4fo(g_aspect_scale_x, g_aspect_scale_y, 1.0, scale);
  m4f_mul_m4f(scale, g_proj);
//  glScalef(g_aspect_scale_x, g_aspect_scale_y, 1.0);
}

void setup_initial_transform() {
  reset_to(g_main_proj);
  float init_transform[16];
  float translate[16];
  m4f_fill_trans_m4fo(
    0.5/g_aspect_scale_x,
    0.5/g_aspect_scale_y,
    0.5,
    translate);

  float scale[16];
  m4f_fill_scale_m4fo(
    1.0/2.00,
    1.0/2.00,
    1.0/2.00,
    scale);

  m4f_mul_m4fo(scale, translate, init_transform);
  m4f_mul_m4f(init_transform, g_proj);

//  glTranslatef(0.5/g_aspect_scale_x, 0.5/g_aspect_scale_y, 0.5);
//  glScalef(1.0/16.0, 1.0/16.0, 1.0/16.0);
//  glRotatef(30.0, 1.0, 1.0, 1.0);
}

void init_light() {
  float position[] = { 1, 1, 1, 0 };
  glLightfv(GL_LIGHT0, GL_POSITION, position);
  float ambient_color[] = { 0.125, 0.125, 0.125, 1 };
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color);
  float diffuse_color[] = { 1, 1, 1, 1 };
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_color);
  float specular_color[] = { 0.75, 0.75, 0.75, 1 };
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular_color);
}

void init_material() {
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  float specular_color[] = { 0.75, 0.75, 0.75, 1 };
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_color);
  float emission_color[] = { 0, 0, 0, 1 };
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission_color);
}

void setup_light() {
  if (g_use_lighting) {
    float original_position[] = { 1, 1, 1, 0 };
    float transformed_1[4] = {};
    float transformed_2[4] = {};
    float x = -g_camera_rotation_y / 180.0 * M_PI;
    float y = -g_camera_rotation_x / 180.0 * M_PI;
    rotatey_v3fo(y, original_position, transformed_1);
    rotatex_v3fo(x, transformed_1, transformed_2);
    glLightfv(GL_LIGHT0, GL_POSITION, transformed_2);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
  }
}

void teardown_light() {
  if (g_use_lighting) {
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
  }
}

//enum { MAX_DEPTH = 6u };
//const float g_power_of_3[MAX_DEPTH + 1] = {
//  1, 3, 9, 27, 81, 243, 729,
//};

void apply_translate(float x, float y, float z,
  const float* input, size_t input_count, float* output) {
  size_t i;
  for (i = 0; i < input_count; i += 3) {
    translate_v3fo(x, y, z, input + i, output + i);
  }
}

// s = scale
// input_count = number of floats in input array
void pattern_transform(float s, const float* input, size_t input_count,
  float* output) {
  // ring
  apply_translate(-s, -s, -s, input, input_count, output);
  output += input_count;
  apply_translate(0, -s, -s, input, input_count, output);
  output += input_count;
  apply_translate(s, -s, -s, input, input_count, output);
  output += input_count;
  apply_translate(s, 0, -s, input, input_count, output);
  output += input_count;
  apply_translate(s, s, -s, input, input_count, output);
  output += input_count;
  apply_translate(0, s, -s, input, input_count, output);
  output += input_count;
  apply_translate(-s, s, -s, input, input_count, output);
  output += input_count;
  apply_translate(-s, 0, -s, input, input_count, output);
  output += input_count;

  // ring with holes
  apply_translate(-s, -s, 0, input, input_count, output);
  output += input_count;
//  apply_translate(0, -s, 0, input, input_count, output);
//  output += input_count;
  apply_translate(s, -s, 0, input, input_count, output);
  output += input_count;
//  apply_translate(s, 0, 0, input, input_count, output);
//  output += input_count;
  apply_translate(s, s, 0, input, input_count, output);
  output += input_count;
//  apply_translate(0, s, 0, input, input_count, output);
//  output += input_count;
  apply_translate(-s, s, 0, input, input_count, output);
  output += input_count;
//  apply_translate(-s, 0, 0, input, input_count, output);
//  output += input_count;

  // ring
  apply_translate(-s, -s, s, input, input_count, output);
  output += input_count;
  apply_translate(0, -s, s, input, input_count, output);
  output += input_count;
  apply_translate(s, -s, s, input, input_count, output);
  output += input_count;
  apply_translate(s, 0, s, input, input_count, output);
  output += input_count;
  apply_translate(s, s, s, input, input_count, output);
  output += input_count;
  apply_translate(0, s, s, input, input_count, output);
  output += input_count;
  apply_translate(-s, s, s, input, input_count, output);
  output += input_count;
  apply_translate(-s, 0, s, input, input_count, output);
  output += input_count;
}

int g_buffer_vert_count = 0;

void recalculate_sponge() {
  size_t cube_count = 1;
  float* buffer_arr[MAX_DEPTH]; // todo: off-by-1?
  size_t buffer_arr_size[MAX_DEPTH]; // todo: off-by-1?
  unsigned i;
  for (i = 0; i <= g_recurse_depth; ++i) {
    cube_count *= 20u;
    buffer_arr_size[i] = cube_count * sizeof(g_slow_cube_vbuffer);
    buffer_arr[i] = malloc(buffer_arr_size[i]);
    if (!buffer_arr[i]) {
      fprintf(stderr, "Failed to allocate %lu bytes.\n", buffer_arr_size[i]);
      exit(3);
    }

    if (i == 0) {
      pattern_transform(g_power_of_3[i], g_slow_cube_vbuffer,
        sizeof(g_slow_cube_vbuffer)/sizeof(float), buffer_arr[i]);
    } else {
      pattern_transform(g_power_of_3[i], buffer_arr[i-1],
        buffer_arr_size[i-1]/sizeof(float), buffer_arr[i]);
    }
  }

//  pattern_transform(0, g_slow_cube_vbuffer,
//    sizeof(g_slow_cube_vbuffer)/sizeof(float), buffer_arr[g_recurse_depth]);
//  memcpy(buffer_arr[g_recurse_depth], g_slow_cube_vbuffer, sizeof(g_slow_cube_vbuffer));
/*
  for (i = 0; i < sizeof(g_slow_cube_vbuffer)/sizeof(float); ++i) {
    printf("(% f, % f, % f)\n",
      buffer_arr[g_recurse_depth][i],
      buffer_arr[g_recurse_depth][i],
      buffer_arr[g_recurse_depth][i]);
  }
  printf("\n");
*/
  glBindBuffer(GL_ARRAY_BUFFER, g_cube_vbuffer_id);
  glBufferData(GL_ARRAY_BUFFER, buffer_arr_size[g_recurse_depth],
    buffer_arr[g_recurse_depth], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  g_buffer_vert_count = buffer_arr_size[g_recurse_depth]/sizeof(float);
//  g_buffer_vert_count = sizeof(g_slow_cube_vbuffer)/sizeof(float);

  for (i = 0; i < g_recurse_depth; ++i) {
    free(buffer_arr[i]);
  }
}

GLuint g_program_id;

unsigned g_skip_sponge_redraw = 0;

void display() {
  if (g_skip_sponge_redraw) {
    --g_skip_sponge_redraw;
  } else {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (g_recurse_depth != g_last_recurse_depth) {
      g_last_recurse_depth = g_recurse_depth;
      recalculate_sponge();
    }

    setup_initial_transform();
//    setup_light();

    setup_world_camera();
    glUseProgram(g_program_id);

    float rot_matrix[16];
    if (0) {
      float rotx_matrix[16];
      m4f_fill_rotx_m4fo(M_PI_4/2, rotx_matrix);
      float roty_matrix[16];
      m4f_fill_roty_m4fo(M_PI_4/2, roty_matrix);
      float rotz_matrix[16];
      m4f_fill_roty_m4fo(M_PI_4/2, rotz_matrix);
      float rotxy_matrix[16];
      m4f_mul_m4fo(rotx_matrix, roty_matrix, rotxy_matrix);
      m4f_mul_m4fo(rotxy_matrix, rotz_matrix, rot_matrix);
    } else {
      m4f_copy_m4fo(g_identity, rot_matrix);
    }
  
    float init_matrix[16];
    m4f_copy_m4fo(g_identity, init_matrix);
    m4f_mul_m4f(g_proj, init_matrix);

    m4f_mul_m4fo(init_matrix, g_view, g_mvp_matrix);

//    m4f_print(g_mvp_matrix);
//    printf("\n");

    glUniformMatrix4fv(g_mvp_id, 1, GL_TRUE, g_mvp_matrix);
    glBindBuffer(GL_ARRAY_BUFFER, g_cube_vbuffer_id);
//    draw_menger_sponge(g_recurse_depth);
    glUniform4f(g_color_id, 1.0, 1.0, 1.0, 1.0);
/*    glDrawArrays(GL_TRIANGLES, 0, 6);
    glUniform4f(g_color_id, 0.0, 1.0, 1.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 6, 6);
    glUniform4f(g_color_id, 1.0, 0.0, 1.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 12, 6);
    glUniform4f(g_color_id, 1.0, 1.0, 0.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 18, 6);
    glUniform4f(g_color_id, 0.0, 0.0, 1.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 24, 6);
    glUniform4f(g_color_id, 0.0, 1.0, 0.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 30, 6);
    glUniform4f(g_color_id, 1.0, 0.0, 0.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 36, 6);
    glUniform4f(g_color_id, 0.25, 0.25, 0.25, 1.0);
    glDrawArrays(GL_TRIANGLES, 42, 6);
*/    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    draw_cube();
    glDrawArrays(GL_TRIANGLES, 0, g_buffer_vert_count);
//    draw_menger_sponge(g_recurse_depth);
//    teardown_light();
  }
//  reset_to(ORTHO);
//  draw_buttons();

  glutSwapBuffers();
}

char* load_file(const char* filename, int* length) {
  FILE* f = fopen(filename, "rb");
  char* contents = NULL;
  *length = 0;
  if (!f) {
    goto exit;
  }
  if (fseek(f, 0, SEEK_END)) {
    goto close_exit;
  }
  long fsize = ftell(f);
  if (fsize > INT_MAX) {
    goto close_exit;
  }
  *length = fsize;
  if (fseek(f, 0, SEEK_SET)) {
    goto close_exit;
  }
  contents = malloc(fsize + 1);
  if (!contents) {
    goto close_exit;
  }
  if (fread(contents, fsize, 1, f)) {
    goto close_exit;
  }
  contents[fsize] = '\0';
close_exit:
  fclose(f);
exit:
  return contents;
}

const int VERTEX_INDEX_IN_SHADER = 0;

void load_buffers() {
  glGenBuffers(1, &g_cube_vbuffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, g_cube_vbuffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_slow_cube_vbuffer),
    g_slow_cube_vbuffer, GL_STATIC_DRAW);
  glVertexAttribPointer(VERTEX_INDEX_IN_SHADER, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(VERTEX_INDEX_IN_SHADER);

  glGenBuffers(1, &g_cube_ibuffer_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_cube_ibuffer_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_cube_ibuffer),
    g_cube_ibuffer, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void compile_shader(GLuint shader_id) {
  glCompileShader(shader_id);

  GLint result = GL_FALSE;
  int info_log_length;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
  glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
  if (result == GL_FALSE) {
    char* error_message = malloc(info_log_length + 1);
    error_message[info_log_length] = '\0';
    glGetShaderInfoLog(shader_id, info_log_length, NULL, error_message);
    fprintf(stderr, "Shader compile failure!\n%s\n", error_message);
    free(error_message);
  }
}

void link_program(GLuint program_id) {
  glLinkProgram(program_id);

  GLint result = GL_FALSE;
  int info_log_length;
  glGetProgramiv(program_id, GL_LINK_STATUS, &result);
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
  if (result == GL_FALSE) {
    char* error_message = malloc(info_log_length + 1);
    error_message[info_log_length] = '\0';
    glGetProgramInfoLog(program_id, info_log_length, NULL, error_message);
    fprintf(stderr, "Shader link failure!\n%s\n", error_message);
    free(error_message);
  }
}

void load_shaders() {
  int vert_shader_length;
  char* vert_shader_text = load_file("basic.vert", &vert_shader_length);
  int frag_shader_length;
  char* frag_shader_text = load_file("basic.frag", &frag_shader_length);

  GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
  GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vert_shader, 1, (const GLchar**)&vert_shader_text, &vert_shader_length);
  glShaderSource(frag_shader, 1, (const GLchar**)&frag_shader_text, &frag_shader_length);

  compile_shader(vert_shader);
  compile_shader(frag_shader);

  GLuint program = glCreateProgram();
  glAttachShader(program, vert_shader);
  glAttachShader(program, frag_shader);
  link_program(program);

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  free(vert_shader_text);
  free(frag_shader_text);

  g_program_id = program;
}

void load_glew() {
  GLenum glew_status = glewInit();
  if (GLEW_OK != glew_status) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    exit(2);
  }
  printf("GLEW version: %s\n", glewGetString(GLEW_VERSION));
}

int check_version_string(const char* name, const char* version,
  long major_req, long minor_req, const char* version_req) {
  printf("%s version: %s\n", name, version);
  char* end;
  long major_num = strtol(version, &end, 10);
  if (*end == '.' && errno != EINVAL && errno != ERANGE) {
    if (major_num > major_req) {
      // all is well
    } else if (major_num < major_req) {
      fprintf(stderr, "Requires %s %s\n", name, version_req);
      return 1;
    } else {
      long minor_num = strtol(end + 1, NULL, 10);
      if (errno != EINVAL && errno != ERANGE) {
        if (minor_num < minor_req) {
          fprintf(stderr, "Requires %s %s\n", name, version_req);
          return 1;
        } else {
          // all is well
        }
      } else {
        fprintf(stderr, "%s minor version parse error\n", name);
        return 1;
      }
    }
  } else {
    fprintf(stderr, "%s major version parse error\n", name);
    return 1;
  }
  return 0;
}

#define COLOR_RED       "\x1b[31m"
#define COLOR_RESET     "\x1b[0m"

void check_minimum_opengl_version() {
  int fail = 0;
  fail |= check_version_string("OpenGL",
    (const char*)glGetString(GL_VERSION), 3, 0, "3.0");
  fail |= check_version_string("OpenGL Shading Language",
    (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION), 1, 3, "1.30");
  if (fail) {
    fprintf(stderr, COLOR_RED
      "You do not meet the minimum OpenGL requirements!\n"
      COLOR_RESET);
  }
}

void init() {
  check_minimum_opengl_version();
  load_glew();
  load_shaders();
  load_buffers();

  g_mvp_id = glGetUniformLocation(g_program_id, "mvp");
  g_color_id = glGetUniformLocation(g_program_id, "color");

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
//  glEnable(GL_CULL_FACE);
//  glMatrixMode(GL_PROJECTION);

//  init_light();
//  init_material();
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
      // the sponge hasn't changed
      g_skip_sponge_redraw += 1;
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
    float diff_x = 0.25 * (g_last_x - x);
    float diff_y = 0.25 * (g_last_y - y);

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
  LIGHT_ENTRY = 'l',
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
          printf("\nInvalid scale");
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
          printf("\nInvalid recursive depth");
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
        printf(" - cancelled\n");
      } else {
        printf("\nUnknown projection\n");
        printf("Known projections are:\n"
          "  o - orthographic\n"
          "  p - perspective\n");
      }
      g_command_state = NORMAL;
      break;
    case LIGHT_ENTRY:
      if (key == '0') {
        g_use_lighting = 0;
        glutPostRedisplay();
        printf("%c\n", key);
      } else if (key == '1') {
        g_use_lighting = 1;
        glutPostRedisplay();
        printf("%c\n", key);
      } else if (key == 27) {
        printf(" - cancelled\n");
      } else {
        printf("\nUnknown light state\n");
        printf("Known states are:\n"
          "  0 - off\n"
          "  1 - on\n");
      }
      g_command_state = NORMAL;
      break;
    default: // enter state
      g_command_state = key;
      switch(g_command_state) {
        case SCALE_ENTRY:
        case RECURSE_ENTRY:
        case PROJECTION_ENTRY:
        case LIGHT_ENTRY:
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
          printf("Unknown command: %c\n", key);
          printf("Known commands are:\n"
            "  l - lighting {0,1}\n"
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
  g_perm_camera_scale = 3.0 / g_power_of_3[g_recurse_depth];
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

void toggle_use_lighting() {
  g_use_lighting = g_use_lighting ? 0 : 1;
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
  g_button_callbacks[48] = toggle_use_lighting;
}

void calculate_normals() {
  size_t i;
  for (i = 0; i < g_cube_ibuffer_size; i += 3) {
    const float* a = vertex3f(g_cube_vbuffer, g_cube_ibuffer[i]);
    const float* b = vertex3f(g_cube_vbuffer, g_cube_ibuffer[i + 1]);
    const float* c = vertex3f(g_cube_vbuffer, g_cube_ibuffer[i + 2]);
    triangle_normal_3fo(a, b, c, g_cube_normal_buffer + i);
  }
}

void cleanup() {
  glDeleteBuffers(1, &g_cube_vbuffer_id);
  glDeleteBuffers(1, &g_cube_ibuffer_id);
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
  glutInitWindowSize(g_window_size_x, g_window_size_y);
  glutInitWindowPosition(100, 100);
  glutCreateWindow ("C Menger Sponge");
  init();
  setup_callbacks();
  calculate_normals();
  display();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse_press);
  glutMotionFunc(mouse_move);
  glutKeyboardFunc(key_press);
  glutMainLoop();
  return 0;
}
