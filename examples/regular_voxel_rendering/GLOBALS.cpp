#include <Arduino.h>
#include <GLOBALS.h>
#include <PINS.h>


// Global input state variables
float JOY_L_X = 0;
float JOY_L_Y = 0;
float JOY_R_X = 0;
float JOY_R_Y = 0;
int8_t JOY_L_X_FLICK = 0;
int8_t JOY_L_Y_FLICK = 0;
int8_t JOY_R_X_FLICK = 0;
int8_t JOY_R_Y_FLICK = 0;

float LT = 0;
float RT = 0;
bool LT_FLICK = 0;
bool RT_FLICK = 0;

bool LB = 0;
bool RB = 0;

bool JOY_L_B = 0;
bool JOY_R_B = 0;

bool DPAD_L_A = 0;
bool DPAD_L_B = 0;
bool DPAD_L_C = 0;
bool DPAD_L_D = 0;

bool DPAD_R_A = 0;
bool DPAD_R_B = 0;
bool DPAD_R_C = 0;
bool DPAD_R_D = 0;

bool RESUME_BUTTON = 0;
bool PAUSE_BUTTON = 0;

// Rising edge state of global input state variables
bool RE_LB = 0;
bool RE_RB = 0;

bool RE_JOY_L_B = 0;
bool RE_JOY_R_B = 0;

bool RE_DPAD_L_A = 0;
bool RE_DPAD_L_B = 0;
bool RE_DPAD_L_C = 0;
bool RE_DPAD_L_D = 0;

bool RE_DPAD_R_A = 0;
bool RE_DPAD_R_B = 0;
bool RE_DPAD_R_C = 0;
bool RE_DPAD_R_D = 0;

bool RE_RESUME_BUTTON = 0;
bool RE_PAUSE_BUTTON = 0;

// Falling edge state of global input state variables
bool FE_LB = 0;
bool FE_RB = 0;

bool FE_JOY_L_B = 0;
bool FE_JOY_R_B = 0;

bool FE_DPAD_L_A = 0;
bool FE_DPAD_L_B = 0;
bool FE_DPAD_L_C = 0;
bool FE_DPAD_L_D = 0;

bool FE_DPAD_R_A = 0;
bool FE_DPAD_R_B = 0;
bool FE_DPAD_R_C = 0;
bool FE_DPAD_R_D = 0;

bool FE_RESUME_BUTTON = 0;
bool FE_PAUSE_BUTTON = 0;


