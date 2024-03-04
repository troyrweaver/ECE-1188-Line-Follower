#include <stdint.h>
#include "msp.h"
#include "../inc/CortexM.h"
#include "../inc/PWM.h"

// ------------Motor_Init------------
// Initialize GPIO pins for output, which will be
// used to control the direction of the motors and
// to enable or disable the drivers.
// The motors are initially stopped, the drivers
// are initially powered down, and the PWM speed
// control is uninitialized.
// Input: none
// Output: none
void Motor_Init(void){
    // P5.4 - left motor, P5.5 - right motor
    P5->SEL0 &= ~0x30;
    P5->SEL1 &= ~0x30;
    P5->DIR |= 0x30;
    P5->OUT &= ~0x30;

    //sleep pins P3.6, P3.7
    P3->SEL0 &= ~0xC0;
    P3->SEL1 &= ~0xC0;
    P3->DIR |= 0xC0;
    P3->OUT &= ~0xC0; //sleep motors

    //PWM pins P2.6, P2.7
    P2->SEL0 &= ~0xC0;
    P2->SEL1 &= ~0xC0;
    P2->DIR |= 0xC0;
    P2->OUT &= ~0xC0;

    PWM_Init12(7500,0,0);
}

// ------------Motor_Stop------------
// Stop the motors, power down the drivers, and
// set the PWM speed control to 0% duty cycle.
// Input: none
// Output: none
void Motor_Stop(void){

      P2->OUT &= ~0xC0;//off
      P3->OUT &= ~0xC0;//low current sleep mode

}

// ------------Motor_Forward------------
// Drive the robot forward by running left and
// right wheels forward with the given duty
// cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Forward(uint16_t leftDuty, uint16_t rightDuty){

        P5->OUT &= ~0x30;
        P3->OUT |= 0xC0;
        PWM_Duty1(rightDuty);
        PWM_Duty2(leftDuty);
}

// ------------Motor_Right------------
// Turn the robot to the right by running the
// left wheel forward and the right wheel
// backward with the given duty cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Right(uint16_t leftDuty, uint16_t rightDuty){

    P3 -> OUT |= 0xC0;//nSleep = 1
    P5 -> OUT &= ~0x10;//P5.4 PH = 0
    P5 -> OUT |= 0x20; //P5.5 PH = 1
    PWM_Duty2(leftDuty);
    PWM_Duty1(rightDuty);

}

// ------------Motor_Left------------
// Turn the robot to the left by running the
// left wheel backward and the right wheel
// forward with the given duty cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Left(uint16_t leftDuty, uint16_t rightDuty){

    P3 -> OUT |= 0xC0;//nSleep = 1
    P5 -> OUT |= 0x10;//P5.4 PH = 1
    P5 -> OUT &= ~0x20;//P5.5 PH = 0
    PWM_Duty2(leftDuty);
    PWM_Duty1(rightDuty);

}

void Motor_Backward(uint16_t leftDuty, uint16_t rightDuty){

   P3->OUT |= 0xC0;//nSleep = 1
   P5->OUT |= 0x30;//PH = 1
   PWM_Duty1(rightDuty);
   PWM_Duty2(leftDuty);

}


