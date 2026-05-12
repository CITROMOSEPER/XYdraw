// XYdraw library ellipse demonstration
// ESP32
// Nemes Dániel
//
// This file demonstrates the ellipse capability of the XYdraw library
// void ellipse(int origin_x, int origin_y, int radius_x, int radius_y);


#include <XYdraw.h>
XYdraw draw(25, 26, 50);  // This is the constructor function. Its arguments are: X channel pin, Y channel pin, refine value (this can be changed later)


void setup()
{
  draw.begin();
}

void loop()
{
  // grow an ellipse out from the center
  for (int i = 0; i < 100; i++)
  {
    draw.ellipse(128, 128, i, i / 2);
    delay(5);
  }

  // wobble the created ellipse
  for (int i = 0; i < 50; i++)
  {
    draw.ellipse(128, 128, 100 - i, 50 + i);
    delay(5);
  }
  for (int i = 0; i < 50; i++)
  {
    draw.ellipse(128, 128, 50 + i, 100 - i);
    delay(5);
  }
  for (int i = 0; i < 50; i++)
  {
    draw.ellipse(128, 128, 100 - i, 50 + i);
    delay(5);
  }
  for (int i = 0; i < 50; i++)
  {
    draw.ellipse(128, 128, 50 + i, 100 - i);
    delay(5);
  }

  // draw random ellipses on the screen
  for (int i = 0; i < 256; i++)
  {
    draw.ellipse(random(0, 255), random(0, 255), random(0, 128), random(0, 128));
    delay(5);
  }
}