// XYdraw library rectangle demonstration
// ESP32
// Nemes Dániel
//
// This file demonstrates the rectangle capability of the XYdraw library
// void rectangle(int x, int y, int x_length, int y_length);


#include <XYdraw.h>
XYdraw draw(25, 26, 50);  // This is the constructor function. Its arguments are: X channel pin, Y channel pin, refine value (this can be changed later)


void setup()
{
  draw.begin();
}

void loop()
{
  // stretch out a rectangle from the bottom left corner to the top right corner
  for (int i = 0; i < 256; i++)
  {
    draw.rectangle(0, 0, i, i);
    delay(5);
  }

  // shrink down a rectangle from the bottom left corner to the top right corner
  for (int i = 0; i < 256; i++)
  {
    draw.rectangle(i, i, 255 - i, 255 - i);
    delay(5);
  }

  // stretch a rectangle from the left edge to the right edge
  for (int i = 0; i < 256; i++)
  {
    draw.rectangle(0, 0, i, 255);
    delay(5);
  }

  // stretch a rectangle from the bottom edge to the top edge
  for (int i = 0; i < 256; i++)
  {
    draw.rectangle(0, 0, 255, i);
    delay(5);
  }

  // draw random rectangles on the screen
  for (int i = 0; i < 256; i++)
  {
    draw.rectangle(random(0, 255), random(0, 255), random(0, 255), random(0, 255));
    delay(5);
  }
}