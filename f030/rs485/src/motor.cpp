#include "motor.h"

namespace Motor {
MotorState currentMotorState = MotorState::STOPPED;
MotorStateMachine currentMotorStateMachine = MotorStateMachine::STOP_STATE;

bool TimeoutPowerOn = false;
bool TimeoutPowerOff = false;

bool mustBeVoltage = false;
// one time flags
bool OneTimeForward = false;
bool OneTimeBackward = false;
bool OneTimeStop = false;

// led toggle delay counter
uint32_t ledToggleCounter = 0;
} // namespace Motor

void Motor::setMotorState(MotorState state) { currentMotorState = state; }
Motor::MotorState Motor::getMotorState() { return currentMotorState; }

void Motor::setMotorStateMachine(MotorStateMachine state) { currentMotorStateMachine = state; }
Motor::MotorStateMachine Motor::getMotorStateMachine() { return currentMotorStateMachine; }

void Motor::powerOnTimeout() { TimeoutPowerOn = true; }
void Motor::powerOffTimeout() {
    TimeoutPowerOff = true;
    if (currentMotorStateMachine == MotorStateMachine::WAIT_FOR_STOP) {
        currentMotorStateMachine = MotorStateMachine::STOP_STATE;
    }
    powerOff();
}

void Motor::init() {
    // timers init
    Timer::setPowerOnTimeout(powerOnTimeout);
    Timer::setPowerOffTimeout(powerOffTimeout);

    // outputs  PA0 - TVD, PA1 - KV2, PA2 - KV1 push pull high speed
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~(GPIO_MODER_MODER0_1 | GPIO_MODER_MODER1_1 | GPIO_MODER_MODER2_1);
    GPIOA->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0);
    GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDR_OSPEEDR1 | GPIO_OSPEEDR_OSPEEDR2);

    // inputs PB0 - backward, PB1 - forward,    
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR1_0);
    GPIOB->PUPDR &= (GPIO_PUPDR_PUPDR0_1 | GPIO_PUPDR_PUPDR1_1); // 0:1 pull-up

    // input PA3 - 220V (0 => presence)
    GPIOA->MODER &= ~(GPIO_MODER_MODER3);
}

void Motor::powerOn() { GPIOA->BSRR |= GPIO_BSRR_BS_0; }

void Motor::powerOff() { GPIOA->BSRR |= GPIO_BSRR_BR_0; }

void Motor::forward() {
    GPIOA->BSRR |= GPIO_BSRR_BS_1;
    GPIOA->BSRR |= GPIO_BSRR_BS_2;
    mustBeVoltage = true;
}
void Motor::backward() {
    GPIOA->BSRR |= GPIO_BSRR_BS_2;
    mustBeVoltage = true;
}

void Motor::stop() {
    GPIOA->BSRR |= GPIO_BSRR_BR_1;
    GPIOA->BSRR |= GPIO_BSRR_BR_2;
    mustBeVoltage = false;
}

bool Motor::isVoltageOn() { return !(GPIOA->IDR & GPIO_IDR_3); }

