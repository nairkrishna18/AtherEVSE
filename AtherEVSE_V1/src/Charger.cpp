#include "Charger.h"
#include <Arduino.h>
#include "GlobalHeader.h"
// #include "main.h"


uint16_t gNeoPixelState;

#ifndef _OTA_H_
  #define _OTA_H_
  #include "OTA.h"
#endif

#include "NeoPixelAPI.h"
#include "RFID_API.h"

#include "BUZZER.h"
#define DPIN_BUZZER     32  // D5 // for trial shifted to D5 Original A1
BUZZER Sound;

#if 1 //Modbus Related Initializations..........
  #include "ModbusRTU.h"
  #include "Modbus.h"

  ModbusRTU mb;
  #define REG_NUM         40 // Modbus Registers for Energy Meter
  /************************SELEC EM2M-1P-C-100A*******************/
  #define EM_ADD_ENERGY       0 //TOTAL ACTIVE ENERGY // 1 // IMPORT ACTIVE ENERGY,,,,
  #define EM_ADD_POWER        14 //TOTAL ACTIVE ENERGY // 1 // IMPORT ACTIVE ENERGY,,,,
  #define EM_ADD_VOLTAGE      20
  #define EM_ADD_CURRENT      22
  #define EM_ADD_POWER_FACTOR 24 //TOTAL ACTIVE ENERGY // 1 // IMPORT ACTIVE ENERGY,,,,
  #define EM_ADD_FREQUENCY    26


  /**********************************************/
  #define MBUS_HW_SERIAL Serial1
  #define MBUS_TXD_PIN   26          
  #define MBUS_RXD_PIN   25
  #define RXTX_PIN       33 // 15-OG
  typedef u_int32_t TaskProfilers; // used typedef for task profilers as it is integer datatype
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
    log_d("SlaveID: %d Hreg %d\r\n", address, start);
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
    #if 1  
    if (readSync(1, EM_ADD_ENERGY, 30, hregs1) == Modbus::EX_SUCCESS) // 1, 3, 5, 7 for energy meter values                
    {
      log_d("Modbus Response Received");
      // log_d("Parameter Reg Low  = %d", hregs1[0]);
      // log_d("Parameter Reg High = %d", hregs1[1]);
      // log_i("Parameter          = %f",ConvertEnergyMeterValues(hregs1[0],hregs1[1]));
      // for(int counter = 0; counter < 30 ;  counter++)
      // {
      //     log_i("hregs1[%d] = %d", counter, hregs1[counter]);
      // }

      log_i("Energy(kWh)      = %f",ConvertEnergyMeterValues(hregs1[EM_ADD_ENERGY],hregs1[EM_ADD_ENERGY+1]));
      log_i("Power(kW)        = %f",ConvertEnergyMeterValues(hregs1[EM_ADD_POWER],hregs1[EM_ADD_POWER+1]));
      log_i("Voltage(VAC)     = %f",ConvertEnergyMeterValues(hregs1[EM_ADD_VOLTAGE],hregs1[EM_ADD_VOLTAGE+1]));
      log_i("Current(Amp)     = %f",ConvertEnergyMeterValues(hregs1[EM_ADD_CURRENT],hregs1[EM_ADD_CURRENT+1]));
      log_i("Power Factor     = %f",ConvertEnergyMeterValues(hregs1[EM_ADD_POWER_FACTOR],hregs1[EM_ADD_POWER_FACTOR+1]));
      log_i("Frequency(Hz)    = %f",ConvertEnergyMeterValues(hregs1[EM_ADD_FREQUENCY],hregs1[EM_ADD_FREQUENCY+1]));



    }
    else
    {
      log_i("Modbus Response Not Received");
    }
    #elif 0
      if (readSync(1, EM_ADD_VOLTAGE, 2, hregs1) == Modbus::EX_SUCCESS) // 1, 3, 5, 7 for energy meter values                
      {
          log_d("Modbus Response Received");
          log_d("Parameter Reg Low  = %d", hregs1[0]);
          log_d("Parameter Reg High = %d", hregs1[1]);
          log_i("Parameter          = %f",ConvertEnergyMeterValues(hregs1[0],hregs1[1]));

      }
      else
      {
          log_i("Modbus Response Not Received");
      }
    #endif

    vTaskDelay(pdMS_TO_TICKS(1000)); // 1 sec delay for meter polling.......
  }
  /*****************************************************************************/

  void initModbus(void)
  {
      #if 1 // InitModbusFunction..............
          MBUS_HW_SERIAL.begin(9600, SERIAL_8N1, MBUS_RXD_PIN, MBUS_TXD_PIN);
          MBUS_HW_SERIAL.printf("\nI am Modbus Terminal @9600bps\n");
          mb.begin(&MBUS_HW_SERIAL, RXTX_PIN);
          mb.master();
      #endif  
  }

  #define STACKSIZE_MODBUS_TASK   5000 // BYTES
  void TaskModBus( void * pvParameters )
  {
      initModbus();
      while(1)
      {
        ProfilerModbus++;
        if(ProfilerModbus > 1)
        {
          ProfilerModbus = 0;
          log_i("TaskModBus  =  Used %d Bytes & Free %d Bytes ",(STACKSIZE_MODBUS_TASK-uxTaskGetStackHighWaterMark(NULL)), uxTaskGetStackHighWaterMark(NULL));
          log_v("MODBUS____TASK____RUNNING______________!");
        }      
        func_ReadHoldingRegister();
      }
  }