void HANDLE_GLOBAL_INPUTS()
{
 // Assign last values
  float LAST_JOY_L_X = JOY_L_X;
  float LAST_JOY_L_Y = JOY_L_Y;
  float LAST_JOY_R_X = JOY_R_X;
  float LAST_JOY_R_Y = JOY_R_Y;

  float LAST_LT = LT;
  float LAST_RT = RT;

  bool LAST_LB = LB;
  bool LAST_RB = RB;

  bool LAST_JOY_L_B = JOY_L_B;
  bool LAST_JOY_R_B = JOY_R_B;

  bool LAST_DPAD_L_A = DPAD_L_A;
  bool LAST_DPAD_L_B = DPAD_L_B;
  bool LAST_DPAD_L_C = DPAD_L_C;
  bool LAST_DPAD_L_D = DPAD_L_D;

  bool LAST_DPAD_R_A = DPAD_R_A;
  bool LAST_DPAD_R_B = DPAD_R_B;
  bool LAST_DPAD_R_C = DPAD_R_C;
  bool LAST_DPAD_R_D = DPAD_R_D;

  bool LAST_RESUME_BUTTON = RESUME_BUTTON;
  bool LAST_PAUSE_BUTTON = PAUSE_BUTTON;

 // Get hardware values
  JOY_L_X = ((analogRead(JOY_L_X_PIN) / 4095.0) - 0.5) * 2;
  JOY_L_Y = ((analogRead(JOY_L_Y_PIN) / 4095.0) - 0.5) * -2;
  JOY_R_X = ((analogRead(JOY_R_X_PIN) / 4095.0) - 0.5) * -2;
  JOY_R_Y = ((analogRead(JOY_R_Y_PIN) / 4095.0) - 0.5) * 2;
  if (abs(JOY_L_X) < 0.1)  JOY_L_X = 0.0;
  if (abs(JOY_L_Y) < 0.1)  JOY_L_Y = 0.0;
  if (abs(JOY_R_X) < 0.1)  JOY_R_X = 0.0;
  if (abs(JOY_R_Y) < 0.1)  JOY_R_Y = 0.0;

  LT = (analogRead(LT_PIN) / 4095.0);
  RT = 1.0 - (analogRead(RT_PIN) / 4095.0);

  LB = digitalRead(LB_PIN);
  RB = digitalRead(RB_PIN);

  JOY_L_B = digitalRead(JOY_L_B_PIN);
  JOY_R_B = digitalRead(JOY_R_B_PIN);

  DPAD_L_A = digitalRead(DPAD_L_A_PIN);
  DPAD_L_B = digitalRead(DPAD_L_B_PIN);
  DPAD_L_C = digitalRead(DPAD_L_C_PIN);
  DPAD_L_D = digitalRead(DPAD_L_D_PIN);

  DPAD_R_A = digitalRead(DPAD_R_A_PIN);
  DPAD_R_B = digitalRead(DPAD_R_B_PIN);
  DPAD_R_C = digitalRead(DPAD_R_C_PIN);
  DPAD_R_D = digitalRead(DPAD_R_D_PIN);

  RESUME_BUTTON = digitalRead(RESUME_BUTTON_PIN);
  PAUSE_BUTTON =  digitalRead(PAUSE_BUTTON_PIN);

 // Assign rising edge values
  RE_LB = LB && !LAST_LB;
  RE_RB = RB && !LAST_RB;

  RE_JOY_L_B = JOY_L_B && !LAST_JOY_L_B;
  RE_JOY_R_B = JOY_R_B && !LAST_JOY_R_B;

  RE_DPAD_L_A = DPAD_L_A && !LAST_DPAD_L_A;
  RE_DPAD_L_B = DPAD_L_B && !LAST_DPAD_L_B;
  RE_DPAD_L_C = DPAD_L_C && !LAST_DPAD_L_C;
  RE_DPAD_L_D = DPAD_L_D && !LAST_DPAD_L_D;

  RE_DPAD_R_A = DPAD_R_A && !LAST_DPAD_R_A;
  RE_DPAD_R_B = DPAD_R_B && !LAST_DPAD_R_B;
  RE_DPAD_R_C = DPAD_R_C && !LAST_DPAD_R_C;
  RE_DPAD_R_D = DPAD_R_D && !LAST_DPAD_R_D;

  RE_RESUME_BUTTON = RESUME_BUTTON && !LAST_RESUME_BUTTON;
  RE_PAUSE_BUTTON = PAUSE_BUTTON && !LAST_PAUSE_BUTTON;

 // Assign falling edge values
  FE_LB = !LB && LAST_LB;
  FE_RB = !RB && LAST_RB;

  FE_JOY_L_B = !JOY_L_B && LAST_JOY_L_B;
  FE_JOY_R_B = !JOY_R_B && LAST_JOY_R_B;

  FE_DPAD_L_A = !DPAD_L_A && LAST_DPAD_L_A;
  FE_DPAD_L_B = !DPAD_L_B && LAST_DPAD_L_B;
  FE_DPAD_L_C = !DPAD_L_C && LAST_DPAD_L_C;
  FE_DPAD_L_D = !DPAD_L_D && LAST_DPAD_L_D;

  FE_DPAD_R_A = !DPAD_R_A && LAST_DPAD_R_A;
  FE_DPAD_R_B = !DPAD_R_B && LAST_DPAD_R_B;
  FE_DPAD_R_C = !DPAD_R_C && LAST_DPAD_R_C;
  FE_DPAD_R_D = !DPAD_R_D && LAST_DPAD_R_D;

  FE_RESUME_BUTTON = !RESUME_BUTTON && LAST_RESUME_BUTTON;
  FE_PAUSE_BUTTON = !PAUSE_BUTTON && LAST_PAUSE_BUTTON;

 // LEFT JOYSTICK
  // flag variables to keep track wether the joystick has been moved to the edge
  static bool joy_l_x_pos_flick_flag = 0;
  static bool joy_l_x_neg_flick_flag = 0;
  static bool joy_l_y_pos_flick_flag = 0;
  static bool joy_l_y_neg_flick_flag = 0;

  if (JOY_L_X_FLICK != 0)
  {
    JOY_L_X_FLICK = 0;
    joy_l_x_pos_flick_flag = 0;
    joy_l_x_neg_flick_flag = 0;
  }

  if (JOY_L_Y_FLICK != 0)
  {
    JOY_L_Y_FLICK = 0;
    joy_l_y_pos_flick_flag = 0;
    joy_l_y_neg_flick_flag = 0;
  }

  if (JOY_L_X > 0.3)  joy_l_x_pos_flick_flag = 1;
  if (JOY_L_X < -0.3) joy_l_x_neg_flick_flag = 1;
  if (JOY_L_Y > 0.3)  joy_l_y_pos_flick_flag = 1;
  if (JOY_L_Y < -0.3) joy_l_y_neg_flick_flag = 1;

  if (joy_l_x_pos_flick_flag && JOY_L_X < 0.2)   JOY_L_X_FLICK = 1;
  if (joy_l_x_neg_flick_flag && JOY_L_X > -0.2)  JOY_L_X_FLICK = -1;
  if (joy_l_y_pos_flick_flag && JOY_L_Y < 0.2)   JOY_L_Y_FLICK = 1;
  if (joy_l_y_neg_flick_flag && JOY_L_Y > -0.2)  JOY_L_Y_FLICK = -1;


 // RIGHT JOYSTICK
  // flag variables to keep track wether the joystick has been moved to the edge
  static bool joy_r_x_pos_flick_flag = 0;
  static bool joy_r_x_neg_flick_flag = 0;
  static bool joy_r_y_pos_flick_flag = 0;
  static bool joy_r_y_neg_flick_flag = 0;

  if (JOY_R_X_FLICK != 0)
  {
    JOY_R_X_FLICK = 0;
    joy_r_x_pos_flick_flag = 0;
    joy_r_x_neg_flick_flag = 0;
  }

  if (JOY_R_Y_FLICK != 0)
  {
    JOY_R_Y_FLICK = 0;
    joy_r_y_pos_flick_flag = 0;
    joy_r_y_neg_flick_flag = 0;
  }

  if (JOY_R_X > 0.3)  joy_r_x_pos_flick_flag = 1;
  if (JOY_R_X < -0.3) joy_r_x_neg_flick_flag = 1;
  if (JOY_R_Y > 0.3)  joy_r_y_pos_flick_flag = 1;
  if (JOY_R_Y < -0.3) joy_r_y_neg_flick_flag = 1;

  if (joy_r_x_pos_flick_flag && JOY_R_X < 0.2)   JOY_R_X_FLICK = 1;
  if (joy_r_x_neg_flick_flag && JOY_R_X > -0.2)  JOY_R_X_FLICK = -1;
  if (joy_r_y_pos_flick_flag && JOY_R_Y < 0.2)   JOY_R_Y_FLICK = 1;
  if (joy_r_y_neg_flick_flag && JOY_R_Y > -0.2)  JOY_R_Y_FLICK = -1;


 // TRIGGERS
  // flag variables to keep track wether the joystick has been moved to the edge
  static bool lt_pos_flick_flag = 0;
  static bool rt_pos_flick_flag = 0;

  if (LT_FLICK != 0)
  {
    LT_FLICK = 0;
    lt_pos_flick_flag = 0;
  }

  if (RT_FLICK != 0)
  {
    RT_FLICK = 0;
    rt_pos_flick_flag = 0;
  }

  if (LT > 0.7)  lt_pos_flick_flag = 1;
  if (RT > 0.7)  rt_pos_flick_flag = 1;

  if (lt_pos_flick_flag && LT < 0.3)   LT_FLICK = 1;
  if (rt_pos_flick_flag && RT < 0.3)   RT_FLICK = 1;
}

