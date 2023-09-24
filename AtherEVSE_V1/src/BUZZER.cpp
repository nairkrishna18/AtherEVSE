#include <Arduino.h>
#include "BUZZER.h"
// #include "Debug.h" // only used in arduino nano project.....

// Where you refer to the class you use the 'scope resolution operator' of a double colon::

// Default Constructor

BUZZER::BUZZER()
{
    
}

void BUZZER::begin(int BUZZERPin)
{
    // Store the pin number within the object for future use....
    BUZZER::_BUZZERPin = BUZZERPin;
    pinMode(_BUZZERPin  , OUTPUT);
    Buzz(BuzOffState); // Turn off all Leds....            
    log_i("Initialized BUZZER Pin To Output....." );
    
}

void BUZZER::Buzz(uint8_t sound)
{
    switch(sound)
    {
        case BUZZER::BuzOnState:
            digitalWrite(_BUZZERPin, SoundOn);    // Turn On Buzzer
        break;

        case BUZZER::BuzOffState:
            digitalWrite(_BUZZERPin, SoundOff);    // Turn Off Buzzer
        break;
        
        default:
            digitalWrite(_BUZZERPin, SoundOff);    // Turn Off Buzzer
        break;

    }
}

void BUZZER::Beep(uint16_t _ToggleDelayms)
{
    static uint8_t _state1, prvState;
    static uint32_t _currentMillis;
    static uint32_t _previousMillis;    

    _currentMillis = millis();
    switch(_state1)
    {
        case 0: // turn off Buzzer
            Buzz(BuzOffState); 
            prvState = _state1;   
            _state1 = 2;          
        break;

        case 1: // Turn on Buzzer
            Buzz(BuzOnState);
            prvState = _state1;
            _state1 = 2;
        break;

        case 2: // Wait here untill delay expired.....
            if((_currentMillis - _previousMillis) >= _ToggleDelayms)
            {
                _previousMillis = _currentMillis;
                if(prvState == 0) _state1 = 1;
                else _state1 = 0;                
            
            }
        break;

        default:
             Buzz(BuzOffState); 
             _state1 = 0;
             log_e("Some Problem with Buzzer State.....");
        break;
    }
}

void BUZZER::Beep(uint16_t _OnDelayms, uint16_t _OffDelayms)
{
    static uint8_t _state1, prvState;
    static uint32_t _currentMillis;
    static uint32_t _previousMillis; 
    static uint16_t _delay;

    _currentMillis = millis();
    
    switch(_state1)
    {
        case 0: // turn off all Led.....
            Buzz(BuzOffState); 
            prvState    = _state1;  
            _delay      = _OffDelayms; 
            _state1     = 2;               
            
              
        break;

        case 1: // Turn on shade mentioned.........
            Buzz(BuzOnState);
            prvState    = _state1;
            _delay      = _OnDelayms; 
            _state1     = 2;            
            
        break;

        case 2: // Wait here untill delay expired.....
            
            if((_currentMillis - _previousMillis) >= _delay)
            {                
                _previousMillis = _currentMillis;
                if(prvState == 0) _state1 = 1;
                else _state1 = 0;   
            }
        break;

        default:
             Buzz(BuzOffState); 
             _state1 = 0;
             log_i("Some Problem with Buzzer States.....");
        break;
    }
}

void BUZZER::MainBuzzerStates(int8_t BuzStates)
{
    static int8_t prvBuzState;
    static uint16_t __gi6_WaitDelay;
    switch(BuzStates)
    {

        case BUZSTATE_OFF:
            Buzz(BuzOffState); 
        break;

        case BUZSTATE_RFID_DENIED:
            Buzz(BuzOnState);
            // prvBuzState = BuzStates;
            // BuzStates = BUZSTATE_WAIT;
            // __gi6_WaitDelay = 1000;
        break;
        
        case BUZSTATE_RFID_AUTHORIZED:
            Beep(250);
            // prvBuzState = BuzStates;
            // BuzStates = BUZSTATE_WAIT;
            // __gi6_WaitDelay = 1000;            
        break;

        case BUZSTATE_WAIT:
            log_i("|---------Start Of Delay = %d---------|",__gi6_WaitDelay );
            vTaskDelay(pdMS_TO_TICKS(__gi6_WaitDelay)); // 1 sec delay for meter polling.......
            BuzStates = BUZSTATE_OFF;      
            log_i("|-------------End Of Delay-------------|");
              
        break;


        default:

        break;
    }


}

