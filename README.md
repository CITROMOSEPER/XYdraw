# XYdraw

**2D and 3D vector graphics on an oscilloscope in XY mode — powered by the ESP32's built-in DAC pins.**

XYdraw drives an oscilloscope as a vector display by continuously outputting X/Y coordinates through the ESP32's two DAC pins. Every shape is drawn by steering the oscilloscope beam through a sequence of points fast enough that persistence of vision fuses them into a visible image.

---

## Features

- **2D primitives** — lines, rectangles, circles, ellipses, cubic Bézier curves
- **Raster rendering** — display any pixel grid as a vector image
- **3D wireframe rendering** — perspective projection with full pitch/yaw/roll camera control
- **Scene-buffered 3D with occlusion** — hidden-line removal via screen-space occlusion testing against opaque triangle faces
- **Vector font** — all 95 printable ASCII characters, scalable, with left/right/center justification
- **Near-plane and screen-space clipping** — Liang–Barsky clipping prevents geometry artifacts at extreme camera angles

---

## Hardware Setup

### What you need

- An ESP32 development board (any board that exposes GPIO25 and GPIO26)
- An oscilloscope with XY mode capability (at least two channels, and software / hardware support)
- Two oscilloscope signal cables (BNC)

### Wiring

| ESP32 Pin | Oscilloscope Input | Signal    |
|-----------|--------------------|-----------|
| GPIO 25   | CH1 (or X input)   | X axis    |
| GPIO 26   | CH2 (or Y axis)    | Y axis    |
| GND       | Ground (both probes)| Reference |

- Connect the oscilloscope probe grounds to the ESP32's GND. Use short cables to minimize noise.
- The two channels can be swapped with the XYdraw() constructor function.

> **Note:** GPIO25 and GPIO26 are the only two DAC-capable pins on most ESP32 variants. They output 0–3.3 V corresponding to DAC values 0–255.

### Oscilloscope Settings

1. Set the oscilloscope to **XY display mode** (often under the Time Base or Display menu).
2. Set both channels to the **same voltage/division** setting.
3. Adjust position and scale so the trace fills the screen. The library outputs values 0–255 on each axis, centered around 128.
4. Turn off any channel filters if available — the ESP32 DAC output is already smooth enough at drawing speeds.

---

## XYdraw Editor

Designing geometry by writing coordinates by hand is tedious. **XYdraw Editor** is a companion desktop GUI — built in Python/Tkinter — that lets you draw 2D shapes and 3D scenes visually and then export ready-to-paste C++ functions that call the XYdraw API.

- 2D canvas with lines, rectangles, circles, ellipses, Bézier curves, and raster grids
- 3D editor with an orbit camera, translation/rotation/scale gizmos, and OBJ/STL import
- Occlusion preview that matches the scene-buffered hidden-line removal exactly
- One-click export: copies a complete C++ function (static or offset coordinates, Allman or K&R braces) to the clipboard or saves it straight to a `.cpp` / `.ino` file

