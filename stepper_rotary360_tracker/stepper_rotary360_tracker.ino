/** 
 *  Stepper motor tracking a 360 rotary dial
 *  
 *  
 *  Author: Aaron S. Crandall <crandall@gonzaga.edu>
 *  Copyright: 2021
 *  License: GPL 3.0
 *  
 *  Stepper motor: 28BYJ-48
 *  Rotary dial: 360 degrees, 2 pin
 */

#include <Stepper.h>

#define MOTOR_STEPS_PER_RPM 2038 // the number of steps in one revolution of your motor (28BYJ-48)
Stepper stepper(MOTOR_STEPS_PER_RPM, 8, 10, 9, 11);
 
#define outputA 2
#define outputB 3
#define ROTARY_BUTTON_PIN 7
 
int counter = 0; 
int aState;
int aLastState;

int pinPressCount = 0;

#define ROTARY_STEPS_PER_360 40

float ROTARY_TO_MOTOR_STEPS = MOTOR_STEPS_PER_RPM / (float)ROTARY_STEPS_PER_360;
volatile float offset_motor_steps = 0;

// ** Interrupt handler (pin 2) for rotary dial state change
void handle_rotary_change() {
  if( digitalRead(ROTARY_BUTTON_PIN) == LOW ) {
    return;   // Don't do anything if button is currently pressed
  }
  
  aState = digitalRead(outputA); // Reads the "current" state of the outputA

  // If the previous and the current state of the outputA are different, that means a Pulse has occured
  if (aState != aLastState){     
    // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
    if (digitalRead(outputB) != aState) { 
      counter ++;

      offset_motor_steps += ROTARY_TO_MOTOR_STEPS;
    } else {
      counter --;

      offset_motor_steps -= ROTARY_TO_MOTOR_STEPS;
    }
    Serial.print("Position: ");
    Serial.println(counter);
  }

  aLastState = aState; // Updates the previous state of the outputA with the current state
}

// ** ************************************************************************************************************ 
void setup() { 
  Serial.begin (115200);

  pinMode (outputA,INPUT);
  pinMode (outputB,INPUT);
  pinMode(ROTARY_BUTTON_PIN, INPUT);
  
  // Reads the initial state of the outputA
  aLastState = digitalRead(outputA);   

  // With the ULN2003 motor controller, badness happend at 20 rpms
  //   19 seemed fine, but 15 is much safer.
  stepper.setSpeed(15);   // rpms

  // Assign interrupt handler for pin 2 - rotary dial state change
  attachInterrupt(digitalPinToInterrupt(outputA), handle_rotary_change, CHANGE);
} 


// ** ************************************************************************************************************
void loop() { 
  if( digitalRead(ROTARY_BUTTON_PIN) == LOW ) {
    counter = 0;
    offset_motor_steps = 0;
    pinPressCount++;
  }


  if( (offset_motor_steps >= 1 or offset_motor_steps <= -1) ) {
    Serial.print("Pin cnt: ");
    Serial.print(pinPressCount);
    Serial.print("  motor offset steps: ");
    Serial.println(offset_motor_steps);

    if( offset_motor_steps > 0 ) {
      stepper.step(-1);
      offset_motor_steps -= 1;
    }
    else
    {
      stepper.step(1);
      offset_motor_steps += 1;
    }
   }
   

 }
