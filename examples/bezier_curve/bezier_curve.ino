// XYdraw library bézier curve demonstration
// ESP32
// Nemes Dániel
//
// This file demonstrates the bézier curve capability of the XYdraw library
// void bezier_curve(int anchor1_x, int anchor1_y, int anchor2_x, int anchor2_y, 
//                   int cp1_x,     int cp1_y,     int cp2_x,     int cp2_y);


#include <XYdraw.h>
XYdraw draw(25, 26, 50);  // This is the constructor function. Its arguments are: X channel pin, Y channel pin, refine value (this can be changed later)


void setup()
{
  draw.begin();
}

void loop()
{
  // animate a bézier curve changing its control points
  for (int i = 0; i < 256; i++)
  {
    draw.bezier_curve(0, 0, 255, 255, 255 - i, i, i, 255 - i);
    delay(5);
  }
  for (int i = 255; i >= 0; i--)
  {
    draw.bezier_curve(0, 0, 255, 255, 255 - i, i, i, 255 - i);
    delay(5);
  }

  // animate a bézier curve changing its anchor points along with its control points
  for (int i = 0; i < 256; i++)
  {
    draw.bezier_curve(0, i, 255, 255 - i, 255 - i, 0, i, 255);
    delay(5);
  }
  for (int i = 255; i >= 0; i--)
  {
    draw.bezier_curve(0, i, 255, 255 - i, 255 - i, 0, i, 255);
    delay(5);
  }

  // animate a bézier curve with different refine values
  for (int i = 0; i < 100; i++)
  {
    draw.set_refine(i);
    draw.bezier_curve(0, 0, 255, 255, 255, 0, 0, 255);
    delay(50);
  }
  draw.set_refine(50);

  // draw random bézier curves on the screen
  for (int i = 0; i < 256; i++)
  {
    draw.bezier_curve(random(0, 255), random(0, 255), random(0, 255), random(0, 255), random(0, 255), random(0, 255), random(0, 255), random(0, 255));
    delay(5);
  }
}