#include "msp.h"
#include "../inc/Reflectance.h"
#include "../inc/Clock.h"
#include "../inc/Motor.h"
#include "../inc/LaunchPad.h"
#include "../inc/SysTickInts.h"
#include "../inc/CortexM.h"
#include "../inc/BumpInt.h"

void motorState(uint8_t state);
void SysTick_Handler(void);

struct State {
  uint32_t out;//2-bit output
  const struct State *next[4]; // Next if 2-bit input is 0-3
};
typedef const struct State State_t;

#define Center   &fsm[0]
#define Left     &fsm[1]
#define Right    &fsm[2]
//#define Lost     &fsm[4]

State_t fsm[4]={
    {0x1, {Right, Left, Right, Center}},   // Center
    {0x2, {Left, Center, Right, Center}},   // Left
    {0x3, { Right, Left,   Center, Center}},   // Right
  //  {0x4, { }},     // Lost
};

State_t *StatePtr;  // pointer to the current state
uint8_t Input;
uint8_t Output;
volatile uint8_t refl_data;

int main(void){
  Clock_Init48MHz();
  Motor_Init();
  Reflectance_Init();
  SysTick_Init(48000, 2);
  LaunchPad_Init();
 // BumpInt_Init(parameter);

  StatePtr = Center;

  EnableInterrupts();

  while(1)
      WaitForInterrupt();
}

void motorState(uint8_t state) {
    switch(state){
        case 0x1:
            Motor_Forward(3000, 3000); // Center
            break;
        case 0x2:
            Motor_Left(0, 2000); // Left
            break;
        case 0x3:
            Motor_Right(2000, 0); // Right
            break;
        case 0x4:
            Motor_Stop(); // Lost
            break;
        default:
            break;
    }
}

void SysTick_Handler(void){
    volatile static uint8_t count = 0;

    if(count == 0)
        Reflectance_Start();

    else if(count == 1) {
        Input = Reflectance_Position(Reflectance_End());
        StatePtr = StatePtr->next[Input];
        Output = StatePtr->out;
        motorState(Output);
    }

    count++;
    if(count == 10)
        count = 0;
}

