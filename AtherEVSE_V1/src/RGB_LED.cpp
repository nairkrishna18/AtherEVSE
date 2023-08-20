#include <Arduino.h>
#include "RGB_LED.h"


// Where you refer to the class you use the 'scope resolution operator' of a double colon::

// Default Constructor

RGB_LED::RGB_LED()
{
    
}

void RGB_LED::begin(int RLEDPin,  int GLEDPin, int BLEDPin)
{
    // Store the pin number within the object for future use....
    RGB_LED::_RLEDPin = RLEDPin;
    RGB_LED::_GLEDPin = GLEDPin;
    RGB_LED::_BLEDPin = BLEDPin;

    pinMode(_RLEDPin  , OUTPUT);
    pinMode(_GLEDPin  , OUTPUT);
    pinMode(_BLEDPin  , OUTPUT);
    log_v("Initialized RGB Pins To Output.....");
    
    Display(Blank); // Turn off all Leds....        
    log_v("Blank all ");
    delay(500);
}

void RGB_LED::Display(uint8_t shade)
{
    switch(shade)
    {
        case RGB_LED::Blank:
            digitalWrite(_RLEDPin, LedOffState);    // Turn LedOffState Red LED
            digitalWrite(_GLEDPin, LedOffState);    // Turn LedOffState Green LED
            digitalWrite(_BLEDPin, LedOffState);    // Turn LedOffState Blue LED
        break;

        case RGB_LED::Red:
            digitalWrite(_GLEDPin, LedOffState);    // Turn LedOffState Green LED
            digitalWrite(_BLEDPin, LedOffState);    // Turn LedOffState Blue LED
            digitalWrite(_RLEDPin, LedOnState);     // TurnLedOnState Red LED            
        break;
        
        case RGB_LED::Green:            
            digitalWrite(_BLEDPin, LedOffState);    // Turn LedOffState Blue LED
            digitalWrite(_RLEDPin, LedOffState);    // Turn LedOffState Red LED            
            digitalWrite(_GLEDPin,LedOnState);     // TurnLedOnState Green LED
        break;
        
        case RGB_LED::Blue:                        
            digitalWrite(_RLEDPin, LedOffState);    // Turn LedOffState Red LED            
            digitalWrite(_GLEDPin, LedOffState);    // Turn LedOffState Green LED
            digitalWrite(_BLEDPin,LedOnState);     // TurnLedOnState Blue LED
        break;

        case RGB_LED::Yellow:                                    
            digitalWrite(_BLEDPin, LedOffState);    // Turn LedOffState Blue LED
            digitalWrite(_RLEDPin,LedOnState);     // TurnLedOnState Red LED            
            digitalWrite(_GLEDPin,LedOnState);     // TurnLedOnState Green LED
        break;

        case RGB_LED::Magenta:                                    
            digitalWrite(_GLEDPin, LedOffState);    // Turn LedOffState Green LED
            digitalWrite(_RLEDPin,LedOnState);     // TurnLedOnState Red LED  
            digitalWrite(_BLEDPin,LedOnState);     // TurnLedOnState Blue LED                      
        break;

        case RGB_LED::Cyan:                                                
            digitalWrite(_RLEDPin, LedOffState);    // Turn LedOffState Red LED  
            digitalWrite(_GLEDPin,LedOnState);     // TurnLedOnState Green LED
            digitalWrite(_BLEDPin,LedOnState);     // TurnLedOnState Blue LED                      
        break;
        
        case RGB_LED::White:                                                
            digitalWrite(_RLEDPin,LedOnState);     // TurnLedOnState Red LED  
            digitalWrite(_GLEDPin,LedOnState);     // TurnLedOnState Green LED
            digitalWrite(_BLEDPin,LedOnState);     // TurnLedOnState Blue LED                      
        break;
        
        default:
            digitalWrite(_RLEDPin, LedOffState);    // Turn LedOffState Red LED
            digitalWrite(_GLEDPin, LedOffState);    // Turn LedOffState Green LED
            digitalWrite(_BLEDPin, LedOffState);    // Turn LedOffState Blue LED
        break;

    }
}

