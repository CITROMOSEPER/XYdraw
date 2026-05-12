// XYdraw library circle demonstration
// ESP32
// Nemes Dániel
//
// This file demonstrates the circle capability of the XYdraw library
// void circle(int origin_x, int origin_y, int radius);


#include <XYdraw.h>
XYdraw draw(25, 26, 50);  // This is the constructor function. Its arguments are: X channel pin, Y channel pin, refine value (this can be changed later)


void setup()
{
  draw.begin();
}

void loop()
{
  // stretch out a circle from the bottom left corner to the center
  for (int i = 0; i < 128; i++)
  {
    draw.circle(i, i, i);
    delay(5);
  }

  // shrink down a circle from the center to the top right corner
  for (int i = 128; i < 256; i++)
  {
    draw.circle(i, i, 255 - i);
    delay(5);
  }

  // pulse a circle at the centre of the screen
  for (int i = 0; i < 128; i++)
  {
    draw.circle(128, 128, i);
    delay(5);
  }
  for (int i = 128; i > 0; i--)
  {
    draw.circle(128, 128, i);
    delay(5);
  }

  // draw random circles on the screen
  for (int i = 0; i < 256; i++)
  {
    draw.circle(random(0, 256), random(0, 256), random(0, 128));
    delay(5);
  }
}