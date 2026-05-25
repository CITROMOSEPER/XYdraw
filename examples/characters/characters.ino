// This program is meant to showcase the font library of the XYdraw library
// ESP32
// Nemes Dániel


#include <PINS.h>
#include <GLOBALS.h>

#include <XYdraw.h>
XYdraw XYdraw(CH_X_PIN, CH_Y_PIN, 30);


String characters[] = {
  "!", "\"", "#", "$", "%", "&", "\'", "(", ")", "*", "+", ",", "-", ".", "/", ":", ";", "<", ">", "=", "?", "@", "[", "]", "\\", "`", "_", "^", "{", "}", "|", "~", "Đ",  // 33
  "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",                                                                                                                        // 10
  "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",                                        // 26
  "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"                                         // 26    Total = 95
};

bool BOUNDINGBOX = false;
int8_t ID = 0;
uint8_t SIZE = 10;


void setup()
{
  Serial.begin(115200);
  INITIALIZE_PINS();
}

void loop()
{
  HANDLE_GLOBAL_INPUTS();

  if (RE_PAUSE_BUTTON)  BOUNDINGBOX = !BOUNDINGBOX;
  if (RE_DPAD_L_B)  SIZE++;
  if (RE_DPAD_L_D)  SIZE--;
  if (RE_RB)  ID++;
  if (RE_LB)  ID--;
  if (ID > 94)  ID = 0;
  if (ID < 0)  ID = 94;

  XYdraw.render_centered_string(characters[ID], 127, 127, SIZE, SIZE * 2);
  if (BOUNDINGBOX)
  {
    XYdraw.line(127, 127, 127 + SIZE * 2, 127);
    XYdraw.line(127 + SIZE * 2, 127, 127 + SIZE * 2, 127 + SIZE * 3);
    XYdraw.line(127 + SIZE * 2, 127 + SIZE * 3, 127, 127 + SIZE * 3);
    XYdraw.line(127 + SIZE * 2, 127 + SIZE * 3, 127, 127);
  }
}
