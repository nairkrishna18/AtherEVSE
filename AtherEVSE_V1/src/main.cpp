#include <Arduino.h>
#include "Charger.h"








/***********************************************************
 * Function Name: void init_EspDetails(void)
 * Display Various details of esp32 mcu... 
***********************************************************/
void init_EspDetails(void)
{  
  log_i("SDK Version       = %s"  ,ESP.getSdkVersion());
  log_i("Chip Model        = %s"  ,ESP.getChipModel());  
  log_i("Chip Revision     = %d"  ,ESP.getChipRevision());  
  log_i("Chip Cores(#)     = %d"  ,ESP.getChipCores());
  log_i("CPU Freq(MHz)     = %d"  ,ESP.getCpuFreqMHz());  
  log_i("Flash Size(Bytes) = %d"  ,ESP.getFlashChipSize());  
  log_i("PSRAM Size(Bytes) = %d"  ,ESP.getPsramSize());
  log_i("Free PSRAM(Bytes) = %d"  ,ESP.getFreePsram());
  log_i("E Fuse/ChipId     = %lld",ESP.getEfuseMac());  
  log_i("FlashChipSpeed(Hz)= %d"  ,ESP.getFlashChipSpeed());   
  log_i("Flash Chip Mode   = %d"  ,ESP.getFlashChipMode());   
  log_i("Flash Chip Size   = %d"  ,ESP.getFlashChipSize());
  log_i("Heap Size (Bytes) = %d"  ,ESP.getHeapSize());
  log_i("Free Heap (Bytes) = %d"  ,ESP.getFreeHeap());
  log_i("FreeSketch(Bytes) = %d"  ,ESP.getFreeSketchSpace());
  log_i("Sketch Size(Bytes)= %d"  ,ESP.getSketchSize());   
}




void setup()
{
  Serial.begin(115200);
   
  log_i("|----------------System-Initializing----------------|");
  log_i("Serial Port Configured @115200 8-N-1");
  
  // init_EspDetails();

  log_i("Check for FOTA Update, Press Switch.....");
  log_i("Initializing.");
  
  // for(int i1=0; i1<25; i1++)
  // {
  //   Serial.print(".");
  //   delay(200);
  // }  
  
  // pinMode(OUT_RLY_PIN  , OUTPUT);
  // digitalWrite(OUT_RLY_PIN, HIGH);    // LOW = OFF, HIGH = ON  
  // delay(3000);
  // digitalWrite(OUT_RLY_PIN, LOW);    // LOW = OFF, HIGH = ON
  // delay(3000);


  initCreateTask();
  
  log_i("|----------------System-Initialized----------------|\n\n");
  delay(2000); 
 
  

  log_i("Setup Completed");
}

void loop()
{  

  #if 0 // to test Out SSD Pin..........
    digitalWrite(OUT_RLY_PIN, HIGH);    // Turn On Output Relay
    delay(3000);
    digitalWrite(OUT_RLY_PIN, LOW);    // Turn Off Output Relay
    delay(3000);
  #endif

  delay(100);    
}

 