void RGB_LED::Flash(uint8_t shade1, uint16_t _delay1ms)
{
    static uint8_t _state1, prvState;
    static uint32_t _currentMillis;
    static uint32_t _previousMillis; 

    _currentMillis = millis();
    log_v("_currentMillis = ",_currentMillis);
    log_v("_state1        = ",_state1);
    switch(_state1)
    {
        case 0: // turn off all Led.....
            Display(Blank); 
            prvState = _state1;   
            _state1 = 2;               
            log_v("_state1        = Blank State ");   
              
        break;

        case 1: // Turn on shade mentioned.........
            Display(shade1);
            prvState = _state1;
            _state1 = 2;
            log_v("_state1        = Shade State ");  
            
        break;

        case 2: // Wait here untill delay expired.....
            log_v("_state1        = Delay Wait ");
            if((_currentMillis - _previousMillis) >= _delay1ms)
            {
                log_v("_state1        = Delay Completed ");
                _previousMillis = _currentMillis;
                if(prvState == 0) _state1 = 1;
                else _state1 = 0;                
                log_v("prevmillis     =  ", _previousMillis);
            }
        break;

        default:
             Display(Blank); 
             _state1 = 0;
             log_e("Some Problem.....");
        break;
    }


}

void RGB_LED::Flash(uint8_t shade1,uint8_t shade2, uint16_t _delay1ms)
{
    static uint8_t _state1, prvState;
    static uint32_t _currentMillis;
    static uint32_t _previousMillis; 

    _currentMillis = millis();
    log_v("_currentMillis = ",_currentMillis);
    log_v("_state1        = ",_state1);
    switch(_state1)
    {
        case 0: // turn off all Led.....
            Display(shade1); 
            prvState = _state1;   
            _state1 = 2;               
            log_v("_state1        = Blank State ");   
              
        break;

        case 1: // Turn on shade mentioned.........
            Display(shade2);
            prvState = _state1;
            _state1 = 2;
            log_v("_state1        = Shade State ");  
            
        break;

        case 2: // Wait here untill delay expired.....
            log_v("_state1          = Delay Wait ");
            // log_d("_currentMillis   =  ", _currentMillis);
            // log_d("prevmillis       =  ", _previousMillis);
            // log_d("Delta Millis     =  ", (_currentMillis - _previousMillis));
            if((_currentMillis - _previousMillis) >= _delay1ms)
            {
                log_v("_state1        = Delay Completed ");
                _previousMillis = _currentMillis;
                if(prvState == 0) _state1 = 1;
                else _state1 = 0;                
                log_v("prevmillis     =  ", _previousMillis);
            }
        break;

        default:
             Display(Blank); 
             _state1 = 0;
             log_e("Some Problem.....");
        break;
    }


}

void RGB_LED::Flash(uint8_t shade1, uint16_t _delay1ms, uint8_t shade2, uint16_t _delay2ms)
{
    static uint8_t _state1, prvState;
    static uint32_t _currentMillis;
    static uint32_t _previousMillis; 
    static uint16_t _delay;

    _currentMillis = millis();
    log_v("_currentMillis = ",_currentMillis);
    log_v("_state1        = ",_state1);
    switch(_state1)
    {
        case 0: // turn off all Led.....
            Display(shade1); 
            prvState    = _state1;  
            _delay      = _delay1ms; 
            _state1     = 2;               
            log_v("_state1        = Blank State ");   
              
        break;

        case 1: // Turn on shade mentioned.........
            Display(shade2);
            prvState    = _state1;
            _delay      = _delay2ms; 
            _state1     = 2;
            log_v("_state1        = Shade State ");  
            
        break;

        case 2: // Wait here untill delay expired.....
            log_v("_state1        = Delay Wait ");
            if((_currentMillis - _previousMillis) >= _delay)
            {
                log_v("_state1        = Delay Completed ");
                _previousMillis = _currentMillis;
                if(prvState == 0) _state1 = 1;
                else _state1 = 0;                
                log_v("prevmillis     =  ", _previousMillis);
            }
        break;

        default:
             Display(Blank); 
             _state1 = 0;
             log_e("Some Problem.....");
        break;
    }
}





