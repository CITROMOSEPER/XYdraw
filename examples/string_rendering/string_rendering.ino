// XYdraw library string rendering demonstration
// ESP32
// Nemes Dániel
//
// This file demonstrates the string rendering capability of the XYdraw library
// void render_left_justified_string(String input_string, int x, int y, int size, int letter_distance;
// void render_right_justified_string(String input_string, int x, int y, int size, int letter_distance;
// void render_centered_string(String input_string, int x, int y, int size, int letter_distance);


#include <XYdraw.h>
XYdraw draw(25, 26, 20);  // This is the constructor function. Its arguments are: X channel pin, Y channel pin, refine value (this can be changed later)


const int repeat = 100;


void update_texts(String left_text, String right_text, String center_text)
{
  // draw each string justified to the corresponding side
  draw.render_left_justified_string(left_text, 0, 255, 15, 20);
  draw.render_right_justified_string(right_text, 255, 65, 15, 20);
  draw.render_centered_string(center_text, 128, 150, 15, 20);
}

void setup()
{
  draw.begin();
}

void loop()
{
  // start with empty strings
  String left_text = "";
  String right_text = "";
  String center_text = "";

  // gradually add the letters to each string, as if they were typed, and display them until the repeat counter reaches 0
  for (int i = 0; i < repeat; i++)
  {
    update_texts(left_text, right_text, center_text);
  }

  left_text = "L";
  right_text = "t";
  center_text = "t";
  for (int i = 0; i < repeat; i++)
  {
    update_texts(left_text, right_text, center_text);
  }

  left_text = "Le";
  right_text = "ht";
  center_text = "nt";
  for (int i = 0; i < repeat; i++)
  {
    update_texts(left_text, right_text, center_text);
  }

  left_text = "Lef";
  right_text = "ght";
  center_text = "nte";
  for (int i = 0; i < repeat; i++)
  {
    update_texts(left_text, right_text, center_text);
  }

  left_text = "Left";
  right_text = "ight";
  center_text = "ente";
  for (int i = 0; i < repeat; i++)
  {
    update_texts(left_text, right_text, center_text);
  }

  left_text = "Left";
  right_text = "Right";
  center_text = "Cente";
  for (int i = 0; i < repeat; i++)
  {
    update_texts(left_text, right_text, center_text);
  }

  left_text = "Left";
  right_text = "Right";
  center_text = "Center";
  for (int i = 0; i < repeat; i++)
  {
    update_texts(left_text, right_text, center_text);
  }
}
