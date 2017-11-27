// Zack Bright        - zbright _ mit _ edu,  Sept 2015
// Daniel J. Gonzalez - dgonz _ mit _ edu,    Sept 2015
// Peter KT Yu        - peterkty _ mit _ edu, Sept 2016
// Ryan Fish          - fishr _mit_ edu,      Sept 2016


#include "DualMC33926MotorShield.h"

#ifndef HELPER_H
#define HELPER_H

// PI has been defined in arduino

const float wheelRadius = 0.037;  // In meters

// There are two type of motors, Model 26 and Model 53.
// Model 26 is 2000 encoder counts per output revolution (1 turn of wheel), Model 53 is 1000 counts per output revolution
// Model 26 also has a 1:26 gearing ratio, while Model 53 has a 1:53 gearing ratio

// If your motor has a 53 in the part number sticker, comment out the next line
//#define GEARBOX_26

#ifdef GEARBOX_26
  const float enc2rev = 2000.0;  // these motors have 2000 counts per rev
  const float gearing = 26.0;
#else  // otherwise gearbox 53 
  const float enc2rev = 1000.0;  // these motors have 1000 counts per rev
  const float gearing = 53.0;
#endif
const float enc2robotvel = (1.0 / enc2rev) * 2 * PI * wheelRadius; 

const float maxMV = 22.2 * 26.94 / gearing * wheelRadius;  // max wheelspeed, note cancelled factors of PI and 1/(PI)


// The variables with extern modifiers are defined in helper.cpp file
// These hold the current encoder count.
extern signed long encoder1count;  //current total encoder ticks since arduino powered on wraps around at 
extern signed long encoder1countPrev;

//Timing variables
extern unsigned long currentTime;
extern unsigned long prevTime;
extern unsigned long startTime;          // Time the arduino finished setup and started looping
extern unsigned long freqTime;
extern unsigned long currentSerialTime;  // current serial message time
extern unsigned long prevSerialTime;     // last time serial messages were dispatched
extern float         dt;                 // time between control loops

//Velocity Control Gains, Motor 1
extern const float Kpv1;        //Proportional gain, velocity, motor 1
extern const float Kdv1;        //Derivative gain
extern const float Kiv1;        //Integral gain

//Motor 1 Errors and Command Components
extern float error1;            //Current Motor 1 velocity error
extern float error1Prev;        //previous velocity error
extern float integratedError1;  //previously integrated error
extern int   m1Command;         //Motor 1 voltage command, bounded in -400 to 400
extern float proportionalCommand1;  //proportional component of motor command
extern float derivativeCommand1;    //derivative component
extern float integralCommand1;      //integral component
extern int   currentCommand1;

//Motor Velocities
extern float mV1;          // Estimated motor 1 velocity. (meters/second)
extern float desiredMV1;   // (meters/second)
extern float avgMV1;

//Sine Wave
extern float sinFreq;  // (Hz)

//motor shield object
extern DualMC33926MotorShield md;

//Staff constants
const int   FREQ = 2000;
const float PERIOD = 0.0005;
const float PERIOD_MICROS = (PERIOD * 1000000.0);
const int   SERIAL_FREQ = 100;  //Should be at least 10 times greater than sin wave frequency
const float SERIAL_PERIOD = 0.01;
const float SERIAL_PERIOD_MICROS = 10000.0;

//Staff functions
void stopIfFault(void);
void DoSerialSend(void);

#endif //HELPER_H
