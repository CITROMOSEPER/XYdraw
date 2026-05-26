// XYdraw library regular voxel rendering
// ESP32
// Nemes Dániel
//
// This file demonstrates the regular voxel rendering capability of the XYdraw library
//
// render_voxels(bool voxels[], unsigned int x_size, unsigned int y_size, unsigned int z_size,
//               unsigned int voxel_size,
//               int x_pos,   int y_pos,   int z_pos,
//               int cam_x,   int cam_y,   int cam_z,
//               int cam_x_dir, int cam_y_dir, int cam_z_dir);


#include <Arduino.h>
#include <GLOBALS.h>
#include <PINS.h>

#include <XYdraw.h>
XYdraw draw(CH_X_PIN, CH_Y_PIN, 5);


// ── Voxel grid ────────────────────────────────────────────────────────────────
// 3x3x3 grid: full ground floor (y=0) plus a centre pillar going up to y=2.
// Index layout: y*(X_SIZE*Z_SIZE) + x*Z_SIZE + z
const unsigned int X_SIZE     = 3;
const unsigned int Y_SIZE     = 3;
const unsigned int Z_SIZE     = 3;
const unsigned int VOXEL_SIZE = 20;
const int          GRID_X_POS = -30;
const int          GRID_Y_POS = -30;
const int          GRID_Z_POS =  50;

bool voxels[X_SIZE * Y_SIZE * Z_SIZE] =
{
  // y=0: full floor (x=0,1,2 each with z=0,1,2)
  1, 1, 1,
  1, 1, 1,
  1, 1, 1,
  // y=1: centre pillar only (x=1, z=1)
  0, 0, 0,
  0, 1, 0,
  0, 0, 0,
  // y=2: centre pillar only
  0, 0, 0,
  0, 1, 0,
  0, 0, 0,
};

// ── Camera state ──────────────────────────────────────────────────────────────
float cam_x     = 0.0f;
float cam_y     = 0.0f;
float cam_z     = 0.0f;
float cam_yaw   = 0.0f;  // degrees, positive = turn right
float cam_pitch = 0.0f;  // degrees, positive = look up

// Tune these to taste.
const float MOVE_SPEED  = 80.0f;   // world units per second
const float LOOK_SPEED  = 90.0f;   // degrees per second
const float PITCH_LIMIT = 89.0f;   // prevents flipping over at the poles

// ── Timing ───────────────────────────────────────────────────────────────────
unsigned long last_time = 0;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main functions

void setup()
{
  INITIALIZE_PINS();
  draw.begin();

  // Compute the world-space centre of the voxel grid.
  float center_x = GRID_X_POS + X_SIZE * VOXEL_SIZE * 0.5f;
  float center_y = GRID_Y_POS + Y_SIZE * VOXEL_SIZE * 0.5f;
  float center_z = GRID_Z_POS + Z_SIZE * VOXEL_SIZE * 0.5f;

  // Place the camera at a 45-degree orbit angle, 20 degrees above the
  // horizontal, and 150 units away so the whole grid fits in frame.
  const float INIT_YAW   = 45.0f;
  const float INIT_PITCH = 10.0f;
  const float INIT_DIST  = 150.0f;

  float yaw_rad   = radians(INIT_YAW);
  float pitch_rad = radians(INIT_PITCH);

  cam_x = center_x + INIT_DIST * sinf(yaw_rad) * cosf(pitch_rad);
  cam_y = center_y + INIT_DIST * sinf(pitch_rad);
  cam_z = center_z + INIT_DIST * cosf(yaw_rad) * cosf(pitch_rad);

  cam_yaw   = INIT_YAW + 180.0f;
  cam_pitch = -INIT_PITCH;

  last_time = millis();
}

void loop()
{
  HANDLE_GLOBAL_INPUTS();

  // ── Delta time ──────────────────────────────────────────────────────────────
  unsigned long now = millis();
  float dt = (now - last_time) / 1000.0f;
  last_time = now;
  if (dt > 0.1f) dt = 0.1f;  // cap to avoid big jumps after stalls

  // ── Look (R joystick) ───────────────────────────────────────────────────────
  // JOY_R_X uses a -2 hardware multiplier, so negating it restores the
  // natural convention: push right --> look right (yaw increases).
  // JOY_R_Y uses a +2 hardware multiplier and a pulled-forward-negative ADC
  // convention, so negating it gives: push forward --> look up (pitch increases).
  cam_yaw   += JOY_R_X * LOOK_SPEED * dt;
  cam_pitch -= JOY_R_Y * LOOK_SPEED * dt;

  if (cam_pitch >  PITCH_LIMIT) cam_pitch =  PITCH_LIMIT;
  if (cam_pitch < -PITCH_LIMIT) cam_pitch = -PITCH_LIMIT;

  // ── Move (L joystick + triggers) ────────────────────────────────────────────
  // Compute horizontal forward and right unit vectors from the current yaw.
  // Vertical pitch is intentionally excluded from forward/strafe so that
  // looking up or down does not cause the camera to drift vertically.
  float yaw_rad = radians(cam_yaw);
  float fwd_x   = sinf(yaw_rad);
  float fwd_z   = cosf(yaw_rad);
  float right_x = cosf(yaw_rad);
  float right_z = -sinf(yaw_rad);

  // JOY_L_Y: push forward --> positive (moves camera forward).
  // JOY_L_X: push right   --> positive (strafes camera right).
  float move = MOVE_SPEED * dt;
  float forward_input =  JOY_L_Y;
  float strafe_input  =  JOY_L_X;

  cam_x += (fwd_x * forward_input + right_x * strafe_input) * move;
  cam_z += (fwd_z * forward_input + right_z * strafe_input) * move;

  // LT lowers the camera, RT raises it.
  cam_y += (RT - LT) * move;

  // ── Build renderer direction integers ──────────────────────────────────────
  // cam_x_dir is pitch: negative means looking down.
  // cam_y_dir is yaw:   the renderer expects the horizontal look angle.
  // cam_z_dir is roll:  always 0.
  int cam_x_dir = (int)cam_pitch;
  int cam_y_dir = (int)cam_yaw;
  int cam_z_dir = 0;

  // ── Render ──────────────────────────────────────────────────────────────────
  // Wireframe mode: all edges drawn, no occlusion.
  draw.render_voxels(voxels, X_SIZE, Y_SIZE, Z_SIZE, VOXEL_SIZE,
                     GRID_X_POS, GRID_Y_POS, GRID_Z_POS,
                     (int)cam_x, (int)cam_y, (int)cam_z,
                     cam_x_dir, cam_y_dir, cam_z_dir);
}