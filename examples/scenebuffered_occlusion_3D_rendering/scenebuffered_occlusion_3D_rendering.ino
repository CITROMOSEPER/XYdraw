// XYdraw library scenebuffered, screen-space occlusion 3D rendering demonstration
// ESP32
// Nemes Dániel
//
// This file demonstrates the scenebuffered, screen-space occlusion 3D rendering
// capability of the XYdraw library.
//
// ── How the scene buffer differs from regular 3D rendering ───────────────────
// In regular mode every threeD_line() call is drawn immediately, with no
// awareness of other objects. In scenebuffered mode you first stage all
// geometry into a buffer, then call render_scenebuffer() once. During that
// single call the library:
//   1. Projects all polygon faces into screen space.
//   2. For every line segment (and every polygon outline edge), splits it
//      wherever it crosses a projected triangle edge.
//   3. Tests each resulting sub-segment at its midpoint: if the midpoint falls
//      inside a projected triangle AND the polygon is closer to the camera,
//      the sub-segment is hidden and not drawn.
// This produces proper hidden-line removal — back-facing and occluded edges
// disappear as expected for a solid object.
//
// Functions demonstrated:
//   void clear_scenebuffer();
//   void add_threeD_line_to_scenebuffer(int start_x, int start_y, int start_z,
//                                       int end_x,   int end_y,   int end_z);
//   void add_polygon_to_scenebuffer(int p1_x, int p1_y, int p1_z,
//                                   int p2_x, int p2_y, int p2_z,
//                                   int p3_x, int p3_y, int p3_z);
//   void render_scenebuffer(int cam_x,     int cam_y,     int cam_z,
//                           int cam_x_dir, int cam_y_dir, int cam_z_dir);
//
// ── Important: triangulate all faces ─────────────────────────────────────────
// The scene buffer only accepts triangular faces. Rectangular walls must be
// split into two right triangles sharing the diagonal. The cube below has
// five rectangular faces (four walls + floor) — each is two triangles.


#include <XYdraw.h>
XYdraw draw(25, 26, 20);  // This is the constructor function. Its arguments are: X channel pin, Y channel pin, refine value (this can be changed later)


// Orbit parameters
const float ORBIT_RADIUS = 150.0;  // horizontal distance from cube centre
const float CAM_HEIGHT   = 120.0;  // height above cube centre (Y axis)
const float ORBIT_STEP   =   0.1;  // degrees advanced per frame

int   cam_pitch = 0;      // constant downward tilt, computed in setup()
float orbit_angle = 0.0;  // current orbit angle in degrees


void draw_scenebuffered_cube(int cam_x, int cam_y, int cam_z, int pitch, int yaw)
{
  draw.clear_scenebuffer();

  // rooftop — 4 triangular faces
  draw.add_polygon_to_scenebuffer(  0, 100,   0,  100, 100,   0,  50, 150,  50);  // front  (z = 0)
  draw.add_polygon_to_scenebuffer(  0, 100, 100,  100, 100, 100,  50, 150,  50);  // back   (z = 100)
  draw.add_polygon_to_scenebuffer(  0, 100,   0,    0, 100, 100,  50, 150,  50);  // left   (x = 0)
  draw.add_polygon_to_scenebuffer(100, 100,   0,  100, 100, 100,  50, 150,  50);  // right  (x = 100)

  // front wall (z = 0) — 2 triangles
  draw.add_polygon_to_scenebuffer(  0,   0,   0,  100,   0,   0,  100, 100,   0);
  draw.add_polygon_to_scenebuffer(  0,   0,   0,  100, 100,   0,    0, 100,   0);

  // back wall (z = 100) — 2 triangles
  draw.add_polygon_to_scenebuffer(  0,   0, 100,  100,   0, 100,  100, 100, 100);
  draw.add_polygon_to_scenebuffer(  0,   0, 100,  100, 100, 100,    0, 100, 100);

  // left wall (x = 0) — 2 triangles
  draw.add_polygon_to_scenebuffer(  0,   0,   0,    0,   0, 100,    0, 100, 100);
  draw.add_polygon_to_scenebuffer(  0,   0,   0,    0, 100, 100,    0, 100,   0);

  // right wall (x = 100) — 2 triangles
  draw.add_polygon_to_scenebuffer(100,   0,   0,  100,   0, 100,  100, 100, 100);
  draw.add_polygon_to_scenebuffer(100,   0,   0,  100, 100, 100,  100, 100,   0);

  // bottom floor (y = 0) — 2 triangles
  draw.add_polygon_to_scenebuffer(  0,   0,   0,  100,   0,   0,  100,   0, 100);
  draw.add_polygon_to_scenebuffer(  0,   0,   0,  100,   0, 100,    0,   0, 100);

  draw.render_scenebuffer(cam_x, cam_y, cam_z, pitch, yaw, 0);
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

  draw_scenebuffered_cube(cam_x, cam_y, cam_z, cam_pitch, cam_yaw);

  orbit_angle += ORBIT_STEP;
  if (orbit_angle >= 360.0f) orbit_angle -= 360.0f;
}