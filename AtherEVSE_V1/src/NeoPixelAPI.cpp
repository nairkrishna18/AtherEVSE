#include "NeoPixelAPI.h"
#include "GlobalHeader.h"

const int interval = 200;
unsigned long previousMillis = 0;
static int8_t PixelCounter;
static int16_t PixelBrightnessCntr;
int           pixelQueue = 0;           // Pattern Pixel Queue
uint8_t gStatusNeoPixel = 0;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void initNeoPixels(void)
{
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(100); // set brightness levels globally.
  log_i("rainbowCycleBlocking() --> start");
  rainbowCycleBlocking(1, DIR_CW, 3);
  log_i("rainbowCycleBlocking() --> end");  
  ClearNeoPixels();
  gStatusNeoPixel = 1; // NeoPixel Initialized.
}


void mainNeoPixel(uint8_t animation)
{
  uint8_t nxtState;
  switch(animation)
  {
    case NP_BLANK:
      colorFill(pixels.Color(0, 0, 0), 100, DIR_CW); // No Colour........
    break;
   
    case NP_RFID_ERASING_EEPROM:              
      colorChaseNum(pixels.Color(0, 0, 255), 125,1,DIR_CW);
    break;


    /*************Colour Fill Counter Clocl Wise Directtion*********************************/
    
    case NP_ACF_RED_DIRCCW_10SEC:   // Used to Indiacte User to wait untill eeprom data erased           
      colorChaseNum(pixels.Color(255, 0, 0), 625,1,DIR_CCW);
    break;

    case NP_ACF_YELLOW_DIRCCW_10SEC:   // Used to Indiacte User to wait untill eeprom data erased           
      colorChaseNum(pixels.Color(255, 255, 0), 625,1,DIR_CCW);
    break;

    case NP_ACF_YELLOW_DIRCCW_2SEC: 
      colorChaseNum(pixels.Color(255, 255, 0), 125,1,DIR_CCW);
    break;

    case NP_ACF_GREEN_DIRCCW_2SEC:              
      colorChaseNum(pixels.Color(0, 255, 0), 125,1,DIR_CCW);
    break;

    case NP_RFID_WAIT_FOR_MASTER_CARD: // coor fill Violet CW_dir
      colorFill(pixels.Color(127, 0, 255), 100, DIR_CW); //            
    break;

    case NP_RFID_WAIT_FOR_USER_CANCEL:
       colorFill(pixels.Color(255, 255, 0), 625, DIR_CW); //                
    break;
   
    case NP_RFID_USER_CANCELLED:
      colorFill(pixels.Color(0, 255, 255), 125, DIR_CW); //   
    break;


    case NP_MFRC_READER_ERROR:
    
      colorChaseNum(pixels.Color(255,0,0),100,1,DIR_CW);
    break;

    case NP_TRY_NEW_ANIMATION:
      colorRing(pixels.Color(0, 255, 0));
      // colorShiner(pixels.Color(255, 0, 0), 8);
      // colorBeater2Beats(pixels.Color(255, 0, 0), 10);
      // colorBeaterDualShades(pixels.Color(255, 0, 0),pixels.Color(255, 0, 0), 10);
          // colorBeater(pixels.Color(255, 0, 0),10,10);
          // colorFillDual(pixels.Color(255, 0, 0),pixels.Color(0, 0, 255),125,DIR_VSPLIT_UP);

    break;
   

    /*************Colour Wipe States Ends Here********************/

    case NP_RESET_PIXELS:
       ClearNeoPixels();
       animation=nxtState;
    break;


    case 100:
      rainbow(20);
      // Serial.print("rainbow()");
    break;

    case 200:
      rainbowCycle(2);
      // Serial.print("rainbowCycle()");
    break;

    case 255:
        theaterChase(pixels.Color(0, 255, 0), 50); // Blue    
    break;

  }
  // 
  

}


// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait)  // Not working need testing more
{
  static uint16_t pixNum, PixBright;
  if (millis() - previousMillis >= wait) 
  {
    previousMillis = millis();
    for(int i = 0; i < pixels.numPixels(); i++) 
    {
      pixels.setPixelColor(i + pixelQueue, color); //  Set pixel's color (in RAM)
    }
    pixels.show();                             //  Update strip to match
    for(int i=0; i < pixels.numPixels(); i+=3) 
    {
      pixels.setPixelColor(i + pixelQueue, pixels.Color(0, 0, 0)); //  Set pixel's color (in RAM)
    }
    pixelQueue++;                             //  Advance current pixel
    if(pixelQueue >= 3)
    {
       pixelQueue = 0;                         //  Loop the pattern from the first LED
    }    
  } 
}


void rainbow_Blocking(uint8_t wait) 
{
  uint16_t i, j;

  for(j=0; j<256; j++) 
  {
    for(i=0; i<pixels.numPixels(); i++) 
    {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycleBlocking(uint8_t wait, uint8_t _direction, uint8_t _numcycles) 
{
  uint16_t i, j;

  for(j=0; j<256*_numcycles; j++) 
  { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) 
    {
      if(_direction == DIR_CW)
      {
        pixels.setPixelColor((pixels.numPixels()-1) - i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
      }
      else
      {
        pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
      }
    }
    pixels.show();
    delay(wait);
  }
}


void rainbow(uint8_t wait) 
{
  static uint16_t pixNum, PixBright;

  if (millis() - previousMillis >= wait) 
  {
    previousMillis = millis();
    PixBright++; // Update Pixel brightness Value 0 to 255
    // Serial.print("PixBright = ");
    // Serial.println(PixBright);
    
    if(PixBright <= 255) // Display Number of pixels to be animated 
    {
      for(pixNum=0; pixNum<pixels.numPixels(); pixNum++) // pixels number from 0 to numPixels....
      {
        // Serial.print("pixNum = ");
        // Serial.println(pixNum);
        pixels.setPixelColor(pixNum, Wheel((pixNum + PixBright) & 255));
      }
      pixels.show();      
    }   
    else
    {
      PixBright = 0; 
    }    
  }
}

void rainbowCycle(uint8_t wait) 
{
 
  static uint16_t pixNum, PixBright;

  if (millis() - previousMillis >= wait) 
  {
    previousMillis = millis();
    PixBright++; // Update Pixel brightness Value 0 to 255
    // Serial.print("PixBright = ");
    // Serial.println(PixBright);
    
    if(PixBright <= 255*5) // Display Number of pixels to be animated 
    {
      for(pixNum=0; pixNum<pixels.numPixels(); pixNum++) // pixels number from 0 to numPixels....
      {
        // Serial.print("pixNum = ");
        // Serial.println(pixNum);
        pixels.setPixelColor(pixNum, Wheel(((pixNum * 256 / pixels.numPixels()) + PixBright) & 255));
      }
      pixels.show();      
    }   
    else
    {
      PixBright = 0; 
    }    
  }
}

#if 0 // not implemented yet, may be later if required...........
//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) 
{
  for (int j=0; j < 256; j++) // 0 to 255
  {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) // 0,1,2
    {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) // 0 to Num Pixels
      {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) 
      {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
#endif


void ClearNeoPixels(void)
{
  PixelCounter = 0;
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.show(); // optional check.........
}



// Fill the dots one after the other with a color

/****colorFill******************************************************************
 * colorFill(uint32_t c, uint16_t wait, uint8_t direction):
 * P1 = uint32_t c --> pixels.Color(255, 128, 128) // Select the color to show.
 * P2 = uint16_t wait --> Animation delay in ms
 * P3 = uint8_t direction --> Direction of animation - DIR_CW, DIR_CCW
 * Each pixel will be filled until last piexel 
 * Erase entire Ring
 * Start the routine again....
********************************************************************************/
void colorFill(uint32_t c, uint16_t wait, uint8_t direction) 
{ 
    if (millis() - previousMillis >= wait) 
    {
      previousMillis = millis();      
      if(PixelCounter > pixels.numPixels()) // Display Number of pixels to be animated 
      {
        pixels.clear(); // Set all pixel colors to 'off'
        PixelCounter = -1; // so that all pixels are clear including 1st Pixel......
      }

      if(direction == DIR_CW) // Clock wise direction........
      {
        pixels.setPixelColor(PixelCounter-1, c);
        log_v("PixelCounterVal = %d",(PixelCounter-1));        
      }
      else
      if(direction == DIR_CCW) // Counter Clock Wise Direction.....
      {
        
        pixels.setPixelColor(16-(PixelCounter), c);    
        log_v("PixelCounterVal = %d",(16-PixelCounter));
      }     
      else
      if(direction == DIR_VSPLIT_UP) // Counter Clock Wise Direction.....
      {
        pixels.setPixelColor(PixelCounter-1, c);
        pixels.setPixelColor(16-(PixelCounter), c);    
        log_v("PixelCounterVal = %d",(16-PixelCounter));
      }     


      pixels.show();        
      PixelCounter++;    
    }  
}

/****colorFillDualShade******************************************************************
 * colorFillDual(uint32_t c1,uint32_t c2, uint16_t wait, uint8_t direction) 
 * P1 = uint32_t c1 --> pixels.Color(255, 128, 128) // Select the color 1 to show.
 * P2 = uint32_t c2 --> pixels.Color(255, 128, 128) // Select the color 2 to show.
 * P3 = uint16_t wait --> Animation delay in ms
 * P4 = uint8_t direction --> Direction of animation - DIR_CW, DIR_CCW, DIR_VSPLIT_UP, DIR_VSPLIT_DN
 * Each pixel will be filled until last piexel 
 * Erase entire Ring
 * Start the routine again....
********************************************************************************/
void colorFillDual(uint32_t c1,uint32_t c2, uint16_t wait, uint8_t direction) 
{ 
    static uint32_t __shade;
    static uint8_t  __mode;
    uint8_t numPixels = pixels.numPixels(); // 16


    // log_i("numPixels = %d",numPixels);        

    if(__mode == 0)
    {
      __shade = c1;
    }
    else
    {
      __shade = c2;
    }
    if (millis() - previousMillis >= wait) 
    {
      previousMillis = millis();      

      // if((direction == DIR_VSPLIT_UP)&&(PixelCounter > numPixels/2)) // Display Number of pixels to be animated 
      if(((direction == DIR_VSPLIT_UP)||(direction == DIR_VSPLIT_DN))&&(PixelCounter > numPixels/2)) // Display Number of pixels to be animated 
      {
        pixels.clear(); // Set all pixel colors to 'off'
        PixelCounter = 0; // so that all pixels are clear including 1st Pixel......   
        
        __mode++;
        if(__mode > 1)
        {
          __mode = 0;
        }      
       
      }
      else
      if((direction != DIR_VSPLIT_UP)&&(direction != DIR_VSPLIT_DN)&&(PixelCounter > numPixels)) // Display Number of pixels to be animated 
      {
        pixels.clear(); // Set all pixel colors to 'off'
        PixelCounter = -1; // so that all pixels are clear including 1st Pixel......     
        
        __mode++;
        if(__mode > 1)
        {
          __mode = 0;
        }   
      }
      if(direction == DIR_CW) // Clock wise direction........
      {
        pixels.setPixelColor(PixelCounter-1, __shade);
        log_v("PixelCounterVal = %d",(PixelCounter-1));        
      }
      else
      if(direction == DIR_CCW) // Counter Clock Wise Direction.....
      {        
        // pixels.setPixelColor(16-(PixelCounter), __shade);    
        pixels.setPixelColor(numPixels-(PixelCounter), __shade);    
        log_v("PixelCounterVal = %d",(16-PixelCounter));
      }     
      else
      if(direction == DIR_VSPLIT_DN) // Counter Clock Wise Direction.....
      {        
        pixels.setPixelColor(PixelCounter-1, __shade);        
        pixels.setPixelColor(numPixels-(PixelCounter), __shade);           

        log_i("PixelCounterVal = %d : %d : %d",PixelCounter,((numPixels/2)+PixelCounter-1),((numPixels/2)-(PixelCounter)));      
      }
      else
      if(direction == DIR_VSPLIT_UP) // Counter Clock Wise Direction.....
      {        
        pixels.setPixelColor((numPixels/2)+PixelCounter-1, __shade);        
        pixels.setPixelColor((numPixels/2)-(PixelCounter), __shade);    
        
        log_i("PixelCounterVal = %d : %d : %d",PixelCounter,((numPixels/2)+PixelCounter-1),((numPixels/2)-(PixelCounter)));      
      }
      pixels.show();        
      PixelCounter++;    
    }  
}

/****colorChase******************************************************************
 * colorFill(uint32_t c, uint16_t wait, uint8_t direction):
 * P1 = uint32_t c --> pixels.Color(255, 128, 128) // Select the color to show.
 * P2 = uint16_t wait --> Animation delay in ms
 * P3 = uint8_t direction --> Direction of animation - DIR_CW, DIR_CCW
 * Each pixel will be filled until last piexel 
 * Erase entire Ring
 * Start the routine again....
********************************************************************************/
void colorChase(uint32_t c, uint16_t wait, uint8_t direction)
{
  if (millis() - previousMillis >= wait) 
  { 
    log_v("PixelCounter = %d",PixelCounter);
    previousMillis = millis();      
    if(PixelCounter > pixels.numPixels()) // Display Number of pixels to be animated 
    {
      pixels.clear(); // Set all pixel colors to 'off'
      PixelCounter = -1; // so that all pixels are clear including 1st Pixel......
    }    
    
    pixels.clear();
    if(direction == DIR_CW)
    {
      pixels.setPixelColor(PixelCounter, c);
    }
    else
    if(direction == DIR_CCW)
    {
      pixels.setPixelColor(16-PixelCounter, c);
    }
    else
    if(direction == DIR_VSPLIT_UP)
    {
      pixels.setPixelColor(16-PixelCounter, c);
      pixels.setPixelColor(PixelCounter, c);
    }    
    pixels.show();        
    PixelCounter++;      
  }

}

/****colorChaseNum******************************************************************
 * colorChaseNum(uint32_t c, uint16_t wait, uint8_t NumPixels, uint8_t direction);
 * P1 = uint32_t c --> pixels.Color(255, 128, 128) // Select the color to show.
 * P2 = uint16_t wait --> Animation delay in ms
 * P3 = uint8_t NumPixels --> 0 to 15 
 * P4 = uint8_t direction --> Direction of animation - DIR_CW, DIR_CCW
 * Each pixel will be filled until last pixel 
 * Erase Previous Pixel
 * Start the routine again....
********************************************************************************/
void colorChaseNum(uint32_t c, uint16_t wait, uint8_t NumPixels, uint8_t direction)
{  
  if (millis() - previousMillis >= wait) 
  { 
    log_v("PixelCounter 0 = %d",PixelCounter);
    previousMillis = millis();   

    if(PixelCounter > (pixels.numPixels()-(NumPixels))) // Display Number of pixels to be animated 
    {
       log_v("PixelCounterEnd = %d",(pixels.numPixels()-(NumPixels)));
      pixels.clear(); // Set all pixel colors to 'off'
      PixelCounter = 0; // so that all pixels are clear including 1st Pixel......
    }    
    
    pixels.clear();    
    
    log_v("PixelCounter 1 = %d",PixelCounter);

    if(direction == DIR_CW)
    {
      pixels.setPixelColor(PixelCounter, c);
      for(uint8_t cnt = 0; cnt < NumPixels; cnt++)
      {      
        log_v("PixelCounter 2 = %d",PixelCounter+cnt);
        pixels.setPixelColor(PixelCounter+cnt, c);
      }
    }
    else
    if(direction == DIR_CCW)
    {
      pixels.setPixelColor(16-PixelCounter, c);
      for(uint8_t cnt = 0; cnt < NumPixels; cnt++)
      {      
        log_v("PixelCounter 2 = %d",PixelCounter+cnt);
        pixels.setPixelColor(16-(PixelCounter+cnt), c);
      }
    }
    else
    if(direction == DIR_VSPLIT_UP)
    {
      pixels.setPixelColor(PixelCounter, c);
      pixels.setPixelColor(16-PixelCounter, c);      
      for(uint8_t cnt = 0; cnt < NumPixels; cnt++)
      {      
        log_v("PixelCounter 2 = %d",PixelCounter+cnt);
         pixels.setPixelColor(PixelCounter+cnt, c);
        pixels.setPixelColor(16-(PixelCounter+cnt), c);
      }
    }

    
    
    pixels.show();        
    PixelCounter++;    
  
  }

}

/****colorRing******************************************************************
 * colorRing(uint32_t c);
 * P1 = uint32_t c --> pixels.Color(255, 128, 128) // Select the color to show.
 * Entire ring will be on with same colour
********************************************************************************/
void colorRing(uint32_t c)
{
  pixels.fill(c,0,pixels.numPixels());
  pixels.show();  
}
// void colorRing(uint32_t c)
// {
//   pixels.fill(c,0,16);
//   pixels.show();  
// }

void colorShiner(uint32_t c, uint16_t wait)
{
  static uint8_t stat1;
  // if (millis() - previousMillis >= wait) 
  // if (((stat1 == 0)&&(millis() - previousMillis >= wait)) ||((stat1 == 1)&&(millis() - previousMillis >= wait*0.25)) )
  if (((stat1 == 0)&&(millis() - previousMillis >= wait)) ||((stat1 == 1)&&(millis() - previousMillis >= wait)) )
  { 
    // log_v("PixelCounter 0 = %d",PixelCounter);
    previousMillis = millis();     
    
    if(stat1 == 0)
    {
      PixelBrightnessCntr++;
      if(PixelBrightnessCntr > 255)
      {
        PixelBrightnessCntr = 255;
        stat1 = 1;
      }
      
    }
    else
    {
      PixelBrightnessCntr--;
      if(PixelBrightnessCntr < 1)
      {
        PixelBrightnessCntr = 0;
        stat1 = 0;
      }
    }
    pixels.setBrightness(PixelBrightnessCntr);  
    pixels.fill(c,0,16);
    pixels.show();    
  }
  
}

void colorBeater(uint32_t c, uint16_t wait, uint16_t pauseCntr)
{
  static uint8_t stat1;
 
  // if(((stat1 < 2)&&(millis() - previousMillis >= wait))||((stat1 >= 2)&&(millis() - previousMillis >= wait)))
  if((millis() - previousMillis >= wait))
  { 
    // log_v("PixelCounter 0 = %d",PixelCounter);
    previousMillis = millis();     

    
    switch(stat1)
    {
      case 0:
        PixelBrightnessCntr++;
        if(PixelBrightnessCntr > 255)
        {
          PixelBrightnessCntr = 255;
          stat1 = 1;
        }
        pixels.setBrightness(PixelBrightnessCntr);  
        pixels.fill(c,0,16);
        pixels.show();    
      break;

      case 1:
        PixelBrightnessCntr--;
        if(PixelBrightnessCntr < 0)
        {
          PixelBrightnessCntr = 1;
          stat1 = 2;
        }
        pixels.setBrightness(PixelBrightnessCntr);  
        pixels.fill(c,0,16);
        pixels.show();    
      break;

      case 2:
        PixelBrightnessCntr++; // used to pause the animation.....
        if(PixelBrightnessCntr > pauseCntr)
        {
          PixelBrightnessCntr = 0;
          stat1 = 0;
        }
      break;

    }
  
  }

}

void colorBeater2Beats(uint32_t c, uint16_t wait)
{
  static uint8_t stat1;
  // if (millis() - previousMillis >= wait) 
  // if (((stat1 == 0)&&(millis() - previousMillis >= wait)) ||((stat1 == 1)&&(millis() - previousMillis >= wait*0.25)) )
  // if (((stat1 == 0)&&(millis() - previousMillis >= wait*0.25)) ||((stat1 == 1)&&(millis() - previousMillis >= wait)) )
  if(((stat1 < 2)&&(millis() - previousMillis >= wait))||((stat1 >= 2)&&(millis() - previousMillis >= wait)))
  { 
    // log_v("PixelCounter 0 = %d",PixelCounter);
    previousMillis = millis();     

    
    switch(stat1)
    {
      case 0:
        PixelBrightnessCntr++;
        if(PixelBrightnessCntr > 255)
        {
          PixelBrightnessCntr = 255;
          stat1 = 1;
        }
        pixels.setBrightness(PixelBrightnessCntr);  
        pixels.fill(c,0,16);
        pixels.show();    
      break;

      case 1:
        PixelBrightnessCntr = PixelBrightnessCntr - 2;
        if(PixelBrightnessCntr < 0)
        {
          PixelBrightnessCntr = 1;
          stat1 = 2;
        }
        pixels.setBrightness(PixelBrightnessCntr);  
        pixels.fill(c,0,16);
        pixels.show();    
      break;

      case 2:
        PixelBrightnessCntr++;
        if(PixelBrightnessCntr > 255)
        {
          PixelBrightnessCntr = 255;
          stat1 = 3;
        }
        pixels.setBrightness(PixelBrightnessCntr);  
        pixels.fill(c,0,16);
        pixels.show();    
      break;

      case 3:
        PixelBrightnessCntr = PixelBrightnessCntr - 2 ;
        if(PixelBrightnessCntr <= 0)
        {
          PixelBrightnessCntr = 0;
          stat1 = 4;
        }
        pixels.setBrightness(PixelBrightnessCntr);  
        pixels.fill(c,0,16);
        pixels.show();    
      break;

      case 4:
        PixelBrightnessCntr++;
        if(PixelBrightnessCntr > 200)
        {
          PixelBrightnessCntr = 0;
          stat1 = 0;
        }
      break;

    }
    
    // if((stat1 == 0)||(stat1 == 2))
    // {
    //   PixelBrightnessCntr++;
    //   // if(PixelBrightnessCntr > 255)
    //   if((stat1 == 0) &&(PixelBrightnessCntr > 128))
    //   {
    //     PixelBrightnessCntr = 255;
    //     if(stat1 == 0)  stat1 = 1;
    //     else stat1 = 3;
    //   }      
    // }
    // else
    // if((stat1 == 1)||(stat1 == 3))
    // {
    //   PixelBrightnessCntr--;
    //   if(PixelBrightnessCntr < 1)
    //   {
    //     PixelBrightnessCntr = 0;
    //     if(stat1 == 1) stat1 = 2;
    //     else stat1 = 0;
    //   }
    // }    
    
  }
  
}

void colorBeaterDualShades(uint32_t c1,uint32_t c2, uint16_t wait)
{
  static uint8_t stat1;
  // if (millis() - previousMillis >= wait) 
  // if (((stat1 == 0)&&(millis() - previousMillis >= wait)) ||((stat1 == 1)&&(millis() - previousMillis >= wait*0.25)) )
  // if (((stat1 == 0)&&(millis() - previousMillis >= wait*0.25)) ||((stat1 == 1)&&(millis() - previousMillis >= wait)) )
  if(((stat1 < 2)&&(millis() - previousMillis >= wait))||((stat1 >= 2)&&(millis() - previousMillis >= wait)))
  { 
    // log_v("PixelCounter 0 = %d",PixelCounter);
    previousMillis = millis();     

    
    switch(stat1)
    {
      case 0:
        PixelBrightnessCntr++;
        if(PixelBrightnessCntr > 255)
        {
          PixelBrightnessCntr = 255;
          stat1 = 1;
        }
        pixels.setBrightness(PixelBrightnessCntr);  
        pixels.fill(c1,0,16);
        pixels.show();    
      break;

      case 1:
        PixelBrightnessCntr = PixelBrightnessCntr-1;
        if(PixelBrightnessCntr < 0)
        {
          PixelBrightnessCntr = 1;
          stat1 = 2;
        }
        pixels.setBrightness(PixelBrightnessCntr);  
        pixels.fill(c1,0,16);
        pixels.show();    
      break;

      case 2:
        PixelBrightnessCntr++;
        if(PixelBrightnessCntr > 255)
        {
          PixelBrightnessCntr = 255;
          stat1 = 3;
        }
        pixels.setBrightness(PixelBrightnessCntr);  
        pixels.fill(c2,0,16);
        pixels.show();    
      break;

      case 3:
        PixelBrightnessCntr = PixelBrightnessCntr-1;
        if(PixelBrightnessCntr <= 0)
        {
          PixelBrightnessCntr = 0;
          stat1 = 4;
        }
        pixels.setBrightness(PixelBrightnessCntr);  
        pixels.fill(c2,0,16);
        pixels.show();    
      break;

      case 4:
        PixelBrightnessCntr++;
        if(PixelBrightnessCntr > 200)
        {
          PixelBrightnessCntr = 0;
          stat1 = 0;
        }
      break;

    }
    
  }
  
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) 
{
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) 
  {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) 
  {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