#endif // Modbus Functionality Ends here..........

/*********************FOTA TASK & FUNCTIONS STARTS HERE*****************/
void TaskFOTA( void * pvParameters )
{  
 
  // C:\Users\KRISHNA NAIR\.platformio\packages\framework-arduinoespressif32\tools\sdk\esp32\include\freertos\include\esp_additions\freertos
  
  if(!digitalRead(FOTA_SWITCH))
  {
    log_i("Fota Pin Pressed....");
    delay(1000);
    if(!digitalRead(FOTA_SWITCH))
    {
      log_i("Fota Pin Press Confirmed, Enable FOTA Task.....");      
      func_FotaTrigger(1);       
    }
  }

    while(1)
    {
      #if 0
      ProfilerFOTA++;
      if(ProfilerFOTA > 1)
      {
        ProfilerFOTA = 0;
        log_i("TaskFOTA  =  Used %d Bytes & Free %d Bytes ",(STACKSIZE_FOTA_TASK-uxTaskGetStackHighWaterMark(NULL)), uxTaskGetStackHighWaterMark(NULL));
        log_v("FOTA____TASK____RUNNING______________!");
      }      
      #endif

      // func_ReadHoldingRegister();
    }
}
/*********************FOTA TASK & FUNCTIONS ENDS HERE*******************/

/*********************RFID TASK & FUNCTIONS STARTS HERE*****************/
uint8_t initRfidModule(void)
{
     return initRFID(); 
}

#define STACKSIZE_RFID_TASK   5000 // BYTES
TaskHandle_t HandleRfid;
TaskProfilers ProfilerRfid;
void TaskRfid( void * pvParameters )
{
   
    if(!initRfidModule()) 
    {
      log_w("RFID TASK Suspended............");
      vTaskSuspend(HandleRfid);      
    }
    else
    {
      log_i("RFID Task Will Continue......");
    }
     
    while(1)
    {
      #if 1 // Profiler Counter...........
        ProfilerRfid++;
        if(ProfilerRfid > 100)
        {
          ProfilerRfid = 0;
          log_i("TaskRfid  =  Used %d Bytes & Free %d Bytes ",(STACKSIZE_RFID_TASK-uxTaskGetStackHighWaterMark(NULL)), uxTaskGetStackHighWaterMark(NULL));
          log_v("RFID____TASK____RUNNING______________!");
        }    
      #endif     
       funcRFIDAuthorize();
       delay(100);
      
    }
}

/*********************RFID TASK & FUNCTIONS ENDS HERE*******************/

/*********************NEOPIXEL TASK & FUNCTIONS STARTS HERE*************/
#define STACKSIZE_NEOPIXEL_TASK 5000 // BYTES
TaskProfilers ProfilerNeoPixel;
TaskHandle_t HandleNeoPixel;
void TaskNeoPixel( void * pvParameters )
{
    initNeoPixels(); 
    while(1)
    {
      #if 0 // it prints a lot of messages..................... better to disable this here......
      ProfilerNeoPixel++;
      if(ProfilerNeoPixel > 600000)
      {
        ProfilerNeoPixel = 0;
        log_i("TaskNeoPixel  =  Used %d Bytes & Free %d Bytes ",(STACKSIZE_NEOPIXEL_TASK-uxTaskGetStackHighWaterMark(NULL)), uxTaskGetStackHighWaterMark(NULL));  
        log_v("NEOPIXEL____TASK____RUNNING______________!");
      }
      #endif

      mainNeoPixel(gNeoPixelState); 
    }
}
/*********************NEOPIXEL TASK & FUNCTIONS ENDS HERE*************/

/*********************BUZZER TASK & FUNCTIONS STARTS HERE*************/
void initBuzzer(void)
{
  Sound.begin(DPIN_BUZZER);
  Sound.Buzz(Sound.BuzOffState);
} 
#define STACKSIZE_BUZZER_TASK 5000 // BYTES
TaskProfilers ProfilerBuzzer;
TaskHandle_t HandleBuzzer;
void TaskBuzzer( void * pvParameters )
{
    initBuzzer(); 
    while(1)
    {
      #if 0 // it prints a lot of messages..................... better to disable this here......
      ProfilerBuzzer++;
      if(ProfilerBuzzer > 600000)
      {
        ProfilerBuzzer = 0;
        log_i("TaskBuzzer  =  Used %d Bytes & Free %d Bytes ",(STACKSIZE_BUZZER_TASK-uxTaskGetStackHighWaterMark(NULL)), uxTaskGetStackHighWaterMark(NULL));  
        log_v("BUZZER____TASK____RUNNING______________!");
      }
      #endif
      // Sound.Beep(500,3000);
      
    }
}
/*********************BUZZER TASK & FUNCTIONS ENDS HERE***************/