void Motor::stateMachineHandler() {
    // check inputs (reverse only through stop)
    if (Timer::getInForward() && !OneTimeForward) {
        currentMotorStateMachine = MotorStateMachine::TURN_ON_FORWARD;
        OneTimeForward = true;
        OneTimeStop = false;
    }
    if (Timer::getInBackward() && !OneTimeBackward) {
        currentMotorStateMachine = MotorStateMachine::TURN_ON_BACKWARD;
        OneTimeBackward = true;
        OneTimeStop = false;
    }
    if (!Timer::getInForward() && !Timer::getInBackward() && !OneTimeStop) {
        currentMotorStateMachine = MotorStateMachine::TURN_OFF;
        OneTimeStop = true;
        OneTimeForward = false;
        OneTimeBackward = false;
    }
    // TODO: check for RS485 signals
    if (Modbus::getIsPacketReceived()) {
        Modbus::parsePacket();
        if (Modbus::getIsWriteRequest()) {
            Modbus::setIsWriteRequest(false);
            if (Modbus::getControlReg() == Modbus::MotorForward_Data) {
                currentMotorStateMachine = MotorStateMachine::TURN_ON_FORWARD;
            } else if (Modbus::getControlReg() == Modbus::MotorBackward_Data) {
                currentMotorStateMachine = MotorStateMachine::TURN_ON_BACKWARD;
            } else if (Modbus::getControlReg() == Modbus::MotorStop_Data) {
                currentMotorStateMachine = MotorStateMachine::TURN_OFF;
            }
            Modbus::sendReply();
        } else if(Modbus::getIsReadRequest) {
            int x = 0;
            x++;
            Modbus::sendReply();
        }
    }
    //----------------------------------------------------------------------------------------
    //--------------------   MAIN STATE MACHINE    -------------------------------------------
    //----------------------------------------------------------------------------------------
    switch (currentMotorStateMachine) {
    case TURN_ON_FORWARD:
        forward();
        Timer::startPowerOnTimer(POWER_ON_TIME);
        currentMotorStateMachine = MotorStateMachine::WAIT_FOR_FORWARD;
        break;
    case WAIT_FOR_FORWARD:
        if (TimeoutPowerOn) {
            TimeoutPowerOn = false;
            powerOn();
            currentMotorStateMachine = MotorStateMachine::FORWARD_STATE;
            currentMotorState = MotorState::FORWARD;
            Modbus::setDirectionReg(Modbus::MotorRunForward_Data);
            Leds::forwardOn();
            Leds::backwardOff();
            Leds::emergencyOff();
            // TODO: simple delay
        }
        break;
    case FORWARD_STATE:
        // check for Voltage
        if (!isVoltageOn()) {
            currentMotorStateMachine = MotorStateMachine::EMERGENCY_STATE;
        }
        break;
    case TURN_ON_BACKWARD:
        backward();
        Timer::startPowerOnTimer(POWER_ON_TIME);
        currentMotorStateMachine = MotorStateMachine::WAIT_FOR_BACKWARD;
        break;
    case WAIT_FOR_BACKWARD:
        if (TimeoutPowerOn) {
            TimeoutPowerOn = false;
            powerOn();
            currentMotorStateMachine = MotorStateMachine::BACKWARD_STATE;
            currentMotorState = MotorState::BACKWARD;
            Modbus::setDirectionReg(Modbus::MotorRunBackward_Data);
            Leds::forwardOff();
            Leds::backwardOn();
            Leds::emergencyOff();
            // TODO: simple delay
        }
        break;
    case BACKWARD_STATE:
        // check for voltage
        if (!isVoltageOn()) {
            currentMotorStateMachine = MotorStateMachine::EMERGENCY_STATE;
        }
        break;
    case TURN_OFF:
        stop();
        Timer::startPowerOffTimer(POWER_OFF_TIME);
        currentMotorStateMachine = MotorStateMachine::WAIT_FOR_STOP;
        break;
    case WAIT_FOR_STOP:
        if (TimeoutPowerOff) {
            TimeoutPowerOff = false;
            powerOff();
            currentMotorStateMachine = MotorStateMachine::STOP_STATE;
            currentMotorState = MotorState::STOPPED;
            if (Modbus::getDirectionReg() == Modbus::MotorRunForward_Data) {
                Modbus::setDirectionReg(Modbus::MotorStopForward_Data);
            } else if (Modbus::getDirectionReg() == Modbus::MotorRunBackward_Data) {
                Modbus::setDirectionReg(Modbus::MotorStopBackward_Data);
            }
            Leds::forwardOff();
            Leds::backwardOff();
            Leds::emergencyOff();
            // TODO: simple delay
        }
        break;
    case STOP_STATE:
        if (isVoltageOn()) {
            currentMotorStateMachine = MotorStateMachine::TURN_OFF;
        }
        break;
    case EMERGENCY_STATE:
        // blink led
        if (ledToggleCounter >= LED_TOGGLE_TIME) {
            ledToggleCounter = 0;
            Leds::emergencyToggle();
        } else {
            ledToggleCounter++;
        }
        if (mustBeVoltage) {
            // check for voltage if it appear go to state;
        } else {
            if (!isVoltageOn()) {
                currentMotorStateMachine = MotorStateMachine::STOP_STATE;
            }
        }
        break;
    }
}
