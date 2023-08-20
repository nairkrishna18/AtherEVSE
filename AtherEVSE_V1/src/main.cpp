#include <Arduino.h>

#ifndef _OTA_H_
  #define _OTA_H_
  #include "OTA.h"
#endif

#include "Charger.h"

#if 1 // RGB LED 
  #include "RGB_LED.h"

  RGB_LED LED;

  #define RED_LED_PIN     2
  #define GREEN_LED_PIN   4
  #define BLUE_LED_PIN    5
#endif


#if 0 //Modbus Related Initializations..........
#include "ModbusRTU.h"
#include "Modbus.h"

ModbusRTU mb;
#define REG_NUM         40 // Modbus Registers for Energy Meter
/************************SELEC EM2M-1P-C-100A*******************/
#define EM_ADD_VOLTAGE    20
#define EM_ADD_CURRENT    22
#define EM_ADD_FREQUENCY  26
#define EM_ADD_ENERGY     0 //TOTAL ACTIVE ENERGY // 1 // IMPORT ACTIVE ENERGY,,,,

/**********************************************/
#define MBUS_HW_SERIAL Serial1
#define MBUS_TXD_PIN   26          
#define MBUS_RXD_PIN   25
#define RXTX_PIN       33 // 15-OG
typedef int TaskProfilers; // used typedef for task profilers as it is integer datatype
TaskProfilers ProfilerModbus;
TaskHandle_t HandleModbus;
uint16_t hregs1[REG_NUM];

// Function definitions for Modbus

/***********************************************************************************************/
xSemaphoreHandle xMutex;
Modbus::ResultCode err;
Modbus::ResultCode readSync(uint8_t address, uint16_t start, uint16_t num, uint16_t* buf) 
{
  // xSemaphoreTake(xMutex, portMAX_DELAY);
  if (mb.slave()) 
  {
    // xSemaphoreGive(xMutex);
    return Modbus::EX_GENERAL_FAILURE;
  }
  Serial.printf("SlaveID: %d Hreg %d\r\n", address, start);
  mb.readIreg(address, start, buf, num, [](Modbus::ResultCode event, uint16_t, void*) 
  {
    err = event;
    return true;
  });
  while (mb.slave()) 
  {
    vTaskDelay(1);
    mb.task();
  }
  Modbus::ResultCode res = err;
  // xSemaphoreGive(xMutex);
  return res;
}
/*****************************************************************************/
float ConvertEnergyMeterValues(int _LowReg, int _HighReg)
{
  int _iParameter;
  float _fParameter;
  _iParameter = _HighReg << 16; // shift 4 bytes to left   
  _iParameter = ((_iParameter) | (_LowReg)); // bit ORing with current value
  _fParameter = *(float *) &_iParameter; // convert to float & print.
  return _fParameter;
}
/******************************************************************************************/
void func_ReadHoldingRegister(void)
{
  if (readSync(1, EM_ADD_VOLTAGE, 6, hregs1) == Modbus::EX_SUCCESS) // 1, 3, 5, 7 for energy meter values                
  {
    log_i("Modbus Response Received");
    log_i("Parameter Reg Low  = %d", hregs1[0]);
    log_i("Parameter Reg High = %d", hregs1[1]);
    log_i("Parameter          = %f",ConvertEnergyMeterValues(hregs1[0],hregs1[1]));

  }
  else
  {
    log_i("Modbus Response Not Received");
  }
  vTaskDelay(pdMS_TO_TICKS(1000)); // 1 sec delay for meter polling.......
}
/*****************************************************************************/
#endif // Modbus Functionality Ends here..........



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
  log_i("Serial Port Configured @115200");
  log_i("VERSION_NO = %d\n",VERSION_NO); // @OTA.h
  pinMode(SW3_PIN , INPUT_PULLUP); 
  LED.begin(RED_LED_PIN,GREEN_LED_PIN, BLUE_LED_PIN);
  
  LED.Display(LED.White);
	delay(500);

  delay(100);  


  init_EspDetails();
  init_Fota(1);


  log_i("Check for FOTA Update, Press Switch.....");
  log_i("Initializing.");
  
  for(int i1=0; i1<25; i1++)
  {
    Serial.print(".");
    delay(200);
  }

// C:\Users\KRISHNA NAIR\.platformio\packages\framework-arduinoespressif32\tools\sdk\esp32\include\freertos\include\esp_additions\freertos
  
  if(!digitalRead(SW3_PIN))
  {
    log_i("Fota Pin Pressed....");
    delay(1000);
    if(!digitalRead(SW3_PIN))
    {
      log_i("Fota Pin Press Confirmed, Enable FOTA Task.....");      
      func_FotaTrigger(1);       
    }
  }

#if 0 // InitModbusFunction..............
  MBUS_HW_SERIAL.begin(9600, SERIAL_8N1, MBUS_RXD_PIN, MBUS_TXD_PIN);
  MBUS_HW_SERIAL.printf("\nI am Modbus Terminal @38400bps\n");
  mb.begin(&MBUS_HW_SERIAL, RXTX_PIN);
  mb.master();
#endif
initModbus();

  log_i("|----------------System-Initialized----------------|\n\n");
  delay(2000); 

  log_i("Setup Completed");
}

void loop()
{
   
    LED.Flash(LED.Yellow,10);

    #if 0
      func_ReadHoldingRegister();
    #endif

    funcChargerStateMachine();
   

  delay(10);
}
