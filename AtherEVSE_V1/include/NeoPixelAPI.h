#include "Adafruit_NeoPixel.h"

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        27 //13 // for esp32 wrover kit

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS  16 // 8 //16 //8 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.

void initNeoPixels(void);
void ClearNeoPixels(void);


void colorFill(uint32_t c, uint16_t wait, uint8_t direction) ;
void colorFillDual(uint32_t c1,uint32_t c2, uint16_t wait, uint8_t direction) ;

void colorChase(uint32_t c, uint16_t wait, uint8_t direction);

void colorChaseNum(uint32_t c, uint16_t wait, uint8_t NumPixels, uint8_t direction);

void colorRing(uint32_t c);


void colorShiner(uint32_t c, uint16_t wait);

void colorBeater(uint32_t c, uint16_t wait, uint16_t pauseCntr);
void colorBeater2Beats(uint32_t c, uint16_t wait);
void colorBeaterDualShades(uint32_t c1,uint32_t c2, uint16_t wait);

void mainNeoPixel(uint8_t animation);
uint32_t Wheel(byte WheelPos) ;
void rainbow_Blocking(uint8_t wait) ;
// void rainbowCycleBlocking(uint8_t wait) ;
void rainbowCycleBlocking(uint8_t wait, uint8_t _direction, uint8_t _numcycles) ;
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait) ;
void theaterChase(uint32_t color, int wait) ;
