#ifndef MOTOR_H
#define MOTOR_H

#include "main.h"
#include "timer.h"
#include "leds.h"
#include "modbus.h"

namespace Motor {

enum MotorState : uint8_t
{
    STOPPED,
    FORWARD,
    BACKWARD
};
void setMotorState(MotorState state);
MotorState getMotorState();

enum MotorStateMachine : uint8_t {
    TURN_ON_FORWARD,
    WAIT_FOR_FORWARD,
    FORWARD_STATE,
    TURN_ON_BACKWARD,
    WAIT_FOR_BACKWARD,
    BACKWARD_STATE,
    TURN_OFF,
    WAIT_FOR_STOP,
    STOP_STATE,
    EMERGENCY_STATE
};
void setMotorStateMachine(MotorStateMachine state);
MotorStateMachine getMotorStateMachine();

void stateMachineHandler();

void powerOnTimeout();
void powerOffTimeout();

void init();
inline void powerOn(); // TVD solid state relay
inline void powerOff();
inline void forward();
inline void backward();
inline void stop();

bool isVoltageOn();

constexpr uint16_t POWER_ON_TIME = 250; //ms
constexpr uint16_t POWER_OFF_TIME = 100; //ms
constexpr uint32_t LED_TOGGLE_TIME = 100000; //cycles
} // namespace Motor

#endif // motor_H
