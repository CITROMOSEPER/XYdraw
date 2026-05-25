// XYdraw library source file
// ESP32
// Nemes Dániel


#include <Arduino.h>
#include <XYdraw.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup functions

// Constructor funciton
XYdraw::XYdraw(int ch_x_pin, int ch_y_pin, unsigned int refine)
{
  _refine = refine;
  _ch_x_pin = ch_x_pin;
  _ch_y_pin = ch_y_pin;
  _fb_line_count = 0;
  _fb_poly_count = 0;
  _cam_x = 0;
  _cam_y = 0;
  _cam_z = 0;
}

// Initializer function
void XYdraw::begin()
{
  pinMode(_ch_x_pin, OUTPUT);
  pinMode(_ch_y_pin, OUTPUT);
}

// Set the quality of splines
void XYdraw::set_refine(unsigned int refine)
{
  if (refine == 0)  refine = 1;
  _refine = refine;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2D rendering functions

// draws a line from 4 coordinates
void XYdraw::line(int start_x, int start_y, int end_x, int end_y)
{
  for (int i = 0; i <= _refine; i++)
  {
    float t = (float)i / (float)_refine;
    
    // Linear interpolation between the start and end points
    float x = start_x + t * (end_x - start_x);
    float y = start_y + t * (end_y - start_y);

    // Ensure values are within the DAC range (0-255)
    int dac_x = constrain((int)x, 0, 255);
    int dac_y = constrain((int)y, 0, 255);

    dacWrite(_ch_x_pin, dac_x);
    dacWrite(_ch_y_pin, dac_y);

    // delay(1); // Small delay to ensure DAC output stabilization
  }
}

// draws a rectangle from two coordinates and two side lengths
void XYdraw::rectangle(int x, int y, int x_length, int y_length)
{
  line(x, y, x + x_length, y);
  line(x + x_length, y, x + x_length, y + y_length);
  line(x + x_length, y + y_length, x, y + y_length);
  line(x, y + y_length, x, y);
}

// draws a circle from 2 coordinates and a radius
void XYdraw::circle(int origin_x, int origin_y, int radius)
{
  for (float i = 0; i < 360.0; i += 360.0 / (float)_refine)
  {
    dacWrite(_ch_x_pin, constrain((int)(origin_x + sin(radians(i)) * radius), 0, 255));
    dacWrite(_ch_y_pin, constrain((int)(origin_y + cos(radians(i)) * radius), 0, 255));
  }
  // delay(1);
}

// draws an ellipse from 2 coordinates and two radii
void XYdraw::ellipse(int origin_x, int origin_y, int radius_x, int radius_y)
{
  // Iterate through angles from 0 to 360 degrees
  for (float i = 0; i < 360.0; i += 360.0 / (float)_refine)
  {
    // Calculate x and y positions based on ellipse parametric equations
    int x = origin_x + (radius_x * sin(radians(i)));
    int y = origin_y + (radius_y * cos(radians(i)));

    // Ensure values are within the DAC range (0-255)
    int dac_x = constrain((int)x, 0, 255);
    int dac_y = constrain((int)y, 0, 255);

    dacWrite(_ch_x_pin, dac_x);
    dacWrite(_ch_y_pin, dac_y);
  }
  // delay(1);
}

// Renders an array in raster format
void XYdraw::render_raster(bool raster[], int x_size, int y_size)
{
  int temporaryRefine = _refine;
  _refine = 2;

  float x_scale = 255.00 / x_size;
  float y_scale = 255.00 / y_size;

  int i = 0;
  for (int y = 0; y < y_size; y++)
  {
    for (int x = 0; x < x_size; x++)
    {
      if (raster[i] == 1)
      {
        rectangle(x * x_scale, y * y_scale, x_scale, y_scale);
      }
      i++;
    }
  }

  _refine = temporaryRefine;
}

// draws a bézier curve from 4 control point coordinates
void XYdraw::bezier_curve(int anchor1_x, int anchor1_y, int anchor2_x, int anchor2_y, int cp1_x, int cp1_y, int cp2_x, int cp2_y)
{
  for (int i = 0; i <= _refine; i++)
  {
    float t = (float)i / (float)_refine;

    // Linear interpolations for the first level
    float x0 = (1 - t) * anchor1_x + t * cp1_x;
    float y0 = (1 - t) * anchor1_y + t * cp1_y;
    float x1 = (1 - t) * cp1_x + t * cp2_x;
    float y1 = (1 - t) * cp1_y + t * cp2_y;
    float x2 = (1 - t) * cp2_x + t * anchor2_x;
    float y2 = (1 - t) * cp2_y + t * anchor2_y;

    // Linear interpolations for the second level
    float x3 = (1 - t) * x0 + t * x1;
    float y3 = (1 - t) * y0 + t * y1;
    float x4 = (1 - t) * x1 + t * x2;
    float y4 = (1 - t) * y1 + t * y2;

    // Linear interpolation for the third level
    float x = (1 - t) * x3 + t * x4;
    float y = (1 - t) * y3 + t * y4;

    // Ensure values are within the DAC range (0-255)
    int dac_x = constrain((int)x, 0, 255);
    int dac_y = constrain((int)y, 0, 255);

    dacWrite(_ch_x_pin, dac_x);
    dacWrite(_ch_y_pin, dac_y);

    // delay(1); // Small delay to ensure DAC output stabilization
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Regular 3D rendering functions

void XYdraw::polygon(int p1_x,      int p1_y,      int p1_z,
                     int p2_x,      int p2_y,      int p2_z,
                     int p3_x,      int p3_y,      int p3_z,
                     int cam_x,     int cam_y,     int cam_z,
                     int cam_x_dir, int cam_y_dir, int cam_z_dir)
{
  threeD_line(p1_x,      p1_y,      p1_z,
              p2_x,      p2_y,      p2_z,
              cam_x,     cam_y,     cam_z,
              cam_x_dir, cam_y_dir, cam_z_dir);
  threeD_line(p2_x,      p2_y,      p2_z,
              p3_x,      p3_y,      p3_z,
              cam_x,     cam_y,     cam_z,
              cam_x_dir, cam_y_dir, cam_z_dir);
  threeD_line(p3_x,      p3_y,      p3_z,
              p1_x,      p1_y,      p1_z,
              cam_x,     cam_y,     cam_z,
              cam_x_dir, cam_y_dir, cam_z_dir);
}

static bool lb_clip_screen(float& sx, float& sy, float& ex, float& ey, float xmin, float xmax, float ymin, float ymax)
{
  // ─── Liang–Barsky 2-D screen-space line clipper ──────────────────────────────
  // Clips segment (sx,sy)→(ex,ey) to [xmin,xmax]×[ymin,ymax] in centred screen
  // coordinates.  Modifies endpoints in-place.  Returns false if the segment
  // lies entirely outside (caller should not draw).
  // Used by both threeD_line and render_scenebuffer.

  float dx = ex - sx;
  float dy = ey - sy;
  float t0 = 0.0f;
  float t1 = 1.0f;

  // Four boundary tests: left, right, bottom, top.
  // p[i] = rate of approach to the outside; q[i] = headroom inside.
  float p[4] = { -dx,       dx,       -dy,       dy       };
  float q[4] = { sx - xmin, xmax - sx, sy - ymin, ymax - sy };

  for (int i = 0; i < 4; i++)
  {
    if (fabsf(p[i]) < 1e-7f)        // segment parallel to this edge
    {
      if (q[i] < 0.0f) return false; // lies outside — discard entirely
      continue;
    }
    float r = q[i] / p[i];
    if (p[i] < 0.0f) { if (r > t0) t0 = r; }
    else              { if (r < t1) t1 = r; }
    if (t0 > t1) return false;       // clipped to nothing
  }

  // Update endpoints in the correct order (end before start).
  ex = sx + t1 * dx;
  ey = sy + t1 * dy;
  sx = sx + t0 * dx;
  sy = sy + t0 * dy;
  return true;
}

void XYdraw::threeD_line(int start_x,   int start_y,   int start_z,
                         int end_x,     int end_y,     int end_z,
                         int cam_x,     int cam_y,     int cam_z,
                         int cam_x_dir, int cam_y_dir, int cam_z_dir)
{
  // draws a line in 3D space.
  //
  // World-space line endpoints: (start_x, start_y, start_z) → (end_x, end_y, end_z)
  // Camera position:            (cam_x, cam_y, cam_z)
  // Camera orientation Euler angles in degrees:
  //   cam_x_dir = pitch  (rotation around X – look up/down)
  //   cam_y_dir = yaw    (rotation around Y – look left/right)
  //   cam_z_dir = roll   (rotation around Z – tilt)
  //
  // Pipeline:
  //   1. Translate each world point relative to camera position.
  //   2. Rotate into camera space using the inverse (= transpose) of the camera's
  //      orientation matrix, built as Rx(pitch)·Ry(yaw)·Rz(roll).
  //      Inverse applied as: Rz(-roll) · Ry(-yaw) · Rx(-pitch)
  //   3. Clip against near plane (cz < 1) to avoid divide-by-zero / inversion.
  //   4. Perspective-project with focal length 128 px (≈90° H-FOV on 256-px canvas).
  //   5. Clip projected line to screen bounds with Liang–Barsky and draw.

  // ── Precompute trig for each rotation angle ──────────────────────────────
  float pitch = radians((float)cam_x_dir);
  float yaw   = radians((float)cam_y_dir);
  float roll  = radians((float)cam_z_dir);

  float cp = cos(pitch), sp = sin(pitch);  // pitch (X)
  float cy = cos(yaw),   sy = sin(yaw);    // yaw   (Y)
  float cr = cos(roll),  sr = sin(roll);   // roll  (Z)

  // ── Helper: world point → camera-space point ─────────────────────────────
  // Camera orientation C = Ry(yaw)·Rx(pitch)·Rz(roll)  (FPS intrinsic YXZ).
  // View transform = C^T = Rz(-roll)·Rx(-pitch)·Ry(-yaw), applied left-to-right:
  //
  //   Step 1  Ry(-yaw)    :  x'= cy·x - sy·z,  y'= y,  z'= sy·x + cy·z
  //   Step 2  Rx(-pitch)  :  x"= x',  y"= cp·y' + sp·z',  z"= -sp·y' + cp·z'
  //   Step 3  Rz(-roll)   :  cx= cr·x" + sr·y",  cy= -sr·x" + cr·y",  cz= z"

  // — Project start point —
  float tx = (float)(start_x - cam_x);
  float ty = (float)(start_y - cam_y);
  float tz = (float)(start_z - cam_z);

  // Step 1: Ry(-yaw)  — turn around world Y first
  float rx1 = cy * tx - sy * tz;
  float ry1 = ty;
  float rz1 = sy * tx + cy * tz;

  // Step 2: Rx(-pitch)  — tilt around camera LOCAL X (after yaw)
  // Swapping these two steps is what makes pitch behave correctly when
  // the camera is yawed: with Rx first the rotation was around world X,
  // causing the scene to spin.  With Ry first, Rx acts on the already-
  // yawed frame, so pitch always tilts straight up/down on screen.
  float rx2 = rx1;
  float ry2 =  cp * ry1 + sp * rz1;
  float rz2 = -sp * ry1 + cp * rz1;

  // Step 3: Rz(-roll)
  float s_cx =  cr * rx2 + sr * ry2;
  float s_cy = -sr * rx2 + cr * ry2;
  float s_cz =  rz2;

  // — Project end point —
  tx = (float)(end_x - cam_x);
  ty = (float)(end_y - cam_y);
  tz = (float)(end_z - cam_z);

  // Step 1: Ry(-yaw)
  rx1 = cy * tx - sy * tz;
  ry1 = ty;
  rz1 = sy * tx + cy * tz;

  // Step 2: Rx(-pitch)
  rx2 = rx1;
  ry2 =  cp * ry1 + sp * rz1;
  rz2 = -sp * ry1 + cp * rz1;

  // Step 3: Rz(-roll)
  float e_cx =  cr * rx2 + sr * ry2;
  float e_cy = -sr * rx2 + cr * ry2;
  float e_cz =  rz2;

  // ── Near-plane clipping (cz >= NEAR required for valid projection) ────────
  const float NEAR = 1.0f;

  if (s_cz < NEAR && e_cz < NEAR)
    return;  // Entire line is behind the camera — nothing to draw.

  // Clip whichever endpoint lies behind the near plane.
  // Only one branch fires because we already returned when both are behind.
  if (s_cz < NEAR)
  {
    // Parametric intersection of segment with the plane cz = NEAR.
    float t = (NEAR - s_cz) / (e_cz - s_cz);
    s_cx = s_cx + t * (e_cx - s_cx);
    s_cy = s_cy + t * (e_cy - s_cy);
    s_cz = NEAR;
  }
  else if (e_cz < NEAR)
  {
    float t = (NEAR - e_cz) / (s_cz - e_cz);
    e_cx = e_cx + t * (s_cx - e_cx);
    e_cy = e_cy + t * (s_cy - e_cy);
    e_cz = NEAR;
  }

  // ── Perspective projection ────────────────────────────────────────────────
  // Screen centre is at DAC value 128; focal length 128 gives ~90° H-FOV.
  const float FOCAL = 128.0f;

  float proj_sx = FOCAL * s_cx / s_cz;
  float proj_sy = FOCAL * s_cy / s_cz;
  float proj_ex = FOCAL * e_cx / e_cz;
  float proj_ey = FOCAL * e_cy / e_cz;

  // ── 2-D screen clip then draw ─────────────────────────────────────────────
  // Screen bounds in centred coordinates: [-128, 127] maps to DAC [0, 255].
  // lb_clip_screen properly cuts lines at the screen boundary instead of
  // clamping endpoints — without this, off-screen vertices would be pinned to
  // a screen edge and the line drawn from there, producing completely wrong
  // geometry (especially visible when the camera is close to objects).
  if (!lb_clip_screen(proj_sx, proj_sy, proj_ex, proj_ey,
                      -128.0f, 127.0f, -128.0f, 127.0f))
    return;

  // After clipping, coordinates are guaranteed within [-128, 127] — safe cast.
  line((int)(128.0f + proj_sx), (int)(128.0f + proj_sy),
       (int)(128.0f + proj_ex), (int)(128.0f + proj_ey));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// scenebuffered (occlusion) 3D rendering functions
//
// Usage per frame:
//   clear_scenebuffer()                  — discard previous frame's objects
//   add_threeD_line_to_scenebuffer(...)  — stage line segments
//   add_polygon_to_scenebuffer(...)      — stage opaque triangles
//   render_scenebuffer(cam_...)          — project, occlude, and draw everything
//
// Occlusion model
// ───────────────
// Polygons are opaque from both faces (no back-face culling).  A line (or
// polygon outline edge) is split at every screen-space triangle edge crossing.
// Each sub-interval is tested at its midpoint: if the midpoint falls inside a
// projected triangle AND the polygon is closer to the camera than the line at
// that point, the sub-interval is hidden and skipped.
//
// DEPTH_EPS prevents a polygon from occluding its own outline edges (they
// sit co-planar with the polygon's surface).

void XYdraw::clear_scenebuffer()
{
  _fb_line_count = 0;
  _fb_poly_count = 0;
}

void XYdraw::add_threeD_line_to_scenebuffer(int start_x, int start_y, int start_z,
                                            int end_x,   int end_y,   int end_z)
{
  if (_fb_line_count >= FB_MAX_LINES) return;
  FBLine& l  = _fb_lines[_fb_line_count++];
  l.sx = (int16_t)start_x;  l.sy = (int16_t)start_y;  l.sz = (int16_t)start_z;
  l.ex = (int16_t)end_x;    l.ey = (int16_t)end_y;    l.ez = (int16_t)end_z;
}

void XYdraw::add_polygon_to_scenebuffer(int p1_x, int p1_y, int p1_z,
                                        int p2_x, int p2_y, int p2_z,
                                        int p3_x, int p3_y, int p3_z)
{
  if (_fb_poly_count >= FB_MAX_POLYGONS) return;
  FBPoly& p   = _fb_polys[_fb_poly_count++];
  p.p1x = (int16_t)p1_x;  p.p1y = (int16_t)p1_y;  p.p1z = (int16_t)p1_z;
  p.p2x = (int16_t)p2_x;  p.p2y = (int16_t)p2_y;  p.p2z = (int16_t)p2_z;
  p.p3x = (int16_t)p3_x;  p.p3y = (int16_t)p3_y;  p.p3z = (int16_t)p3_z;
}

void XYdraw::render_scenebuffer(int cam_x,     int cam_y,     int cam_z,
                                int cam_x_dir, int cam_y_dir, int cam_z_dir)
{
  _cam_x = cam_x;  _cam_y = cam_y;  _cam_z = cam_z;

  // ── Constants ──────────────────────────────────────────────────────────────
  const float NEAR      = 1.0f;
  const float FOCAL     = 128.0f;
  const float DEPTH_EPS = 0.5f;
  const int   T_CAP     = FB_MAX_POLYGONS * 6 + 2;

  // ── Camera trig ────────────────────────────────────────────────────────────
  float pitch = radians((float)cam_x_dir);
  float yaw   = radians((float)cam_y_dir);
  float roll  = radians((float)cam_z_dir);

  float cp = cosf(pitch), sp = sinf(pitch);
  float cosy = cosf(yaw),  siny = sinf(yaw);
  float cr = cosf(roll),   sr = sinf(roll);

  // ── World → camera-space (mirrors threeD_line exactly) ────────────────────
  auto to_cam = [=](float wx, float wy, float wz,
                     float& ocx, float& ocy, float& ocz)
  {
    float tx = wx - (float)cam_x;
    float ty = wy - (float)cam_y;
    float tz = wz - (float)cam_z;
    float rx1 =  cosy * tx - siny * tz;
    float ry1 =  ty;
    float rz1 =  siny * tx + cosy * tz;
    float ry2 =  cp * ry1 + sp * rz1;
    float rz2 = -sp * ry1 + cp * rz1;
    ocx =  cr * rx1 + sr * ry2;
    ocy = -sr * rx1 + cr * ry2;
    ocz =  rz2;
  };

  // ── Near-plane edge clip helper ────────────────────────────────────────────
  // Given segment a→b where az >= NEAR and bz < NEAR, returns the point on
  // the segment where cz == NEAR.
  auto clip_near_edge = [](float ax, float ay, float az,
                            float bx, float by, float bz,
                            float& ox, float& oy, float& oz)
  {
    float t = (1.0f - az) / (bz - az);
    ox = ax + t * (bx - ax);
    oy = ay + t * (by - ay);
    oz = 1.0f;
  };

  // ── Projected-triangle storage ─────────────────────────────────────────────
  struct ProjTri
  {
    float sx[3], sy[3];         // screen-space vertices, centred at 0
    float Nx, Ny, Nz, D;       // camera-space plane: N·P = D
    float minx, maxx, miny, maxy; // screen-space AABB for fast pre-reject
  };
  static ProjTri proj_tris[FB_MAX_POLYGONS * 2];
  static int     n_tris;
  n_tris = 0;

  auto add_proj_tri = [&](float cx0, float cy0, float cz0,
                           float cx1, float cy1, float cz1,
                           float cx2, float cy2, float cz2)
  {
    if (n_tris >= FB_MAX_POLYGONS * 2) return;
    ProjTri& t = proj_tris[n_tris++];
    t.sx[0] = FOCAL * cx0 / cz0;  t.sy[0] = FOCAL * cy0 / cz0;
    t.sx[1] = FOCAL * cx1 / cz1;  t.sy[1] = FOCAL * cy1 / cz1;
    t.sx[2] = FOCAL * cx2 / cz2;  t.sy[2] = FOCAL * cy2 / cz2;
    // Precompute screen-space AABB for O(1) pre-reject in draw_clipped_segment.
    t.minx = fminf(t.sx[0], fminf(t.sx[1], t.sx[2]));
    t.maxx = fmaxf(t.sx[0], fmaxf(t.sx[1], t.sx[2]));
    t.miny = fminf(t.sy[0], fminf(t.sy[1], t.sy[2]));
    t.maxy = fmaxf(t.sy[0], fmaxf(t.sy[1], t.sy[2]));
    float v1x = cx1-cx0, v1y = cy1-cy0, v1z = cz1-cz0;
    float v2x = cx2-cx0, v2y = cy2-cy0, v2z = cz2-cz0;
    t.Nx = v1y*v2z - v1z*v2y;
    t.Ny = v1z*v2x - v1x*v2z;
    t.Nz = v1x*v2y - v1y*v2x;
    t.D  = t.Nx*cx0 + t.Ny*cy0 + t.Nz*cz0;
  };

  // ── Phase 1: project all FBPolys into screen-space triangles ──────────────
  for (int pi = 0; pi < _fb_poly_count; pi++)
  {
    const FBPoly& poly = _fb_polys[pi];
    float vcx[3], vcy[3], vcz[3];
    to_cam((float)poly.p1x, (float)poly.p1y, (float)poly.p1z, vcx[0], vcy[0], vcz[0]);
    to_cam((float)poly.p2x, (float)poly.p2y, (float)poly.p2z, vcx[1], vcy[1], vcz[1]);
    to_cam((float)poly.p3x, (float)poly.p3y, (float)poly.p3z, vcx[2], vcy[2], vcz[2]);

    bool front[3] = { vcz[0] >= NEAR, vcz[1] >= NEAR, vcz[2] >= NEAR };
    int  n_front  = (int)front[0] + (int)front[1] + (int)front[2];

    if (n_front == 0) continue;

    if (n_front == 3)
    {
      add_proj_tri(vcx[0], vcy[0], vcz[0],
                   vcx[1], vcy[1], vcz[1],
                   vcx[2], vcy[2], vcz[2]);
    }
    else if (n_front == 2)
    {
      int b  = !front[0] ? 0 : (!front[1] ? 1 : 2);
      int f0 = (b + 1) % 3;
      int f1 = (b + 2) % 3;
      float ax, ay, az, bx, by, bz2;
      clip_near_edge(vcx[f0], vcy[f0], vcz[f0], vcx[b],  vcy[b],  vcz[b],  ax, ay, az);
      clip_near_edge(vcx[f1], vcy[f1], vcz[f1], vcx[b],  vcy[b],  vcz[b],  bx, by, bz2);
      add_proj_tri(vcx[f0], vcy[f0], vcz[f0], vcx[f1], vcy[f1], vcz[f1], bx, by, bz2);
      add_proj_tri(vcx[f0], vcy[f0], vcz[f0], bx, by, bz2, ax, ay, az);
    }
    else  // n_front == 1
    {
      int f  = front[0] ? 0 : (front[1] ? 1 : 2);
      int b0 = (f + 1) % 3;
      int b1 = (f + 2) % 3;
      float ax, ay, az, bx, by, bz2;
      clip_near_edge(vcx[f], vcy[f], vcz[f], vcx[b0], vcy[b0], vcz[b0], ax, ay, az);
      clip_near_edge(vcx[f], vcy[f], vcz[f], vcx[b1], vcy[b1], vcz[b1], bx, by, bz2);
      add_proj_tri(vcx[f], vcy[f], vcz[f], ax, ay, az, bx, by, bz2);
    }
  }

  // ── t-value array ──────────────────────────────────────────────────────────
  static float t_vals[FB_MAX_POLYGONS * 6 + 2];
  static int   n_t_vals;

  // ── draw_clipped_segment ───────────────────────────────────────────────────
  // Transforms a world-space segment, near-clips it, screen-clips it with
  // Liang–Barsky, splits it at occluding triangle edges, and draws only the
  // visible sub-intervals.
  auto draw_clipped_segment = [&](float wx0, float wy0, float wz0,
                                   float wx1, float wy1, float wz1)
  {
    // ── Camera-space transform ─────────────────────────────────────────────
    float s_cx, s_cy, s_cz;
    float e_cx, e_cy, e_cz;
    to_cam(wx0, wy0, wz0, s_cx, s_cy, s_cz);
    to_cam(wx1, wy1, wz1, e_cx, e_cy, e_cz);

    // ── Near-plane clip ────────────────────────────────────────────────────
    if (s_cz < NEAR && e_cz < NEAR) return;

    if (s_cz < NEAR)
    {
      float t = (NEAR - s_cz) / (e_cz - s_cz);
      s_cx = s_cx + t * (e_cx - s_cx);
      s_cy = s_cy + t * (e_cy - s_cy);
      s_cz = NEAR;
    }
    else if (e_cz < NEAR)
    {
      float t = (NEAR - e_cz) / (s_cz - e_cz);
      e_cx = e_cx + t * (s_cx - e_cx);
      e_cy = e_cy + t * (s_cy - e_cy);
      e_cz = NEAR;
    }

    // ── Perspective project (centred at 0) ────────────────────────────────
    float p_sx = FOCAL * s_cx / s_cz,  p_sy = FOCAL * s_cy / s_cz;
    float p_ex = FOCAL * e_cx / e_cz,  p_ey = FOCAL * e_cy / e_cz;

    // ── Liang–Barsky screen clip ──────────────────────────────────────────
    // Clip to [-128, 127] in centred coordinates = [0, 255] in DAC space.
    // We need the parametric [t_screen_start, t_screen_end] of the clipped
    // portion on the original [0,1] segment so the occlusion t-values remain
    // consistent.  Derive them from whichever screen axis has the larger span.
    float csx = p_sx, csy = p_sy, cex = p_ex, cey = p_ey;
    if (!lb_clip_screen(csx, csy, cex, cey, -128.0f, 127.0f, -128.0f, 127.0f))
      return;

    float full_dx = p_ex - p_sx,  full_dy = p_ey - p_sy;
    float t_screen_start, t_screen_end;
    if (fabsf(full_dx) >= fabsf(full_dy))
    {
      float inv = (fabsf(full_dx) > 1e-6f) ? 1.0f / full_dx : 0.0f;
      t_screen_start = (csx - p_sx) * inv;
      t_screen_end   = (cex - p_sx) * inv;
    }
    else
    {
      float inv = (fabsf(full_dy) > 1e-6f) ? 1.0f / full_dy : 0.0f;
      t_screen_start = (csy - p_sy) * inv;
      t_screen_end   = (cey - p_sy) * inv;
    }
    if (t_screen_start > t_screen_end)
    {
      float tmp = t_screen_start; t_screen_start = t_screen_end; t_screen_end = tmp;
    }

    float ldx = p_ex - p_sx,  ldy = p_ey - p_sy;

    // ── Screen-space AABB of the clipped line segment (centred coords) ────────
    // Used as a cheap pre-reject against each triangle's AABB before running
    // any edge-intersection or point-in-triangle math.
    float lminx = fminf(csx, cex),  lmaxx = fmaxf(csx, cex);
    float lminy = fminf(csy, cey),  lmaxy = fmaxf(csy, cey);

    // ── Collect t-values where this (screen-clipped) line crosses tri edges ──
    n_t_vals = 0;
    t_vals[n_t_vals++] = t_screen_start;
    t_vals[n_t_vals++] = t_screen_end;

    for (int ti = 0; ti < n_tris; ti++)
    {
      const ProjTri& tri = proj_tris[ti];

      // AABB overlap: if the triangle's screen box doesn't touch the line's
      // screen box, none of its edges can intersect the line — skip all 3.
      if (tri.maxx < lminx || tri.minx > lmaxx ||
          tri.maxy < lminy || tri.miny > lmaxy) continue;

      for (int ei = 0; ei < 3; ei++)
      {
        int   en  = (ei + 1) % 3;
        float edx = tri.sx[en] - tri.sx[ei];
        float edy = tri.sy[en] - tri.sy[ei];

        float denom = ldx * edy - ldy * edx;
        if (fabsf(denom) < 1e-6f) continue;

        float dx = tri.sx[ei] - p_sx;
        float dy = tri.sy[ei] - p_sy;

        float t = (dx * edy - dy * edx) / denom;
        float s = (dx * ldy - dy * ldx) / denom;

        if (t >= t_screen_start && t <= t_screen_end && s >= 0.0f && s <= 1.0f)
        {
          if (n_t_vals < T_CAP)
            t_vals[n_t_vals++] = t;
        }
      }
    }

    // ── Sort t_vals (insertion sort) ───────────────────────────────────────
    for (int i = 1; i < n_t_vals; i++)
    {
      float key = t_vals[i];
      int   j   = i - 1;
      while (j >= 0 && t_vals[j] > key) { t_vals[j + 1] = t_vals[j]; j--; }
      t_vals[j + 1] = key;
    }

    // ── Draw visible sub-intervals ─────────────────────────────────────────
    for (int i = 0; i < n_t_vals - 1; i++)
    {
      float ta = t_vals[i];
      float tb = t_vals[i + 1];
      if (tb - ta < 1e-5f) continue;

      float t_mid  = (ta + tb) * 0.5f;
      float mid_sx = p_sx + t_mid * ldx;
      float mid_sy = p_sy + t_mid * ldy;

      // Perspective-correct camera-space z of the LINE at t_mid.
      float z_line = 1.0f / ((1.0f - t_mid) / s_cz + t_mid / e_cz);

      bool occluded = false;

      for (int ti = 0; ti < n_tris && !occluded; ti++)
      {
        const ProjTri& tri = proj_tris[ti];

        // Point-vs-AABB: if the midpoint is outside the triangle's bounding
        // box it can't be inside the triangle — skip the winding-number test.
        if (mid_sx < tri.minx || mid_sx > tri.maxx ||
            mid_sy < tri.miny || mid_sy > tri.maxy) continue;

        // 2-D point-in-triangle (winding-order independent).
        float d0 = (tri.sx[1]-tri.sx[0])*(mid_sy-tri.sy[0])
                 - (tri.sy[1]-tri.sy[0])*(mid_sx-tri.sx[0]);
        float d1 = (tri.sx[2]-tri.sx[1])*(mid_sy-tri.sy[1])
                 - (tri.sy[2]-tri.sy[1])*(mid_sx-tri.sx[1]);
        float d2 = (tri.sx[0]-tri.sx[2])*(mid_sy-tri.sy[2])
                 - (tri.sy[0]-tri.sy[2])*(mid_sx-tri.sx[2]);

        bool has_neg = (d0 < 0.0f) || (d1 < 0.0f) || (d2 < 0.0f);
        bool has_pos = (d0 > 0.0f) || (d1 > 0.0f) || (d2 > 0.0f);
        if (has_neg && has_pos) continue;

        // Camera-space depth of the POLYGON at this screen point.
        float poly_denom = tri.Nx * (mid_sx / FOCAL)
                         + tri.Ny * (mid_sy / FOCAL)
                         + tri.Nz;
        if (fabsf(poly_denom) < 1e-6f) continue;

        float z_poly = tri.D / poly_denom;

        if (z_poly > NEAR && z_poly < z_line - DEPTH_EPS)
          occluded = true;
      }

      if (!occluded)
      {
        // Endpoints are within the Liang–Barsky clipped region [-128, 127].
        line((int)(128.0f + p_sx + ta * ldx),
             (int)(128.0f + p_sy + ta * ldy),
             (int)(128.0f + p_sx + tb * ldx),
             (int)(128.0f + p_sy + tb * ldy));
      }
    }
  }; // end draw_clipped_segment

  // ── Phase 2: draw staged 3-D line segments ────────────────────────────────
  for (int li = 0; li < _fb_line_count; li++)
  {
    const FBLine& ln = _fb_lines[li];
    draw_clipped_segment((float)ln.sx, (float)ln.sy, (float)ln.sz,
                          (float)ln.ex, (float)ln.ey, (float)ln.ez);
  }

  // ── Phase 3: draw polygon outline edges ───────────────────────────────────
  // Each polygon contributes its three edges as visible lines.
  // DEPTH_EPS ensures a polygon does not occlude its own edges.
  for (int pi = 0; pi < _fb_poly_count; pi++)
  {
    const FBPoly& poly = _fb_polys[pi];
    draw_clipped_segment((float)poly.p1x, (float)poly.p1y, (float)poly.p1z,
                          (float)poly.p2x, (float)poly.p2y, (float)poly.p2z);
    draw_clipped_segment((float)poly.p2x, (float)poly.p2y, (float)poly.p2z,
                          (float)poly.p3x, (float)poly.p3y, (float)poly.p3z);
    draw_clipped_segment((float)poly.p3x, (float)poly.p3y, (float)poly.p3z,
                          (float)poly.p1x, (float)poly.p1y, (float)poly.p1z);
  }
}

bool XYdraw::check_collision_with_camera(unsigned int radius)
{
  float fx = (float)_cam_x;
  float fy = (float)_cam_y;
  float fz = (float)_cam_z;
  float r  = (float)radius;

  for (int i = 0; i < _fb_poly_count; i++)
  {
    const FBPoly& poly = _fb_polys[i];

    float p1x = (float)poly.p1x,  p1y = (float)poly.p1y,  p1z = (float)poly.p1z;
    float p2x = (float)poly.p2x,  p2y = (float)poly.p2y,  p2z = (float)poly.p2z;
    float p3x = (float)poly.p3x,  p3y = (float)poly.p3y,  p3z = (float)poly.p3z;

    // ── Plane normal ───────────────────────────────────────────────────────
    float e1x = p2x-p1x,  e1y = p2y-p1y,  e1z = p2z-p1z;
    float e2x = p3x-p1x,  e2y = p3y-p1y,  e2z = p3z-p1z;
    float nx  = e1y*e2z - e1z*e2y;
    float ny  = e1z*e2x - e1x*e2z;
    float nz  = e1x*e2y - e1y*e2x;
    float nlen = sqrtf(nx*nx + ny*ny + nz*nz);
    if (nlen < 1e-6f) continue;   // degenerate triangle
    float inv = 1.0f / nlen;
    nx *= inv;  ny *= inv;  nz *= inv;

    // ── Signed distance from camera to plane ───────────────────────────────
    float dist = nx*(fx-p1x) + ny*(fy-p1y) + nz*(fz-p1z);
    if (fabsf(dist) > r) continue;   // not close enough to this plane

    // ── Project camera onto the plane ──────────────────────────────────────
    float foot_x = fx - dist*nx;
    float foot_y = fy - dist*ny;
    float foot_z = fz - dist*nz;

    // ── Barycentric inside-triangle test ───────────────────────────────────
    // v0 = p3-p1,  v1 = p2-p1,  v2 = foot-p1
    float v0x = p3x-p1x,  v0y = p3y-p1y,  v0z = p3z-p1z;
    float v1x = p2x-p1x,  v1y = p2y-p1y,  v1z = p2z-p1z;
    float v2x = foot_x-p1x, v2y = foot_y-p1y, v2z = foot_z-p1z;

    float d00 = v0x*v0x + v0y*v0y + v0z*v0z;
    float d01 = v0x*v1x + v0y*v1y + v0z*v1z;
    float d02 = v0x*v2x + v0y*v2y + v0z*v2z;
    float d11 = v1x*v1x + v1y*v1y + v1z*v1z;
    float d12 = v1x*v2x + v1y*v2y + v1z*v2z;

    float denom = d00*d11 - d01*d01;
    if (fabsf(denom) < 1e-6f) continue;   // degenerate

    float inv_denom = 1.0f / denom;
    float u = (d11*d02 - d01*d12) * inv_denom;
    float v = (d00*d12 - d01*d02) * inv_denom;

    if (u >= 0.0f && v >= 0.0f && u + v <= 1.0f)
      return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String rendering functions

// Letter layout:
// 
// start point (x, y)
// |
// |
// v
// *_ _
// |_|_|
// |_|_|
// |_|_|

// Renders a single character
void XYdraw::render_letter(char letter, int x, int y, int offset, int size)
{
  switch (letter)
  {
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Special character rendering functions
    case '!':
      render_exclamationmark(x + offset, y, size);
      break;
    case '"':
      render_doublequotes(x + offset, y, size);
      break;
    case '#':
      render_numbersign(x + offset, y, size);
      break;
    case '$':
      render_dollarsign(x + offset, y, size);
      break;
    case '%':
      render_percentsign(x + offset, y, size);
      break;
    case '&':
      render_ampersand(x + offset, y, size);
      break;
    case '\'':
      render_singlequote(x + offset, y, size);
      break;
    case '(':
      render_openparenthesis(x + offset, y, size);
      break;
    case ')':
      render_closeparenthesis(x + offset, y, size);
      break;
    case '*':
      render_asterisk(x + offset, y, size);
      break;
    case '+':
      render_plus(x + offset, y, size);
      break;
    case ',':
      render_comma(x + offset, y, size);
      break;
    case '-':
      render_hyphen(x + offset, y, size);
      break;
    case '.':
      render_fullstop(x + offset, y, size);
      break;
    case '/':
      render_slash(x + offset, y, size);
      break;
    case ':':
      render_colon(x + offset, y, size);
      break;
    case ';':
      render_semicolon(x + offset, y, size);
      break;
    case '<':
      render_lessthansign(x + offset, y, size);
      break;
    case '=':
      render_equalsign(x + offset, y, size);
      break;
    case '>':
      render_greaterthansign(x + offset, y, size);
      break;
    case '?':
      render_questionmark(x + offset, y, size);
      break;
    case '@':
      render_atsign(x + offset, y, size);
      break;
    case '[':
      render_openingbracket(x + offset, y, size);
      break;
    case '\\':
      render_backslash(x + offset, y, size);
      break;
    case ']':
      render_closingbracket(x + offset, y, size);
      break;
    case '^':
      render_caret(x + offset, y, size);
      break;
    case '_':
      render_underscore(x + offset, y, size);
      break;
    case '`':
      render_graveaccent(x + offset, y, size);
      break;
    case '{':
      render_openingbrace(x + offset, y, size);
      break;
    case '|':
      render_verticalbar(x + offset, y, size);
      break;
    case '}':
      render_closingbrace(x + offset, y, size);
      break;
    case '~':
      render_tilde(x + offset, y, size);
      break;
    case ' ':
      break;
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Number rendering functions
    case '0':
      render_zero(x + offset, y, size);
      break;
    case '1':
      render_one(x + offset, y, size);
      break;
    case '2':
      render_two(x + offset, y, size);
      break;
    case '3':
      render_three(x + offset, y, size);
      break;
    case '4':
      render_four(x + offset, y, size);
      break;
    case '5':
      render_five(x + offset, y, size);
      break;
    case '6':
      render_six(x + offset, y, size);
      break;
    case '7':
      render_seven(x + offset, y, size);
      break;
    case '8':
      render_eight(x + offset, y, size);
      break;
    case '9':
      render_nine(x + offset, y, size);
      break;
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Non - capitalized letter rendering functions
    case 'a':
      render_a(x + offset, y, size);
      break;
    case 'b':
      render_b(x + offset, y, size);
      break;
    case 'c':
      render_c(x + offset, y, size);
      break;
    case 'd':
      render_d(x + offset, y, size);
      break;
    case 'e':
      render_e(x + offset, y, size);
      break;
    case 'f':
      render_f(x + offset, y, size);
      break;
    case 'g':
      render_g(x + offset, y, size);
      break;
    case 'h':
      render_h(x + offset, y, size);
      break;
    case 'i':
      render_i(x + offset, y, size);
      break;
    case 'j':
      render_j(x + offset, y, size);
      break;
    case 'k':
      render_k(x + offset, y, size);
      break;
    case 'l':
      render_l(x + offset, y, size);
      break;
    case 'm':
      render_m(x + offset, y, size);
      break;
    case 'n':
      render_n(x + offset, y, size);
      break;
    case 'o':
      render_o(x + offset, y, size);
      break;
    case 'p':
      render_p(x + offset, y, size);
      break;
    case 'q':
      render_q(x + offset, y, size);
      break;
    case 'r':
      render_r(x + offset, y, size);
      break;
    case 's':
      render_s(x + offset, y, size);
      break;
    case 't':
      render_t(x + offset, y, size);
      break;
    case 'u':
      render_u(x + offset, y, size);
      break;
    case 'v':
      render_v(x + offset, y, size);
      break;
    case 'w':
      render_w(x + offset, y, size);
      break;
    case 'x':
      render_x(x + offset, y, size);
      break;
    case 'y':
      render_y(x + offset, y, size);
      break;
    case 'z':
      render_z(x + offset, y, size);
      break;
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Number / math symbol rendering functions
    case 'A':
      render_A(x + offset, y, size);
      break;
    case 'B':
      render_B(x + offset, y, size);
      break;
    case 'C':
      render_C(x + offset, y, size);
      break;
    case 'D':
      render_D(x + offset, y, size);
      break;
    case 'E':
      render_E(x + offset, y, size);
      break;
    case 'F':
      render_F(x + offset, y, size);
      break;
    case 'G':
      render_G(x + offset, y, size);
      break;
    case 'H':
      render_H(x + offset, y, size);
      break;
    case 'I':
      render_I(x + offset, y, size);
      break;
    case 'J':
      render_J(x + offset, y, size);
      break;
    case 'K':
      render_K(x + offset, y, size);
      break;
    case 'L':
      render_L(x + offset, y, size);
      break;
    case 'M':
      render_M(x + offset, y, size);
      break;
    case 'N':
      render_N(x + offset, y, size);
      break;
    case 'O':
      render_O(x + offset, y, size);
      break;
    case 'P':
      render_P(x + offset, y, size);
      break;
    case 'Q':
      render_Q(x + offset, y, size);
      break;
    case 'R':
      render_R(x + offset, y, size);
      break;
    case 'S':
      render_S(x + offset, y, size);
      break;
    case 'T':
      render_T(x + offset, y, size);
      break;
    case 'U':
      render_U(x + offset, y, size);
      break;
    case 'V':
      render_V(x + offset, y, size);
      break;
    case 'W':
      render_W(x + offset, y, size);
      break;
    case 'X':
      render_X(x + offset, y, size);
      break;
    case 'Y':
      render_Y(x + offset, y, size);
      break;
    case 'Z':
      render_Z(x + offset, y, size);
      break;
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Unknown character
    default:
      render_unknown_character(x + offset, y, size);
      break;
  }
}

// Renders a left justified string
void XYdraw::render_left_justified_string(String input_string, int x, int y, int size, int letter_distance)
{
  for (int i = 0; i < input_string.length(); i++)
  {
    char letter = input_string.charAt(i);
    int offset = i * (size + letter_distance);
    render_letter(letter, x, y, offset, size);
  }
}

// Renders a right justified string
void XYdraw::render_right_justified_string(String input_string, int x, int y, int size, int letter_distance)
{
  x -= (input_string.length() * (size + letter_distance));
  for (int i = 0; i < input_string.length(); i++)
  {
    char letter = input_string.charAt(i);
    int offset = i * (size + letter_distance);
    render_letter(letter, x, y, offset, size);
  }
}

// Renders a centered string
void XYdraw::render_centered_string(String input_string, int x, int y, int size, int letter_distance)
{
  x -= ((input_string.length() * (size + letter_distance))) * 0.5;
  for (int i = 0; i < input_string.length(); i++)
  {
    char letter = input_string.charAt(i);
    int offset = i * (size + letter_distance);
    render_letter(letter, x, y, offset, size);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Special character rendering functions

// draws an exclamation mark
void XYdraw::render_exclamationmark(int x, int y, int size)
{
  line(x + size, y, x + size, y - size * 1.8);
  line(x + size, y - size * 2.7, x + size, y - size * 3);
}

// draws a double quotes
void XYdraw::render_doublequotes(int x, int y, int size)
{
  line(x + size * 0.5, y, x + size * 0.5, y - size);
  line(x + size * 1.5, y, x + size * 1.5, y - size);
}

// draws a number sign
void XYdraw::render_numbersign(int x, int y, int size)
{
  line(x + size * 0.5, y, x + size * 0.5, y - size * 3);
  line(x + size * 1.5, y, x + size * 1.5, y - size * 3);
  line(x, y - size, x + size * 2, y - size);
  line(x, y - size * 2, x + size * 2, y - size * 2);
}

// draws a dollar sign
void XYdraw::render_dollarsign(int x, int y, int size)
{
  render_S(x, y, size);
  line(x + size, y, x + size, y - size * 3);
}

// draws a percent sign
void XYdraw::render_percentsign(int x, int y, int size)
{
  render_slash(x, y, size);
  circle(x + size * 0.5, y - size * 0.5, size * 0.5);
  circle(x + size * 1.5, y - size * 2.5, size * 0.5);
}

// draws an ampersand
void XYdraw::render_ampersand(int x, int y, int size)
{
  line (x + size * 2, y - size * 3, x + size, y - size);
  bezier_curve(x + size, y - size, x + size, y - size, x, y, x + size * 2, y);
  bezier_curve(x + size, y - size, x + size, y - size * 2, x, y - size * 3, x, y - size * 3);
  line (x + size, y - size * 2, x + size * 2, y - size);
}

// draws a single quote
void XYdraw::render_singlequote(int x, int y, int size)
{
  line(x + size, y, x + size, y - size);
}

// draws an open parenthesis
void XYdraw::render_openparenthesis(int x, int y, int size)
{
  bezier_curve(x + size, y, x + size, y - size * 3, x, y - size * 1.5, x, y - size * 1.5);
}

// draws a closed parenthesis
void XYdraw::render_closeparenthesis(int x, int y, int size)
{
  bezier_curve(x + size, y, x + size, y - size * 3, x + size * 2, y - size * 1.5, x + size * 2, y - size * 1.5);
}

// draws an asterisk
void XYdraw::render_asterisk(int x, int y, int size)
{
  line(x + size * 0.4, y - size * 1, x + size * 1.6, y - size * 1);
  line(x + size, y - size, x + size * 0.6, y - size * 1.6);
  line(x + size, y - size, x + size * 1.4, y - size * 1.6);
  line(x + size, y - size * 0.5, x + size, y - size);
}

// draws a plus sign
void XYdraw::render_plus(int x, int y, int size)
{
  line(x, y - size * 1.5, x + size * 2, y - size * 1.5);
  line(x + size, y - size * 0.5, x + size, y - size * 2.5);
}

// draws a comma
void XYdraw::render_comma(int x, int y, int size)
{
  line(x + size, y - size * 2.3, x + size, y - size * 3);
}

// draws a hyphen
void XYdraw::render_hyphen(int x, int y, int size)
{
  line(x, y - size * 1.5, x + size * 2, y - size * 1.5);
}

// draws a full stop
void XYdraw::render_fullstop(int x, int y, int size)
{
  line(x + size, y - size * 2.5, x + size, y - size * 3);
}

// draws a slash
void XYdraw::render_slash(int x, int y, int size)
{
  line(x, y - size * 3, x + size * 2, y);
}

// draws a colon
void XYdraw::render_colon(int x, int y, int size)
{
  line(x + size, y - size * 1, x + size, y - size * 1.5);
  line(x + size, y - size * 2.5, x + size, y - size * 3);
}

// draws a semicolon
void XYdraw::render_semicolon(int x, int y, int size)
{
  line(x + size, y - size * 1, x + size, y - size * 1.5);
  line(x + size, y - size * 2.3, x + size, y - size * 3);
}

// draws a less than sign
void XYdraw::render_lessthansign(int x, int y, int size)
{
  line(x + size * 1.5, y - size * 1, x + size * 0.5, y - size * 1.5);
  line(x + size * 0.5, y - size * 1.5, x + size * 1.5, y - size * 2);
}

// draws an equal sign
void XYdraw::render_equalsign(int x, int y, int size)
{
  line(x, y - size * 1, x + size * 2, y - size * 1);
  line(x, y - size * 2, x + size * 2, y - size * 2);
}

// draws a greater than sign
void XYdraw::render_greaterthansign(int x, int y, int size)
{
  line(x + size * 0.5, y - size * 1, x + size * 1.5, y - size * 1.5);
  line(x + size * 1.5, y - size * 1.5, x + size * 0.5, y - size * 2);
}

// draws a question mark
void XYdraw::render_questionmark(int x, int y, int size)
{
  bezier_curve(x, y - size, x + size * 2, y - size, x, y, x + size * 2, y);
  bezier_curve(x + size * 2, y - size, x + size, y - size * 2, x + size * 2, y - size * 2, x + size, y - size);
  line(x + size, y - size * 2.5, x + size, y - size * 3);
}

// draws an at sign
void XYdraw::render_atsign(int x, int y, int size)
{
  circle(x + size, y - size * 2, size * 0.5);
  circle(x + size, y - size * 2, size);
  line(x + size * 1.5, y - size * 2.5, x + size * 2, y - size * 3);
}

// draws an opening bracket
void XYdraw::render_openingbracket(int x, int y, int size)
{
  line(x + size, y, x, y);
  line(x, y, x, y - size * 3);
  line(x, y - size * 3, x + size, y - size * 3);
}

// draws a backslash
void XYdraw::render_backslash(int x, int y, int size)
{
  line(x, y, x + size * 2, y - size * 3);
}

// draws a closing bracket
void XYdraw::render_closingbracket(int x, int y, int size)
{
  line(x + size, y, x + size * 2, y);
  line(x + size * 2, y, x + size * 2, y - size * 3);
  line(x + size * 2, y - size * 3, x + size, y - size * 3);
}

// draws a caret
void XYdraw::render_caret(int x, int y, int size)
{
  line(x + size * 0.5, y - size, x + size, y);
  line(x + size, y, x + size * 1.5, y - size);
}

// draws an underscore
void XYdraw::render_underscore(int x, int y, int size)
{
  line(x, y - size * 3, x + size * 2, y - size * 3);
}

// draws a grave accent
void XYdraw::render_graveaccent(int x, int y, int size)
{
  line(x + size * 0.7, y, x + size * 1.3, y - size);
}

// draws an opening brace
void XYdraw::render_openingbrace(int x, int y, int size)
{
  bezier_curve(x + size, y, x, y - size * 1.5, x, y, x + size, y - size * 1.5);
  bezier_curve(x, y - size * 1.5, x + size, y - size * 3, x + size, y - size * 1.5, x, y - size * 3);
}

// draws a vertical bar
void XYdraw::render_verticalbar(int x, int y, int size)
{
  line(x + size, y, x + size, y - size * 3);
}

// draws a closing brace
void XYdraw::render_closingbrace(int x, int y, int size)
{
  bezier_curve(x + size, y, x + size * 2, y - size * 1.5, x + size * 2, y, x + size, y - size * 1.5);
  bezier_curve(x + size * 2, y - size * 1.5, x + size, y - size * 3, x + size, y - size * 1.5, x + size * 2, y - size * 3);
}

// draws a tilde
void XYdraw::render_tilde(int x, int y, int size)
{
  bezier_curve(x, y - size * 1.5, x + size, y - size * 1.5, x + size * 0.5, y - size * 0.5, x + size * 0.5, y - size * 0.5);
  bezier_curve(x + size, y - size * 1.5, x + size * 2, y - size * 1.5, x + size * 1.5, y - size * 2.5, x + size * 1.5, y - size * 2.5);
}

// draws the unicode replacement character
void XYdraw::render_unknown_character(int x, int y, int size)
{
  line(x + size, y - size, x + size * 2, y - size * 2);
  line(x + size * 2, y - size * 2, x + size, y - size * 3);
  line(x + size, y - size * 3, x, y - size * 2);
  line(x, y - size * 2, x + size, y - size);
  render_questionmark(x + size * 0.5, y - size * 1.5, size * 0.5);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Number rendering functions

// draws the number 0
void XYdraw::render_zero(int x, int y, int size)
{
  bezier_curve(x, y - size, x + size * 2 , y - size, x, y + size * 0.2, x + size * 2, y + size * 0.5);
  line(x + size * 2, y - size, x + size * 2, y - size * 2);
  bezier_curve(x + size * 2, y - size * 2, x, y - size * 2, x + size * 2, y - size * 3, x, y - size * 3);
  line(x, y - size * 2, x, y - size);
}

// draws the number 1
void XYdraw::render_one(int x, int y, int size)
{
  line(x, y - size, x + size, y);
  line(x + size, y, x + size, y - size * 3);
}

// draws the number 2
void XYdraw::render_two(int x, int y, int size)
{
  bezier_curve(x, y - size, x + size * 2 , y - size, x, y + size * 0.2, x + size * 2, y + size * 0.5);
  line(x + size * 2, y - size, x, y - size * 3);
  line(x, y - size * 3, x + size * 2, y - size * 3);
}

// draws the number 3
void XYdraw::render_three(int x, int y, int size)
{
  bezier_curve(x, y - size * 0.5, x + size * 2 , y - size, x, y + size * 0.2, x + size * 2, y + size * 0.5);
  line(x + size * 2, y - size, x + size * 1.2, y - size * 1.5);
  line(x + size * 1.2, y - size * 1.5, x + size * 2, y - size * 2);
  bezier_curve(x, y - size * 2.5, x + size * 2 , y - size * 2, x, y - size * 3.5, x + size * 2, y - size * 3.5);
}

// draws the number 4
void XYdraw::render_four(int x, int y, int size)
{
  line(x + size, y, x, y - size * 2);
  line(x, y - size * 2, x + size * 2, y - size * 2);
  line(x + size * 1.5, y - size, x + size * 1.5, y - size * 3);
}

// draws the number 5
void XYdraw::render_five(int x, int y, int size)
{
  line(x + size * 2, y, x, y);
  line(x, y, x, y - size);
  bezier_curve(x, y - size, x, y - size * 3, x + size * 2.5, y - size, x + size * 2.5, y - size * 3);
}

// draws the number 6
void XYdraw::render_six(int x, int y, int size)
{
  bezier_curve(x, y - size, x + size * 2 , y - size * 0.5, x, y + size * 0.2, x + size * 2, y + size * 0.5);
  line(x, y - size, x, y - size * 2.25);
  ellipse(x + size, y - size * 2.25, size, size * 0.75);
}

// draws the number 7
void XYdraw::render_seven(int x, int y, int size)
{
  line(x, y, x + size * 2, y);
  line(x + size * 2, y, x, y - size * 2);
  line(x, y - size * 1, x + size * 2, y - size * 1);
}

// draws the number 8
void XYdraw::render_eight(int x, int y, int size)
{
  ellipse(x + size, y - size * 0.75, size, size * 0.75);
  ellipse(x + size, y - size * 2.25, size, size * 0.75);
}

// draws the number 9
void XYdraw::render_nine(int x, int y, int size)
{
  ellipse(x + size, y - size * 0.75, size, size * 0.75);
  line(x + size * 2, y - size * 0.75, x + size * 2, y - size * 2);
  bezier_curve(x, y - size * 2.5, x + size * 2 , y - size * 2, x, y - size * 3.5, x + size * 2, y - size * 3.5);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Non - capitalized letters rendering functions

// draws the letter a
void XYdraw::render_a(int x, int y, int size)
{
  ellipse(x + size, y - size * 2.5, size, size * 0.5);
  line(x + size * 2, y - size * 3, x + size * 2, y - size * 1.5);
  bezier_curve(x + size * 2, y - size * 1.5, x, y - size * 1.5, x + size * 2, y - size * 0.5, x, y - size * 0.5);
}

// draws the letter b
void XYdraw::render_b(int x, int y, int size)
{
  circle(x + size, y - size * 2, size);
  line(x, y - size * 3, x, y);
}

// draws the letter c
void XYdraw::render_c(int x, int y, int size)
{
  bezier_curve(x + size * 2, y - size * 1, x, y - size * 1.5, x + size * 2, y - size * 0.5, x, y - size * 0.5);
  line(x, y - size * 1.5, x, y - size * 2);
  bezier_curve(x, y - size * 2, x + size * 2, y - size * 2.5, x, y - size * 3, x + size * 2, y - size * 3);
}

// draws the letter d
void XYdraw::render_d(int x, int y, int size)
{
  circle(x + size, y - size * 2, size);
  line(x + size * 2, y - size * 3, x + size * 2, y);
}

// draws the letter e
void XYdraw::render_e(int x, int y, int size)
{
  line(x, y - size * 1.5, x + size * 2, y - size * 1.5);
  bezier_curve(x + size * 2, y - size * 1.5, x, y - size * 1.5, x + size * 2, y - size * 0.5, x, y - size * 0.5);
  line(x, y - size * 1.5, x, y - size * 2);
  bezier_curve(x, y - size * 2, x + size * 2, y - size * 2.5, x, y - size * 3, x + size * 2, y - size * 3);
}

// draws the letter f
void XYdraw::render_f(int x, int y, int size)
{
  bezier_curve(x + size * 2, y - size * 0.5, x + size, y - size * 0.5, x + size * 2, y, x + size, y);
  line(x + size, y - size * 0.5, x + size, y - size * 3);
  line(x, y - size * 1.5, x + size * 2, y - size * 1.5);
}

// draws the letter g
void XYdraw::render_g(int x, int y, int size)
{
  ellipse(x + size, y - size * 1.5, size, size * 0.5);
  line(x + size * 2, y - size, x + size * 2, y - size * 3);
  bezier_curve(x + size * 2, y - size * 3, x, y - size * 3, x + size * 2, y - size * 4, x, y - size * 4);
}

// draws the letter h
void XYdraw::render_h(int x, int y, int size)
{
  line(x, y, x, y - size * 3);
  bezier_curve(x, y - size * 2, x + size * 2, y - size * 2,  x, y - size, x + size * 2, y - size);
  line(x + size * 2, y - size * 2, x + size * 2, y - size * 3);
}

// draws the letter i
void XYdraw::render_i(int x, int y, int size)
{
  line(x + size, y, x + size, y - size * 0.3);
  line(x + size, y - size * 1.1, x + size, y - size * 3);
}

// draws the letter j
void XYdraw::render_j(int x, int y, int size)
{
  line(x + size * 2, y, x + size * 2, y - size * 0.3);
  line(x + size * 2, y - size * 1.1, x + size * 2, y - size * 3);
  bezier_curve(x + size * 2, y - size * 3, x, y - size * 3, x + size * 2, y - size * 4, x, y - size * 4);
}

// draws the letter k
void XYdraw::render_k(int x, int y, int size)
{
  line(x, y, x, y - size * 3);
  line(x, y - size * 2, x + size * 2, y - size);
  line(x + size * 2, y - size * 3, x, y - size * 2);
}

// draws the letter l
void XYdraw::render_l(int x, int y, int size)
{
  line(x, y, x + size, y);
  line(x + size, y, x + size, y - size * 3);
  line(x, y - size * 3, x + size * 2, y - size * 3);
}

// draws the letter m
void XYdraw::render_m(int x, int y, int size)
{
  line(x, y - size, x, y - size * 3);
  bezier_curve(x, y - size * 1.5, x + size, y - size * 1.7, x, y - size, x + size, y - size);
  bezier_curve(x + size, y - size * 1.7, x + size * 2, y - size * 1.5, x + size, y - size, x + size * 2, y - size);
  line(x + size * 2, y - size * 1.5, x + size * 2, y - size * 3);
}

// draws the letter n
void XYdraw::render_n(int x, int y, int size)
{
  line(x, y - size, x, y - size * 3);
  bezier_curve(x, y - size * 2, x + size * 2, y - size * 2, x, y - size, x + size * 2, y - size);
  line(x + size * 2, y - size * 2, x + size * 2, y - size * 3);
}

// draws the letter o
void XYdraw::render_o(int x, int y, int size)
{
  circle(x + size, y - size * 2, size);
}

// draws the letter p
void XYdraw::render_p(int x, int y, int size)
{
  circle(x + size, y - size * 2, size);
  line(x, y - size, x, y - size * 4);
}

// draws the letter q
void XYdraw::render_q(int x, int y, int size)
{
  circle(x + size, y - size * 2, size);
  line(x + size * 2, y - size, x + size * 2, y - size * 4);
}

// draws the letter r
void XYdraw::render_r(int x, int y, int size)
{
  line(x, y - size, x, y - size * 3);
  bezier_curve(x, y - size * 2, x + size * 2, y - size * 2, x, y - size, x + size * 2, y - size);
}

// draws the letter s
void XYdraw::render_s(int x, int y, int size)
{
  bezier_curve(x + size * 2, y - size * 1.5, x, y - size * 1.5, x + size * 2, y - size, x, y - size);
  bezier_curve(x, y - size * 1.5, x + size * 2, y - size * 2.5, x, y - size * 2, x + size * 2, y - size * 2);
  bezier_curve(x + size * 2, y - size * 2.5, x, y - size * 2.5, x + size * 2, y - size * 3, x, y - size * 3);
}

// draws the letter t
void XYdraw::render_t(int x, int y, int size)
{
  line(x + size, y, x + size, y - size * 2.5);
  bezier_curve(x + size, y - size * 2.5, x + size * 2, y - size * 2.5, x + size, y - size * 3, x + size * 2, y - size * 3);
  line(x, y - size * 1.5, x + size * 2, y - size * 1.5);
}

// draws the letter u
void XYdraw::render_u(int x, int y, int size)
{
  line(x, y - size, x, y - size * 2);
  bezier_curve(x, y - size * 2, x + size * 2, y - size * 2, x, y - size * 3, x + size * 2, y - size * 3);
  line(x + size * 2, y - size, x + size * 2, y - size * 3);
}

// draws the letter v
void XYdraw::render_v(int x, int y, int size)
{
  line(x, y - size, x + size, y - size * 3);
  line(x + size, y - size * 3, x + size * 2, y - size);
}

// draws the letter w
void XYdraw::render_w(int x, int y, int size)
{
  line(x, y - size, x + size * 0.5, y - size * 3);
  line(x + size * 0.5, y - size * 3, x + size, y - size * 2);
  line(x + size, y - size * 2, x + size * 1.5, y - size * 3);
  line(x + size * 1.5, y - size * 3, x + size * 2, y - size);
}

// draws the letter x
void XYdraw::render_x(int x, int y, int size)
{
  line(x, y - size, x + size * 2, y - size * 3);
  line(x, y - size * 3, x + size * 2, y - size);
}

// draws the letter y
void XYdraw::render_y(int x, int y, int size)
{
  line(x, y - size, x + size, y - size * 3);
  line(x + size * 2, y - size, x, y - size * 4);
}

// draws the letter z
void XYdraw::render_z(int x, int y, int size)
{
  line(x, y - size, x + size * 2, y - size);
  line(x + size * 2, y - size, x, y - size * 3);
  line(x, y - size * 3, x + size * 2, y - size * 3);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Capitalized letters rendering functions

// draws the letter A
void XYdraw::render_A(int x, int y, int size)
{
  line(x, y - size * 3, x + size, y);
  line(x + size, y, x + size * 2, y - size * 3);
  line(x + size * 0.333, y - size * 2, x + size * 1.666, y - size * 2);
}

// draws the letter B
void XYdraw::render_B(int x, int y, int size)
{
  line(x, y - size * 3, x, y);
  bezier_curve(x, y, x, y - size * 1.5, x + size, y, x + size, y - size * 1.5);
  bezier_curve(x, y - size * 1.5, x, y - size * 3, x + size * 2, y - size * 1.5, x + size * 2, y - size * 3);
}

// draws the letter C
void XYdraw::render_C(int x, int y, int size)
{
  bezier_curve(x + size * 2, y - size, x, y - size, x + size * 2, y, x, y);
  line(x, y - size, x, y - size * 2);
  bezier_curve(x, y - size * 2, x + size * 2, y - size * 2.2, x, y - size * 3, x + size * 2, y - size * 3);
}

// draws the letter D
void XYdraw::render_D(int x, int y, int size)
{
  line(x, y, x, y - size * 3);
  bezier_curve(x, y - size * 3, x, y, x + size * 2, y - size * 3, x + size * 2, y);
}

// draws the letter E
void XYdraw::render_E(int x, int y, int size)
{
  line(x + size * 2, y, x, y);
  line(x, y, x, y - size * 3);
  line(x, y - size * 3, x + size * 2, y - size * 3);
  line(x, y - size * 1.5, x + size * 2, y - size * 1.5);
}

// draws the letter F
void XYdraw::render_F(int x, int y, int size)
{
  line(x + size * 2, y, x, y);
  line(x, y, x, y - size * 3);
  line(x, y - size * 1.5, x + size * 1.8, y - size * 1.5);
}

// draws the letter G
void XYdraw::render_G(int x, int y, int size)
{
  bezier_curve(x + size * 2, y - size * 0.5, x, y - size, x + size * 2, y, x, y);
  line(x, y - size, x, y - size * 2);
  bezier_curve(x, y - size * 2, x + size * 2, y - size * 2, x, y - size * 3, x + size * 2, y - size * 3);
  line(x + size * 2, y - size * 2, x + size * 2, y - size * 1.5);
  line(x + size * 2, y - size * 1.5, x + size, y - size * 1.5);
}

// draws the letter H
void XYdraw::render_H(int x, int y, int size)
{
  line(x, y, x, y - size * 3);
  line(x, y - size * 1.5, x + size * 2, y - size * 1.5);
  line(x + size * 2, y, x + size * 2, y - size * 3);
}

// draws the letter I
void XYdraw::render_I(int x, int y, int size)
{
  line(x + size, y, x + size, y - size *3);
}

// draws the letter J
void XYdraw::render_J(int x, int y, int size)
{
  line(x + size * 2, y, x + size * 2, y - size * 2);
  bezier_curve(x + size * 2, y - size * 2, x, y - size * 2, x + size * 2, y - size * 3, x, y - size * 3);
}

// draws the letter K
void XYdraw::render_K(int x, int y, int size)
{
  line(x, y, x, y - size * 3);
  line(x, y - size * 2, x + size * 2, y);
  line(x + size * 2, y - size * 3, x, y - size * 2);
}

// draws the letter L
void XYdraw::render_L(int x, int y, int size)
{
  line(x, y, x, y - size * 3);
  line(x, y - size * 3, x + size * 2, y - size * 3);
}

// draws the letter M
void XYdraw::render_M(int x, int y, int size)
{
  line(x, y - size * 3, x, y);
  line(x, y, x + size, y - size);
  line(x + size, y - size, x + size * 2, y);
  line(x + size * 2, y, x + size * 2, y - size * 3);
}

// draws the letter N
void XYdraw::render_N(int x, int y, int size)
{
  line(x, y - size * 3, x, y);
  line(x, y, x + size * 2, y - size * 3);
  line(x + size * 2, y - size * 3, x + size * 2, y);
}

// draws the letter O
void XYdraw::render_O(int x, int y, int size)
{
  bezier_curve(x, y - size, x + size * 2 , y - size, x, y + size * 0.2, x + size * 2, y + size * 0.5);
  line(x + size * 2, y - size, x + size * 2, y - size * 2);
  bezier_curve(x + size * 2, y - size * 2, x, y - size * 2, x + size * 2, y - size * 3, x, y - size * 3);
  line(x, y - size * 2, x, y - size);
}

// draws the letter P
void XYdraw::render_P(int x, int y, int size)
{
  line(x, y - size * 3, x, y);
  bezier_curve(x, y, x, y - size * 1.5, x + size * 2, y, x + size * 2, y - size * 1.5);
}

// draws the letter Q
void XYdraw::render_Q(int x, int y, int size)
{
  bezier_curve(x, y - size, x + size * 2 , y - size, x, y + size * 0.2, x + size * 2, y + size * 0.5);
  line(x + size * 2, y - size, x + size * 2, y - size * 2);
  bezier_curve(x + size * 2, y - size * 2, x, y - size * 2, x + size * 2, y - size * 3, x, y - size * 3);
  line(x, y - size * 2, x, y - size);
  line(x + size, y - size * 2, x + size * 2, y - size * 3);
}

// draws the letter R
void XYdraw::render_R(int x, int y, int size)
{
  line(x, y - size * 3, x, y);
  bezier_curve(x, y, x, y - size * 1.5, x + size * 2, y, x + size * 2, y - size * 1.5);
  line(x, y - size * 1.5, x + size * 2, y - size * 3);
}

// draws the letter S
void XYdraw::render_S(int x, int y, int size)
{
  bezier_curve(x + size * 2, y - size, x, y - size, x + size * 2, y, x, y);
  bezier_curve(x, y - size, x + size * 2, y - size * 2, x, y - size * 2, x + size * 2, y - size);
  bezier_curve(x + size * 2, y - size * 2, x, y - size * 2, x + size * 2, y - size * 3, x, y - size * 3);
}

// draws the letter T
void XYdraw::render_T(int x, int y, int size)
{
  line(x, y, x + size * 2, y);
  line(x + size, y, x + size, y - size * 3);
}

// draws the letter U
void XYdraw::render_U(int x, int y, int size)
{
  line(x, y, x, y - size * 2);
  bezier_curve(x, y - size * 2, x + size * 2, y - size * 2, x, y - size * 3, x + size * 2, y - size * 3);
  line(x + size * 2, y - size * 2, x + size * 2, y);
}

// draws the letter V
void XYdraw::render_V(int x, int y, int size)
{
  line(x, y, x + size, y - size * 3);
  line(x + size, y - size * 3, x + size * 2, y);
}

// draws the letter W
void XYdraw::render_W(int x, int y, int size)
{
  line(x, y, x + size * 0.5, y - size * 3);
  line(x + size * 0.5, y - size * 3, x + size, y - size * 2);
  line(x + size, y - size * 2, x + size * 1.5, y - size * 3);
  line(x + size * 1.5, y - size * 3, x + size * 2, y);
}

// draws the letter X
void XYdraw::render_X(int x, int y, int size)
{
  line(x, y, x + size * 2, y - size * 3);
  line(x, y - size * 3, x + size * 2, y);
}

// draws the letter Y
void XYdraw::render_Y(int x, int y, int size)
{
  line(x, y, x + size, y - size);
  line(x + size * 2, y, x + size, y - size);
  line(x + size, y - size, x + size, y - size * 3);
}

// draws the letter Z
void XYdraw::render_Z(int x, int y, int size)
{
  line(x, y, x + size * 2, y);
  line(x + size * 2, y, x, y - size * 3);
  line(x, y - size * 3, x + size * 2, y - size * 3);
}