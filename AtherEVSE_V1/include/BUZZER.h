
#pragma once
#include<Arduino.h>

// Library for Ultrasonic sensor JSN-SR04T

class BUZZER
{
    public: // These are the "functions" exposed to the consumer of this library

    
    typedef enum eBuzzerState
    {
        BuzOffState,
        BuzOnState,        
        BuzBeepState,
        BuzNumStates,
    }eBuzzerState;

    // Constructor (we could allocate the pins here)
    BUZZER();
    
    void begin(int BUZZERPin);
    void Buzz(uint8_t sound); // Turn On or Off
    void Beep(uint16_t _ToggleDelayms); // Beep Continuosly with Fixed delay of On-Off
    void Beep(uint16_t _OnDelayms, uint16_t _OffDelayms);// Beep Continuosly with Differenct delay of On-Off
    void MainBuzzerStates(int8_t BuzStates);

    // void Beep(uint8_t sound, uint16_t _delay1ms);
    // void Beep(uint8_t sound1, uint8_t sound2 , uint16_t _delay1ms);
    // void Beep(uint8_t sound1, uint16_t _delay1ms ,uint8_t sound2, uint16_t _delay2ms);
    
    private: // Only visible within the class
    // Keep a record of which pins we are using
    
    typedef enum eSound
    {
        SoundOn,        
        SoundOff,         
        numSounds,
    }eSound;
    
    int _BUZZERPin;
    // Keep a record of which pins we are using
    
    

    protected: // Not exposed publically, Can be use with in this class and Inherited class

};

enum
{
    BUZSTATE_UKNOWN,
    BUZSTATE_OFF,
    BUZSTATE_RFID_DENIED,
    BUZSTATE_RFID_AUTHORIZED,
    
    BUZSTATE_WAIT,

};