// XYdraw — buffered mode example (src_new)
//
// Frames are drawn into RAM and replayed continuously by a task on the other
// CPU core, so the scope image stays steady no matter how long the sketch
// takes to compute the next frame.

#include <XYdraw.h>

XYdraw draw(25, 26, 50);

int angle = 0;

void setup()
{
  Serial.begin(115200);

  // 16384 points per frame, 1,000,000 points per second.
  if (!draw.begin_buffered(16384, 1000000))
  {
    Serial.println("Buffered mode failed, using direct mode");
  }

  // Optional: sample lines by length instead of a fixed point count.
  // Short lines (text, small details) then cost very few points.
  draw.set_point_spacing(3.0);
}

void loop()
{
  // ── build the frame ───────────────────────────────────────────────────────
  for (int i = 0; i < 40; i++)
  {
    float a = radians(angle + i * 9);
    draw.line(128, 128, 128 + 120 * cos(a), 128 + 120 * sin(a));
  }
  draw.circle(128, 128, 60);
  draw.render_centered_string("XYdraw", 128, 40, 8, 4);

  // ── hand it to the output core ───────────────────────────────────────────
  draw.present();

  angle = (angle + 1) % 360;

  static uint32_t last = 0;
  if (millis() - last > 1000)
  {
    last = millis();
    Serial.printf("points/frame: %u  refresh: %.1f Hz  overflow: %d\n",
                  draw.frame_points(), draw.frame_rate(), draw.frame_overflowed());
  }
}
