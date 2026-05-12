// XYdraw library 2D line demonstration
// ESP32
// Nemes Dániel
//
// This file demonstrates the 2D line capability of the XYdraw library
// void line(int start_x, int start_y, int end_x, int end_y);


#include <XYdraw.h>
XYdraw draw(25, 26, 50);  // This is the constructor function. Its arguments are: X channel pin, Y channel pin, refine value (this can be changed later)


void setup()
{
  draw.begin();
}

void loop()
{
  // move a line across the screen left to right
  for (int i = 0; i < 256; i++)
  {
    draw.line(i, 0, i, 255);
    delay(5);
  }
  // move a line across the screen bottom to top
  for (int i = 0; i < 256; i++)
  {
    draw.line(0, i, 255, i);
    delay(5);
  }

  // rotate a line across the screen counterclockwise
  for (int i = 0; i < 256; i++)
  {
    draw.line(i, 0, 255 - i, 255);
    delay(5);
  }
  for (int i = 255; i >= 0; i--)
  {
    draw.line(0, i, 255, 255 - i);
    delay(5);
  }

  // draw random lines on the screen
  for (int i = 0; i < 256; i++)
  {
    draw.line(random(0, 255), random(0, 255), random(0, 255), random(0, 255));
    delay(5);
  }
}