/*********************MAIN STATE TASK & FUNCTIONS STARTS HERE*************/
void initMainState(void)
{
  
  
} 

#define STACKSIZE_MAINSTATE_TASK 5000 // BYTES
TaskProfilers ProfilerMainState;
TaskHandle_t HandleMainState;
void TaskMainState( void * pvParameters )
{
    initMainState(); 
    while(1)
    {
      #if 0 // it prints a lot of messages..................... better to disable this here......
      ProfilerMainState++;
      if(ProfilerMainState > 60000)
      {
        ProfilerMainState = 0;
        log_i("TaskMainState  =  Used %d Bytes & Free %d Bytes ",(STACKSIZE_MAINSTATE_TASK-uxTaskGetStackHighWaterMark(NULL)), uxTaskGetStackHighWaterMark(NULL));  
        log_v("MAIN_STATE____TASK____RUNNING______________!");
      }
      #endif

      delay(200);
    }
}
/*********************MAIN STATE TASK & FUNCTIONS ENDS HERE***************/

/*********************CREATE TASK STARTS HERE*************************/
void initCreateTask(void)
{ 

  log_i("VERSION_NO = %d\n",VERSION_NO); // @OTA.h
  pinMode(FOTA_SWITCH , INPUT_PULLUP); 
  createFotaTask(1); // Created @core 0

  /***********
   * Check for the FOTA Switch
   * If Fota switch is pressed and confirmed continue with FOTA Task
   * Else Continue with Other Tasks...........
  */
  if(!digitalRead(FOTA_SWITCH))
  {
    log_i("Fota Pin Pressed....");
    delay(1000);
    if(!digitalRead(FOTA_SWITCH))
    {
      log_i("Fota Pin Press Confirmed, Enable FOTA Task.....");      
      func_FotaTrigger(1);       // Core 0
    }
  }
  else // Create Below Task only if FOTA is not activated...................
  {
    xTaskCreatePinnedToCore(
                    TaskModBus,           /* Task function. */
                    "ModbusTask",         /* name of task. */
                    STACKSIZE_MODBUS_TASK,/* Stack size of task in bytes*/
                    NULL,                 /* parameter of the task */
                    3,                    /* 3priority of the task */
                    &HandleModbus,        /* Task handle to keep track of created task, always pass address of handle*/
                    0);  
                                     /* pin task to core 0 */
    xTaskCreatePinnedToCore(
                    TaskRfid,           /* Task function. */
                    "RFID Task",         /* name of task. */
                    STACKSIZE_RFID_TASK,/* Stack size of task in bytes*/
                    NULL,                 /* parameter of the task */
                    3,                    /* 3priority of the task */
                    &HandleRfid,        /* Task handle to keep track of created task, always pass address of handle*/
                    0);                   /* pin task to core 0 */

  }  
                    
  //////////////////Below Tasks are Non Bloacking Tasks Also Used in FOTA............................                    
  xTaskCreatePinnedToCore(
                    TaskNeoPixel,           /* Task function. */
                    "NeopixelTask",         /* name of task. */
                    STACKSIZE_NEOPIXEL_TASK,/* Stack size of task in bytes*/
                    NULL,                   /* parameter of the task */
                    3,                      /* 3priority of the task */
                    &HandleNeoPixel,        /* Task handle to keep track of created task, always pass address of handle*/
                    1);                     /* pin task to core 0 */

  xTaskCreatePinnedToCore(
                    TaskBuzzer,           /* Task function. */
                    "BuzzerTask",         /* name of task. */
                    STACKSIZE_BUZZER_TASK,/* Stack size of task in bytes*/
                    NULL,                   /* parameter of the task */
                    3,                      /* 3priority of the task */
                    &HandleBuzzer,        /* Task handle to keep track of created task, always pass address of handle*/
                    1);                     /* pin task to core 0 */

  xTaskCreatePinnedToCore(
                    TaskMainState,           /* Task function. */
                    "MainStateTask",         /* name of task. */
                    STACKSIZE_MAINSTATE_TASK,/* Stack size of task in bytes*/
                    NULL,                   /* parameter of the task */
                    3,                      /* 3priority of the task */
                    &HandleMainState,        /* Task handle to keep track of created task, always pass address of handle*/
                    0);                     /* pin task to core 0 */

}
/*********************BUZZER TASK & FUNCTIONS ENDS HERE***************/


/*********************EOF*********************************************/


