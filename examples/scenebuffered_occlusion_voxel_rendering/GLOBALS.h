#ifndef GLOBALS_H
#define GLOBALS_H

 
// Global input state variables
extern float JOY_L_X;
extern float JOY_L_Y;
extern float JOY_R_X;
extern float JOY_R_Y;
extern int8_t JOY_L_X_FLICK;
extern int8_t JOY_L_Y_FLICK;
extern int8_t JOY_R_X_FLICK;
extern int8_t JOY_R_Y_FLICK;
 
extern float LT;
extern float RT;
extern bool LT_FLICK;
extern bool RT_FLICK;
 
extern bool LB;
extern bool RB;
 
extern bool JOY_L_B;
extern bool JOY_R_B;
 
extern bool DPAD_L_A;
extern bool DPAD_L_B;
extern bool DPAD_L_C;
extern bool DPAD_L_D;
 
extern bool DPAD_R_A;
extern bool DPAD_R_B;
extern bool DPAD_R_C;
extern bool DPAD_R_D;
 
extern bool RESUME_BUTTON;
extern bool PAUSE_BUTTON;
 
 
// Rising edge state of global input state variables
extern bool RE_LB;
extern bool RE_RB;
 
extern bool RE_JOY_L_B;
extern bool RE_JOY_R_B;
 
extern bool RE_DPAD_L_A;
extern bool RE_DPAD_L_B;
extern bool RE_DPAD_L_C;
extern bool RE_DPAD_L_D;
 
extern bool RE_DPAD_R_A;
extern bool RE_DPAD_R_B;
extern bool RE_DPAD_R_C;
extern bool RE_DPAD_R_D;

extern bool RE_RESUME_BUTTON;
extern bool RE_PAUSE_BUTTON;


// Falling edge state of global input state variables
extern bool FE_LB;
extern bool FE_RB;

extern bool FE_JOY_L_B;
extern bool FE_JOY_R_B;

extern bool FE_DPAD_L_A;
extern bool FE_DPAD_L_B;
extern bool FE_DPAD_L_C;
extern bool FE_DPAD_L_D;

extern bool FE_DPAD_R_A;
extern bool FE_DPAD_R_B;
extern bool FE_DPAD_R_C;
extern bool FE_DPAD_R_D;

extern bool FE_RESUME_BUTTON;
extern bool FE_PAUSE_BUTTON;


void HANDLE_GLOBAL_INPUTS();
void INITIALIZE_PINS();
void PRINT_INPUTS();

#endif