→ **[github.com/CITROMOSEPER/XYdraw-editor](https://github.com/CITROMOSEPER/XYdraw-editor)**

---

## Installation

**Via the Arduino IDE Library Manager:** search for `XYdraw` and click Install.

**Manual install:**
1. Download or clone this repository.
2. Copy the folder into your Arduino `libraries/` directory.
3. Restart the Arduino IDE.

---

## Quick Start

```cpp
#include <XYdraw.h>

// XYdraw(x_pin, y_pin, refine)
// x_pin / y_pin : DAC-capable GPIO pins (25 and 26 on standard ESP32)
// refine        : number of interpolation steps per curve/line (higher = smoother, slower)
XYdraw draw(25, 26, 50);

void setup()
{
  draw.begin();
}

void loop()
{
  // Draw a diagonal line across the screen
  draw.line(0, 0, 255, 255);

  // Draw a circle in the centre
  draw.circle(128, 128, 60);
}
```

The screen refreshes continuously as long as `loop()` keeps calling drawing functions. There is no frame buffer — each call to a drawing function immediately outputs DAC values.

---

## Coordinate System

The drawing canvas is a square from **(0, 0)** to **(255, 255)**.

- **(0, 0)** is the **bottom-left** corner (X = 0 V, Y = 0 V).
- **(255, 255)** is the **top-right** corner (X ≈ 3.3 V, Y ≈ 3.3 V).
- Increasing X moves right; increasing Y moves up.

> The exact orientation depends on how you connect and configure your oscilloscope channels. Swap the X and Y wires, or use the oscilloscope's channel invert function, to flip either axis if your image appears mirrored.

---

## API Reference

### Constructor and Setup

```cpp
XYdraw draw(int x_pin, int y_pin, unsigned int refine);
```
Creates the drawing object. `x_pin` and `y_pin` must be DAC-capable GPIO pins (25 and 26 on standard ESP32 modules). `refine` sets the initial interpolation quality (see [The `refine` Parameter](#the-refine-parameter)).

```cpp
void begin();
```
Initialises the DAC pins. Call once in `setup()`.

```cpp
void set_refine(unsigned int refine);
```
Changes the interpolation quality at runtime. A value of 0 is silently clamped to 1.

---

### 2D Drawing

```cpp
void line(int start_x, int start_y, int end_x, int end_y);
```
Draws a straight line segment. Coordinates outside [0, 255] are clamped to the DAC range.

```cpp
void rectangle(int x, int y, int x_length, int y_length);
```
Draws a rectangle outline. `(x, y)` is the bottom-left corner; `x_length` and `y_length` are the side lengths in DAC units.

```cpp
void circle(int origin_x, int origin_y, int radius);
```
Draws a circle.

```cpp
void ellipse(int origin_x, int origin_y, int radius_x, int radius_y);
```
Draws an ellipse with independent horizontal and vertical radii.

```cpp
void bezier_curve(int anchor1_x, int anchor1_y,
                  int anchor2_x, int anchor2_y,
                  int cp1_x,     int cp1_y,
                  int cp2_x,     int cp2_y);
```
Draws a cubic Bézier curve. `anchor1` and `anchor2` are the start and end points; `cp1` and `cp2` are the two control points.

```cpp
void render_raster(bool raster[], int x_size, int y_size);
```
Renders a flat (1D) boolean pixel array as a raster image scaled to fill the 0–255 canvas. A value of `true` (or `1`) draws that pixel as a small rectangle; `false` (or `0`) leaves it blank. The array is ordered row by row, top row first, left to right within each row. See the `raster` example for how to convert a 2D matrix to this format.

---

### 3D Rendering — Regular (no occlusion)

In this mode every call draws immediately. Polygons are wireframe only — no face fills, no hidden-line removal. Use it for simple wireframe previews or when `FB_MAX_POLYGONS` would be too small for the scene-buffered mode.

```cpp
void threeD_line(int start_x, int start_y, int start_z,
                 int end_x,   int end_y,   int end_z,
                 int cam_x,   int cam_y,   int cam_z,
                 int cam_x_dir, int cam_y_dir, int cam_z_dir);
```
Projects and draws a 3D line segment from world-space to screen-space.

```cpp
void polygon(int p1_x, int p1_y, int p1_z,
             int p2_x, int p2_y, int p2_z,
             int p3_x, int p3_y, int p3_z,
             int cam_x,     int cam_y,     int cam_z,
             int cam_x_dir, int cam_y_dir, int cam_z_dir);
```
Draws the three edges of a triangular face as projected 3D lines. This is a convenience wrapper around three `threeD_line` calls and produces no filled face.

**Camera parameters (shared by both functions):**

| Parameter    | Meaning                                              |
|--------------|------------------------------------------------------|
| `cam_x/y/z`  | Camera position in world space                       |
| `cam_x_dir`  | **Pitch** — tilt up (+) or down (−), in degrees      |
| `cam_y_dir`  | **Yaw** — turn right (+) or left (−), in degrees     |
| `cam_z_dir`  | **Roll** — tilt clockwise (+) or counter-clockwise (−), in degrees |

The camera uses **YXZ intrinsic Euler angles**: yaw is applied first, then pitch around the already-yawed camera axis, then roll. This matches a standard first-person camera where pitch never causes the scene to spin sideways.

---

### 3D Rendering — Scene-Buffered with Occlusion

This mode collects all geometry into a scene buffer, then projects and renders the entire scene in one `render_scenebuffer()` call, performing screen-space hidden-line removal.

**Usage per frame:**
```cpp
draw.clear_scenebuffer();
draw.add_threeD_line_to_scenebuffer(...);  // add as many as needed
draw.add_polygon_to_scenebuffer(...);       // add as many as needed
draw.render_scenebuffer(cam_x, cam_y, cam_z, pitch, yaw, roll);
```

```cpp
void clear_scenebuffer();
```
Discards all staged geometry. Call at the start of each frame.

```cpp
void add_threeD_line_to_scenebuffer(int start_x, int start_y, int start_z,
                                    int end_x,   int end_y,   int end_z);
```
Stages a 3D line segment. Silently ignored if `FB_MAX_LINES` is exceeded.

```cpp
void add_polygon_to_scenebuffer(int p1_x, int p1_y, int p1_z,
                                int p2_x, int p2_y, int p2_z,
                                int p3_x, int p3_y, int p3_z);
```
Stages an opaque triangular polygon. Polygons occlude both staged lines and each other's edges. Silently ignored if `FB_MAX_POLYGONS` is exceeded.

```cpp
void render_scenebuffer(int cam_x,     int cam_y,     int cam_z,
                        int cam_x_dir, int cam_y_dir, int cam_z_dir);
```
Projects, clips, performs hidden-line removal, and draws everything in the scene buffer. Camera parameters are the same as for `threeD_line`.

**Occlusion model:** Each line segment (and each polygon outline edge) is split wherever it crosses the projected edge of any opaque polygon. Each resulting sub-segment is tested at its midpoint: if the midpoint falls inside the 2D projected triangle *and* the polygon is closer to the camera, the sub-segment is hidden and skipped. A small depth bias (`DEPTH_EPS = 0.5`) prevents a polygon from occluding its own outline edges.

**Note:** Quads and larger faces must be decomposed into pairs of triangles before adding them to the scene buffer.

```cpp
bool check_collision_with_camera(unsigned int radius);
```
Returns `true` if the camera sphere (centred at the camera's last position, with the given `radius`) intersects any polygon currently in the scene buffer. Call this *after* `render_scenebuffer()` so the camera position is up to date. Useful for simple collision detection in interactive applications.

---

### String Rendering

All string functions share the same size and spacing parameters:
- `size` — the height unit of each character in DAC pixels. A value of 10–20 is typically readable.
- `letter_distance` — additional horizontal gap between characters, in DAC pixels.

Characters are approximately `2 × size` wide and `3 × size` tall. All 95 printable ASCII characters (space through `~`) are supported; unrecognised characters render as a diamond with a question mark.

```cpp
void render_left_justified_string(String input_string,
                                  int x, int y,
                                  int size, int letter_distance);
```
Renders the string with its left edge at `(x, y)`.

```cpp
void render_right_justified_string(String input_string,
                                   int x, int y,
                                   int size, int letter_distance);
```
Renders the string with its right edge at `(x, y)`.

```cpp
void render_centered_string(String input_string,
                            int x, int y,
                            int size, int letter_distance);
```
Renders the string centred horizontally on `x`.

```cpp
void render_letter(char letter, int x, int y, int offset, int size);
```
Renders a single character. `offset` is added to `x` before drawing, allowing you to position individual glyphs within a line manually.

---

## The `refine` Parameter

`refine` controls the number of intermediate points the library outputs when drawing curves and lines. More points means a smoother, brighter trace at the cost of drawing time.

| Shape         | Effect of increasing `refine`                          |
|---------------|--------------------------------------------------------|
| `line`        | More DAC steps along the segment — useful for long lines |
| `circle`      | More angular steps — rounder at high values            |
| `ellipse`     | Same as circle                                         |
| `bezier_curve`| More curve samples — smoother splines                  |
| `rectangle`   | Applied to each of the four sides                      |
| 3D functions  | Applied to each projected line segment                 |

`render_raster` temporarily overrides `refine` to 2 internally and restores it afterwards.

**Starting values:** 20–50 works well for most scenes. Higher than 100 gives diminishing returns and noticeably slows the frame rate.

---

## Scene Buffer Capacities

These constants are defined at the top of `XYdraw.h` and can be increased if your scene requires more objects:

```cpp
#define FB_MAX_LINES    64   // max 3D line segments  (12 bytes each)
#define FB_MAX_POLYGONS 32   // max triangular faces  (18 bytes each)
```

Increasing these raises RAM usage slightly (both values are stored as compact `int16_t` structs). The occlusion pass also takes longer with more polygons — each line segment is tested against every projected triangle.

---

## Examples

| Sketch | Demonstrates |
|--------|-------------|
| `line` | Animated 2D lines sweeping and rotating across the screen |
| `rectangle` | Expanding and contracting rectangles |
| `circle` | Moving, pulsing, and random circles |
| `ellipse` | Wobbling ellipse and random ellipses |
| `bezier_curve` | Animated Bézier curves with changing anchors and control points; effect of `refine` |
| `raster` | Displaying a 10×10 pixel smiley face via a boolean matrix |
| `regular_3D_rendering` | Wireframe cube with a pyramid roof, camera orbiting in 3D |
| `scenebuffered_occlusion_3D_rendering` | Same cube, but solid-faced with hidden-line removal |
| `string_rendering` | Left/right/center justified strings animating letter by letter |

---

## Performance Notes

- The ESP32 DAC update rate limits the maximum detail and frame rate simultaneously. For a stable, bright trace, keep the total number of DAC writes per frame below roughly 2 000–4 000 (tune by eye with your oscilloscope's persistence setting).
- Each `line()` call costs `refine + 1` DAC writes. Each `circle()` or `ellipse()` costs `refine` writes. Each `bezier_curve()` costs `refine + 1` writes.
- Scene-buffered rendering with occlusion has O(L × P) complexity per frame, where L is the number of line sub-segments and P is the number of projected polygons. For complex scenes, reduce `refine` before increasing `FB_MAX_POLYGONS`.
- The `delay()` calls in the example sketches are there for visual effect only — they slow animation so individual frames are visible on the scope. Remove them in production code to maximise brightness.

---

## Limitations

- **ESP32 only.** The library calls `dacWrite()`, which is specific to the ESP32 Arduino core. It is not compatible with other Arduino boards.
- **Monochrome output.** The oscilloscope beam has a single intensity; there is no colour or brightness control from software.
- **No filled polygons.** The renderer draws outlines only. Solid fills would require a different display technique (e.g. raster scanning).
- **Integer world coordinates.** The 3D functions accept `int` coordinates. For very fine geometry, scale your world up.
- **Scene buffer overflow is silent.** Objects added beyond `FB_MAX_LINES` / `FB_MAX_POLYGONS` are discarded without warning.

---

## Author

Nemes Dániel