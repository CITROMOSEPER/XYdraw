// XYdraw library raster rendering demonstration
// ESP32
// Nemes Dániel
//
// This file demonstrates the raster rendering capability of the XYdraw library
// void render_raster(bool raster[], int x_size, int y_size);


#include <XYdraw.h>
XYdraw draw(25, 26, 10);  // This is the constructor function. Its arguments are: X channel pin, Y channel pin, refine value (this can be changed later)


const int x_size = 10;
const int y_size = 10;

// image matrix
bool raster[x_size][y_size] =
{
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 1, 0, 0, 1, 1, 0, 0},
  {0, 0, 1, 1, 0, 0, 1, 1, 0, 0},
  {0, 0, 1, 1, 0, 0, 1, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
  {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
  {0, 0, 1, 1, 0, 0, 1, 1, 0, 0},
  {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};


void setup()
{
  draw.begin();
}

void loop()
{
  // convert the 2D matrix into a temporary 1D array that can be passed into the render_raster() function
  bool passed_image[x_size * y_size];     // temporary 1D array
  int i = 0;                              // index for the temporary array
  for (int y = y_size - 1; y >= 0; y--)   // iterate through the rows of the matrix
  {
    for (int x = 0; x < x_size; x++)      // iterate throught the columns of the matrix
    {
      passed_image[i] = raster[y][x];     // do the actual conversion for each pixel
      i++;                                // increment temporary array index
    }
  }
  // the flattening is done every loop so the image can be modified dynamically - useful for applications like paint programs
  // if the matrix is constant in your program, then you should do this only once in setup() to avoid allocating extra heap memory for the temporary array

  // actually render the matrix
  draw.render_raster(passed_image, x_size, y_size);
}