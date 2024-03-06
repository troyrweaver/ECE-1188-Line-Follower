#include "msp.h"
#include "Reflectance.h"
#include "Clock.h"
#include "Motor.h"
#include "LaunchPad.h"
#include "SysTickInts.h"
#include "CortexM.h"
#include "BumpInt.h"

void motorState(uint8_t state);
void SysTick_Handler(void);
//void bump(uint8_t);

struct State {
  uint8_t out;                  //2-bit output
  uint8_t debugRGB;             //3-bit output to Launchpad RGB
  uint8_t debugLED;             //1-bit
  const struct State *next[6];  // Next if 2-bit input is 0-3
};
typedef const struct State State_t;

#define Center    &fsm[0]
#define Left      &fsm[1]
#define Right     &fsm[2]
#define LookF     &fsm[3]
#define LookB     &fsm[4]
#define LookR     &fsm[5]
#define LookL     &fsm[6]
#define Lost      &fsm[7]
#define FastL     &fsm[8]
#define FastR     &fsm[9]

//Can map RGB LEDS to track states
// dark --- 0
// red R-- 0x01
// blue --B 0x04
// green -G- 0x02
// yellow RG- 0x03
// sky blue -GB 0x06
// white RGB 0x07
// pink R-B 0x05

State_t fsm[10]={ //THE ORDER OF THESE COULD MAYBE BE CHANGED?

    {0x1, 0x07, 0x00, {Center, Left, Right, LookF, FastL, FastR }},     //Center;  White;       Off        same order as states defined above^^^^^
    {0x2, 0x01, 0x00, {Center, Left, Right, LookF, FastL, FastR }},     //Left;    Red;         Off
    {0x3, 0x04, 0x00, {Center, Left, Right, LookF, FastL, FastR }},     //Right;   Blue;        Off
    {0x4, 0x01, 0x01, {Center, Left, Right, LookB, FastL, FastR }},     //LookF;   Red;         On
    {0x5, 0x02, 0x01, {Center, Left, Right, LookL, FastL, FastR }},     //LookB;   Green;       On
    {0x6, 0x03, 0x01, {Center, Left, Right, LookR, FastL, FastR }},     //LookR;   Yellow;      On
    {0x7, 0x04, 0x01, {Center, Left, Right,  Lost, FastL, FastR }},     //LookL;   Blue;        On
    {0x8, 0x00, 0x01, {  Lost, Lost,  Lost,  Lost,  Lost,  Lost }},     //Lost;    Dark;        On
    {0x9, 0x05, 0x00, {Center, Left, Right, LookF, FastL, FastR }},     //FastL;   Pink;        Off
    {0xA, 0x06, 0x00, {Center, Left, Right, LookF, FastL, FastR }}      //FastR;   Sky Blue;    Off
};

State_t *StatePtr;  // pointer to the current state
uint8_t Input;
uint8_t Output;
volatile uint8_t data;

int main(void){
  Clock_Init48MHz();
  Motor_Init();
  Reflectance_Init();
  SysTick_Init(48000, 2);
  LaunchPad_Init();
//  BumpInt_Init(&bump);

  StatePtr = Center;

  EnableInterrupts();

  while(1)
      WaitForInterrupt();
}

void motorState(uint8_t state) {
    switch(state){
        case 0x1:
            Motor_Forward(3000, 3000);      //center
            break;
        case 0x2:
            Motor_Left(0, 2000);            //left
            break;
        case 0x3:
            Motor_Right(2000, 0);           //right
            break;
        case 0x4:
            Motor_Forward(3000, 3000);      //look forward
            Clock_Delay1ms(50);
            break;
        case 0x5:
            Motor_Backward(3000, 3000);     //look backward
            Clock_Delay1ms(100);
            break;
        case 0x6:
            Motor_Left(3000, 3000);         //look left
            Clock_Delay1ms(150);
            break;
        case 0x7:
            Motor_Right(3000, 3000);        //look right
            Clock_Delay1ms(300);
        case 0x8:
            //Clock_Delay1ms(150);
            Motor_Stop();                   //lost FUCKED UP, MAYBE WAIT A LITTLE BEFORE SHUTTING OFF MOTOR?
            break;
        case 0x9:
            Motor_Left(3000, 3000);         //fast turn left
            break;
        case 0xA:
            Motor_Right(3000, 3000);        //fast turn right
            break;
        default:
            break;
    }
}

//CANT GET THIS TO WORK, I AM CLOSE!!! BUMP SENSORS HAVE TO WORK AND STOP MOTOR
/*void bump(uint8_t bump){ FUCKED UP!!!
    switch(bump){
    case BIT0:
    case BIT2:
    case BIT3:
    case BIT5:
    case BIT6:
    case BIT7:
        Motor_Stop();
        break;
    default:
        break;
    }
}*/

void SysTick_Handler(void){
    volatile static uint8_t count = 0;

    if(count == 0)
        Reflectance_Start();

    else if(count == 1) {
                data = Reflectance_End();
                Input = Reflectance_Position(data);
                LaunchPad_Output(StatePtr->debugRGB);
                LaunchPad_LED(StatePtr->debugLED);
                StatePtr = StatePtr->next[Input];
                Output = StatePtr->out;
                motorState(Output);
    }

    count++;
    if(count == 10)
        count = 0;
}

