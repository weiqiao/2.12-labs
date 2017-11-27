// Zack Bright        - zbright _ mit _ edu,  Sept 2015
// Daniel J. Gonzalez - dgonz _ mit _ edu,    Sept 2015
// Peter KT Yu        - peterkty _ mit _ edu, Sept 2016
// Ryan Fish          - fishr _mit_ edu,      Sept 2016

// The SPI bus library, the encoder buffer library, and the motor shield library (not our focus in this lab)
#include "SPI.h"
#include "LS7366.h"
#include "DualMC33926MotorShield.h"

// TA's helper functions and variables that we recommend using.
// You don't need to define any variables by yourself.
#include "helper.h"

// The function declaration for the main control functions
#include "lab1_funcs.h"

// Motor shield object
DualMC33926MotorShield md;

//==============Setup Function (not our focus in this lab)==============

void setup() {
    Serial.begin(115200);      // Serial com for data output

    initEncoders();       Serial.println("Encoders Initialized...");  
    clearEncoderCount();  Serial.println("Encoders Cleared...");
    md.init();            Serial.println("Motor Driver Initialized...");
    currentTime = micros();
    prevTime = micros();
}

//=========================Main loop Function===========================

void loop() {
    /*
    ////////////////////////////////////////////////
    // One iteration of control loop
    ////////////////////////////////////////////////
    */

    dt = PERIOD;   // Timed control loop period;
    currentTime = micros();

    // This gives us a delay of 1 sec; currentTime is in microsecond.
    // This makes looking at the controller plot easier.
    if (currentTime <= 1000000.0){ 
        desiredMV1 = .1;
        DoSerialSend();
        return;
    }

    if(currentTime - prevTime >= PERIOD_MICROS){
        
        ////////////////////////////
        // START STUDENT CODE
        ////////////////////////////  

        // 0. Setting options
        
        // 0.1 Choose motor 1 desired velocity by setting useSineInput
        boolean useSineInput = true;       // false: use STEP INPUT, true: use SINE WAVE INPUT
        // 0.2 Choose controller type
        boolean useClosedLoopControl = true;  // false: open loop control, true: closed loop control
        
        // 1. Calculate the current desired velocity
        if ( !useSineInput ){
            desiredMV1 = 0.2 * maxMV;             //STEP INPUT
        }
        else {
            float sinWave = sin(2.0 * PI * sinFreq * currentTime / 1000000.0);
            desiredMV1 = 0.2 * maxMV * sinWave;   //SINE INPUT
        }
    
        // 2. Retrieve current encoder counters and estimate velocity
        
        // 2.1 TODO: Verify that the encoder directions correspond to your encoders
        //     To make positive encoder values match positive PWM motor commands, 
        //     a sign flip may be required
        encoder1count = 1 * readEncoder(1); 
        
        // 2.2 TODO: Calculate the current motor speed
        motor1_velocity();

        // 3. Motor 1 velocity control
        // 3.1 TODO: Change your controllers to generate the full command value
        if (!useClosedLoopControl){
            m1Command = openLoopController();
        }
        else{
            m1Command = closedLoopController();
        }
        // 3.2 Bound the command inside [-400, 400] and then send the command to motor drive.
        md.setM1Speed( constrain(m1Command, -400, 400) );

        // 4. Save encoder counts and for next loop iteration
        storeOldVals();
        
        //////////////////////
        // END STUDENT CODE
        //////////////////////

	avgMV1 = avgMV1 * 0.3 + mV1 * 0.7;

        // A helper function that will stop our motors if something goes wrong
        stopIfFault();
    } 
    
    // Send current values to computer
    DoSerialSend();
}

