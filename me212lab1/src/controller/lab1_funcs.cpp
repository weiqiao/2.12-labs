// Peter KT Yu        - peterkty _ mit _ edu, Sept 2016
// Ryan Fish          - fishr _mit_ edu,      Sept 2016

#include "lab1_funcs.h"
#include "helper.h"

int openLoopController()
{
    // TODO:  Replace this with a calculated open loop motor command
    // e.g.  m1Command = desiredMV1 * some_factor;
    // remember the PWM limit is [-400,400]!

    // Fix this line
    m1Command = 400/maxMV*desiredMV1;

    return m1Command;
}

void storeOldVals()
{    
    prevTime = currentTime;

    // TODO: Store the old encoder values so you can take their derivative
    //   relevant variables:
    //   encoder1count
    //   encoder1countPrev
    encoder1countPrev = encoder1count;
    // TODO: Store the old error values so you can take their derivative
    //   error1
    //   error1Prev
    error1Prev = error1;
}

float motor1_velocity()
{

    // TODO: Use the encoder count values and dt to determine motor1 velocity
    // relevant variables:
    //   encoder1count
    //   encoder1countPrev
    //   dt
    float stepVelocity = (encoder1count - encoder1countPrev)/dt;
    // Fix this line and uncomment it
    mV1 = stepVelocity * enc2robotvel;

    return mV1;
}

float proportional_control()
{
    // TODO: Use the desired velocity and actual velocity to calculate the velocity error
    // relevant variables:
    //   mV1
    //   desiredMV1
    //   error1
    error1=desiredMV1-mV1;
    // Fix this line and uncomment it

    proportionalCommand1 = error1 * Kpv1;
    return proportionalCommand1;
}

int closedLoopController()
{
    // TODO: Sum your various controls here 
    // e.g. proportional_control() + derivative_control() + integral_control()
    m1Command = (proportional_control()+derivative_control()+integral_control())*400/maxMV;
    return m1Command;
}

float derivative_control()
{
    // TODO: Use the old velocity value and the new one to determine the derivative of the error term
    // relevant variables:
    //  error1
    //  error1Prev
    //  dt
    //  Kdv1

    // Fix this line and uncomment it
    derivativeCommand1 = (error1-error1Prev)/dt*Kdv1;
    return derivativeCommand1;
}

float integral_control()
{
    //TODO: Use the previously integrated error and the current error
    //to calculate the current error integral
    //relevant variables:
    //  error1
    //  integratedError1
    //  dt
    //  Kiv1

    // Fix this line and uncomment it
    integratedError1 = error1*dt+integratedError1;
    integralCommand1 = integratedError1 *Kiv1;
    return integralCommand1;
}