void INITIALIZE_PINS()
{
  pinMode(JOY_L_X_PIN, INPUT);
  pinMode(JOY_L_Y_PIN, INPUT);
  pinMode(JOY_R_X_PIN, INPUT);
  pinMode(JOY_R_Y_PIN, INPUT);
  pinMode(LT_PIN, INPUT);
  pinMode(RT_PIN, INPUT);
  pinMode(LB_PIN, INPUT);
  pinMode(RB_PIN, INPUT);
  pinMode(JOY_L_B_PIN, INPUT);
  pinMode(JOY_R_B_PIN, INPUT);
  pinMode(DPAD_L_A_PIN, INPUT);
  pinMode(DPAD_L_B_PIN, INPUT);
  pinMode(DPAD_L_C_PIN, INPUT);
  pinMode(DPAD_L_D_PIN, INPUT);
  pinMode(DPAD_R_A_PIN, INPUT);
  pinMode(DPAD_R_B_PIN, INPUT);
  pinMode(DPAD_R_C_PIN, INPUT);
  pinMode(DPAD_R_D_PIN, INPUT);
  pinMode(RESUME_BUTTON_PIN, INPUT);
  pinMode(PAUSE_BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void PRINT_INPUTS()
{
  Serial.print("JOY_L_X: ");
  Serial.println(JOY_L_X);
  Serial.print("JOY_L_Y: ");
  Serial.println(JOY_L_Y);
  Serial.print("JOY_R_X: ");
  Serial.println(JOY_R_X);
  Serial.print("JOY_R_Y: ");
  Serial.println(JOY_R_Y);

  Serial.print("JOY_L_X_FLICK: ");
  Serial.println(JOY_L_X_FLICK);
  Serial.print("JOY_L_Y_FLICK: ");
  Serial.println(JOY_L_Y_FLICK);
  Serial.print("JOY_R_X_FLICK: ");
  Serial.println(JOY_R_X_FLICK);
  Serial.print("JOY_R_Y_FLICK: ");
  Serial.println(JOY_R_Y_FLICK);

  Serial.print("LT: ");
  Serial.println(LT);
  Serial.print("RT: ");
  Serial.println(RT);
  Serial.print("LT_FLICK: ");
  Serial.println(LT_FLICK);
  Serial.print("RT_FLICK: ");
  Serial.println(RT_FLICK);

  Serial.print("LB: ");
  Serial.println(LB);
  Serial.print("RB: ");
  Serial.println(RB);

  Serial.print("JOY_L_B: ");
  Serial.println(JOY_L_B);
  Serial.print("JOY_R_B: ");
  Serial.println(JOY_R_B);

  Serial.print("DPAD_L_A: ");
  Serial.println(DPAD_L_A);
  Serial.print("DPAD_L_B: ");
  Serial.println(DPAD_L_B);
  Serial.print("DPAD_L_C: ");
  Serial.println(DPAD_L_C);
  Serial.print("DPAD_L_D: ");
  Serial.println(DPAD_L_D);

  Serial.print("DPAD_R_A: ");
  Serial.println(DPAD_R_A);
  Serial.print("DPAD_R_B: ");
  Serial.println(DPAD_R_B);
  Serial.print("DPAD_R_C: ");
  Serial.println(DPAD_R_C);
  Serial.print("DPAD_R_D: ");
  Serial.println(DPAD_R_D);

  Serial.print("RESUME_BUTTON: ");
  Serial.println(RESUME_BUTTON);
  Serial.print("PAUSE_BUTTON: ");
  Serial.println(PAUSE_BUTTON);

  Serial.println("");
}