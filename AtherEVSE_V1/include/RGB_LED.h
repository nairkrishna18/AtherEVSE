

#include<Arduino.h>

// Library for Ultrasonic sensor JSN-SR04T

class RGB_LED
{
    public: // These are the "functions" exposed to the consumer of this library

    typedef enum eShade
    {
        Blank, 
        Red,
        Green,
        Blue,
        Yellow, //Red + Green
        Magenta,//Red + Blue
        Cyan,   //Green + Blue
        White,  //Red + Green + Blue
        numShades,
    }eShade;
    
    

    // Constructor (we could allocate the pins here)
    RGB_LED();
    
    void begin(int RLEDPin,  int GLEDPin, int BLEDPin);
    void Display(uint8_t shade);

    void Flash(uint8_t shade1, uint16_t _delay1ms);
    void Flash(uint8_t shade1, uint8_t shade2 , uint16_t _delay1ms);
    void Flash(uint8_t shade1, uint16_t _delay1ms ,uint8_t shade2, uint16_t _delay2ms);
    
    private: // Only visible within the class
    // Keep a record of which pins we are using
    int _RLEDPin;
    int _GLEDPin;
    int _BLEDPin;

    typedef enum eLedState
    {
        LedOnState,
        LedOffState,
        Blink,
        numStates,
    }eLedState;

    // Keep a record of which pins we are using
    
    

    protected: // Not exposed publically, Can be use with in this class and Inherited class

};