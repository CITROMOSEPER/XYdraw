// XYdraw library regular 3D rendering demonstration
// ESP32
// Nemes Dániel
//
// This file demonstrates the regular 3D rendering capability of the XYdraw library.
// In "regular" mode every drawing call immediately outputs DAC values — there is no
// scene buffer and no occlusion: all edges are drawn even if a face should hide them.
// This mode is the fastest option and works well for wireframe objects.
//
// Functions demonstrated:
//   void threeD_line(int start_x,   int start_y,   int start_z,
//                    int end_x,     int end_y,     int end_z,
//                    int cam_x,     int cam_y,     int cam_z,
//                    int cam_x_dir, int cam_y_dir, int cam_z_dir);
//
//   void polygon(int p1_x, int p1_y, int p1_z,
//                int p2_x, int p2_y, int p2_z,
//                int p3_x, int p3_y, int p3_z,
//                int cam_x,     int cam_y,     int cam_z,
//                int cam_x_dir, int cam_y_dir, int cam_z_dir);
//
// Camera orientation angles (all in degrees):
//   cam_x_dir = pitch  — tilt up (+) or down (−)
//   cam_y_dir = yaw    — turn right (+) or left (−)
//   cam_z_dir = roll   — clockwise (+) or counter-clockwise (−) tilt
//
// Coordinate system:
//   World space uses arbitrary integer units.
//   The 3D projection maps the camera's view frustum onto the 0–255 DAC canvas.
//   Screen centre is DAC value (128, 128); the focal length is 128 px (≈ 90° H-FOV).
//   Positive Y is up, positive X is right, positive Z is into the scene.


#include <XYdraw.h>
XYdraw draw(25, 26, 20);  // This is the constructor function. Its arguments are: X channel pin, Y channel pin, refine value (this can be changed later)


// Orbit parameters
const float ORBIT_RADIUS = 150.0;  // horizontal distance from cube centre
const float CAM_HEIGHT   = 120.0;  // height above cube centre (Y axis)
const float ORBIT_STEP   =   0.1;  // degrees advanced per frame

int   cam_pitch = 0;      // constant downward tilt, computed in setup()
float orbit_angle = 0.0;  // current orbit angle in degrees


void draw_cube(int cam_x, int cam_y, int cam_z, int pitch, int yaw)
{
  // bottom face (y = 0)
  draw.threeD_line(  0,   0,   0,  100,   0,   0,  cam_x, cam_y, cam_z, pitch, yaw, 0);
  draw.threeD_line(100,   0,   0,  100,   0, 100,  cam_x, cam_y, cam_z, pitch, yaw, 0);
  draw.threeD_line(100,   0, 100,    0,   0, 100,  cam_x, cam_y, cam_z, pitch, yaw, 0);
  draw.threeD_line(  0,   0, 100,    0,   0,   0,  cam_x, cam_y, cam_z, pitch, yaw, 0);

  // top face (y = 100)
  draw.threeD_line(  0, 100,   0,  100, 100,   0,  cam_x, cam_y, cam_z, pitch, yaw, 0);
  draw.threeD_line(100, 100,   0,  100, 100, 100,  cam_x, cam_y, cam_z, pitch, yaw, 0);
  draw.threeD_line(100, 100, 100,    0, 100, 100,  cam_x, cam_y, cam_z, pitch, yaw, 0);
  draw.threeD_line(  0, 100, 100,    0, 100,   0,  cam_x, cam_y, cam_z, pitch, yaw, 0);

  // vertical edges
  draw.threeD_line(  0,   0,   0,    0, 100,   0,  cam_x, cam_y, cam_z, pitch, yaw, 0);
  draw.threeD_line(100,   0,   0,  100, 100,   0,  cam_x, cam_y, cam_z, pitch, yaw, 0);
  draw.threeD_line(  0,   0, 100,    0, 100, 100,  cam_x, cam_y, cam_z, pitch, yaw, 0);
  draw.threeD_line(100,   0, 100,  100, 100, 100,  cam_x, cam_y, cam_z, pitch, yaw, 0);

  // rooftop made of 4 polygons
  draw.polygon(  0, 100,   0,  100, 100,   0,  50, 150,  50,  cam_x, cam_y, cam_z, pitch, yaw, 0);  // front  (z = 0)
  draw.polygon(  0, 100, 100,  100, 100, 100,  50, 150,  50,  cam_x, cam_y, cam_z, pitch, yaw, 0);  // back   (z = 100)
  draw.polygon(  0, 100,   0,    0, 100, 100,  50, 150,  50,  cam_x, cam_y, cam_z, pitch, yaw, 0);  // left   (x = 0)
  draw.polygon(100, 100,   0,  100, 100, 100,  50, 150,  50,  cam_x, cam_y, cam_z, pitch, yaw, 0);  // right  (x = 100)
}


void setup()
{
  draw.begin();

  // Pitch stays constant because ORBIT_RADIUS and CAM_HEIGHT never change, so the camera-to-cube distance and downward angle are fixed throughout the orbit.
  float dist = sqrt(ORBIT_RADIUS * ORBIT_RADIUS + CAM_HEIGHT * CAM_HEIGHT);
  cam_pitch  = (int)degrees(asin(CAM_HEIGHT / dist));
}

void loop()
{
  float a = radians(orbit_angle);

  // Camera position: fixed Y height, rotating in the XZ plane around cube centre
  int cam_x = (int)(50.0f + ORBIT_RADIUS * cosf(a));
  int cam_y = (int)(50.0f + CAM_HEIGHT);
  int cam_z = (int)(50.0f + ORBIT_RADIUS * sinf(a));

  // Yaw: derived from the horizontal direction from the camera to the cube centre.
  // atan2(dx, dz) gives the correct left/right angle for the library's YXZ convention.
  int cam_yaw = (int)degrees(atan2f(50.0f - cam_x, 50.0f - cam_z));

  draw_cube(cam_x, cam_y, cam_z, cam_pitch, cam_yaw);

  orbit_angle += ORBIT_STEP;
  if (orbit_angle >= 360.0f) orbit_angle -= 360.0f;
}