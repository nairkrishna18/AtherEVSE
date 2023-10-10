#include "Charger.h"
#include <Arduino.h>
#include "GlobalHeader.h"

#define OUT_RLY_PIN     21
#define OUT_RELAY_ON    digitalWrite(OUT_RLY_PIN, HIGH)   // LOW = OFF, HIGH = ON  
#define OUT_RELAY_OFF   digitalWrite(OUT_RLY_PIN, LOW)    // LOW = OFF, HIGH = ON  


uint16_t gNeoPixelState;
uint16_t gBuzzerState;
int8_t gi8_MSTATE = MSTATE_UNKNOWN; // Main States of State Machine
int8_t gi8_NXT_MSTATE; // Previous Main States of State Machine for Wait State....
uint16_t gi6_WaitDelay;
int8_t gi8_RFID_SCAN_ENABLE; // RFID Scan enable variable.........

float Energy;
float Power;
float Voltage;
float Current;
float Frequency;


#ifndef _OTA_H_
  #define _OTA_H_
  #include "OTA.h"
#endif

#ifndef __BL_CLASSIC_CPP
    #define __BL_CLASSIC_CPP
    #include "BL_Classic.h"
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
      
      #if 1
        Energy    = ConvertEnergyMeterValues(hregs1[EM_ADD_ENERGY]      ,hregs1[EM_ADD_ENERGY+1]);
        Power     = ConvertEnergyMeterValues(hregs1[EM_ADD_POWER]       ,hregs1[EM_ADD_POWER+1]);
        Voltage   = ConvertEnergyMeterValues(hregs1[EM_ADD_VOLTAGE]     ,hregs1[EM_ADD_VOLTAGE+1]);
        Current   = ConvertEnergyMeterValues(hregs1[EM_ADD_CURRENT]     ,hregs1[EM_ADD_CURRENT+1]);
        Power     = ConvertEnergyMeterValues(hregs1[EM_ADD_POWER_FACTOR],hregs1[EM_ADD_POWER_FACTOR+1]);
        Frequency = ConvertEnergyMeterValues(hregs1[EM_ADD_FREQUENCY]   ,hregs1[EM_ADD_FREQUENCY+1]);

        log_d("Energy(kWh)      = %f", Energy);
        log_d("Power(kW)        = %f", Power);
        log_d("Voltage(VAC)     = %f", Voltage);
        log_i("Current(Amp)     = %f", Current);
        log_d("Power Factor     = %f", Power);
        log_d("Frequency(Hz)    = %f", Frequency);
      #else
        log_i("Energy(kWh)      = %f",ConvertEnergyMeterValues(hregs1[EM_ADD_ENERGY],hregs1[EM_ADD_ENERGY+1]));
        log_i("Power(kW)        = %f",ConvertEnergyMeterValues(hregs1[EM_ADD_POWER],hregs1[EM_ADD_POWER+1]));
        log_i("Voltage(VAC)     = %f",ConvertEnergyMeterValues(hregs1[EM_ADD_VOLTAGE],hregs1[EM_ADD_VOLTAGE+1]));
        log_i("Current(Amp)     = %f",ConvertEnergyMeterValues(hregs1[EM_ADD_CURRENT],hregs1[EM_ADD_CURRENT+1]));
        log_i("Power Factor     = %f",ConvertEnergyMeterValues(hregs1[EM_ADD_POWER_FACTOR],hregs1[EM_ADD_POWER_FACTOR+1]));
        log_i("Frequency(Hz)    = %f",ConvertEnergyMeterValues(hregs1[EM_ADD_FREQUENCY],hregs1[EM_ADD_FREQUENCY+1]));
      #endif



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
        #if 0
        ProfilerModbus++;
        if(ProfilerModbus > 1)
        {
          ProfilerModbus = 0;
          log_i("TaskModBus  =  Used %d Bytes & Free %d Bytes ",(STACKSIZE_MODBUS_TASK-uxTaskGetStackHighWaterMark(NULL)), uxTaskGetStackHighWaterMark(NULL));
          log_v("MODBUS____TASK____RUNNING______________!");
        }    
        #endif

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
      #if 0  // Profiler Counter...........
        ProfilerRfid++;
        if(ProfilerRfid > 100)
        {
          ProfilerRfid = 0;
          log_d("TaskRfid  =  Used %d Bytes & Free %d Bytes ",(STACKSIZE_RFID_TASK-uxTaskGetStackHighWaterMark(NULL)), uxTaskGetStackHighWaterMark(NULL));
          log_v("RFID____TASK____RUNNING______________!");
        }    
      #endif     
       funcRFIDAuthorize(gi8_RFID_SCAN_ENABLE);
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

  log_i("|----------------Buzzer Initialized--------------|");
  #if 1 // Disable if it is distracting........
    Sound.Buzz(Sound.BuzOnState);
    vTaskDelay(pdMS_TO_TICKS(150)); // 1 sec delay for Buzzer Beep To indicate it is initialized....
  #endif
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
      // Sound.Beep(100,3000);
      Sound.MainBuzzerStates(gBuzzerState);
      
    }
}
/*********************BUZZER TASK & FUNCTIONS ENDS HERE***************/

/*********************Bluetooth TASK & FUNCTIONS STARTS HERE*************/
void initBluetooth(void)
{
   init_BluetoothClassic();
  log_i("|----------------Bluetooth Communication Initialized--------------|");
  
} 

#define STACKSIZE_BL_TASK 5000 // BYTES
TaskProfilers ProfilerBluetooth;
TaskHandle_t HandleBluetooth;
void TaskBluetooth( void * pvParameters )
{
    initBluetooth(); 
    HoldingBuff[0] = 1234;
    HoldingBuff[1] = 0;

    while(1)
    {
      #if 1 // it prints a lot of messages..................... better to disable this here......
      ProfilerBluetooth++;
      if(ProfilerBluetooth > 600000)
      {
        HoldingBuff[0]++; // Voltage
        HoldingBuff[1]++; // Current
        HoldingBuff[2]++; // Energy
        HoldingBuff[3]++; // Frequency
        HoldingBuff[4] = 1; // Charge State
        HoldingBuff[5] = 2; // Error State


        log_i("HoldingBuff[0]=%d",HoldingBuff[0]);
        log_i("HoldingBuff[1]=%d",HoldingBuff[1]);

        ProfilerBluetooth = 0;
        log_i("TaskBluetooth  =  Used %d Bytes & Free %d Bytes ",(STACKSIZE_BUZZER_TASK-uxTaskGetStackHighWaterMark(NULL)), uxTaskGetStackHighWaterMark(NULL));  
        log_v("BLUETOOTH____TASK____RUNNING______________!");
      }
      #endif
      
       // BL_TEST();
      
      recvWithStartEndMarkers();       
      
      

      if (newData == true) 
      { 
        parseBluetoothData();     
      }
      
    }
}
/*********************BUZZER TASK & FUNCTIONS ENDS HERE***************/



/*********************MAIN STATE TASK & FUNCTIONS STARTS HERE*************/
void initMainState(void)
{
  pinMode(OUT_RLY_PIN  , OUTPUT);
  #if 0
    OUT_RELAY_ON;
    delay(3000);
  #endif
  OUT_RELAY_OFF; 
  
} 


/***********************************************************************************
 * Function Name : void TaskMainState( void * pvParameters )
 * Argument : Various parameters 
***********************************************************************************/
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
      if(ProfilerMainState > 600)
      {
        ProfilerMainState = 0;
        log_d("TaskMainState  =  Used %d Bytes & Free %d Bytes ",(STACKSIZE_MAINSTATE_TASK-uxTaskGetStackHighWaterMark(NULL)), uxTaskGetStackHighWaterMark(NULL));  
        log_v("MAIN_STATE____TASK____RUNNING______________!");
        
        log_i("\n|---------------Variables------------------|");         
        log_i("gi8_RFID_STATUS   = %d", gi8_RFID_STATUS); 
        log_i("gi8_MSTATE        = %d", gi8_MSTATE);
        log_i("gi8_NXT_MSTATE    = %d", gi8_NXT_MSTATE);
        log_i("gBuzzerState      = %d", gBuzzerState); 
        log_i("gMasterCardStatus = %d", gMasterCardStatus); 
        log_i("gNeoPixelState    = %d", gNeoPixelState); 
        log_i("gStatusNeoPixel   = %d", gStatusNeoPixel); 
      }
      #endif    

      funcComState();
      funcMainStateMachine();

      delay(10);


    }
}
/*********************MAIN STATE TASK & FUNCTIONS ENDS HERE***************/

/*********************CREATE TASK STARTS HERE*************************/
void initCreateTask(void)
{ 

  log_i("VERSION_NO = %d\n",VERSION_NO); // @OTA.h
  pinMode(FOTA_SWITCH , INPUT_PULLUP); 
  createFotaTask(1); // Created @core 0

  /**********************************************************************
   * Check for the FOTA Switch
   * If Fota switch is pressed and confirmed continue with FOTA Task
   * Else Continue with Other Tasks...........
  ***********************************************************************/
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

    xTaskCreatePinnedToCore(
                    TaskMainState,           /* Task function. */
                    "MainStateTask",         /* name of task. */
                    STACKSIZE_MAINSTATE_TASK,/* Stack size of task in bytes*/
                    NULL,                   /* parameter of the task */
                    3,                      /* 3priority of the task */
                    &HandleMainState,        /* Task handle to keep track of created task, always pass address of handle*/
                    0);                     /* pin task to core 0 */
  }  
                    
  //////////////////Below Tasks are Non Blocking Tasks Also Used in FOTA............................                    
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
                    TaskBluetooth,           /* Task function. */
                    "BluetoothTask",         /* name of task. */
                    STACKSIZE_BL_TASK,/* Stack size of task in bytes*/
                    NULL,                   /* parameter of the task */
                    3,                      /* 3priority of the task */
                    &HandleBluetooth,        /* Task handle to keep track of created task, always pass address of handle*/
                    1);                     /* pin task to core 0 */



  

}
/************************EO void initCreateTask(void)************************/

void funcComState(void)
{

}

void funcMainStateMachine(void)
{

  static int8_t __varChargeStates;

  switch(gi8_MSTATE)
  {
    case MSTATE_ERROR:

    break;

    case MSTATE_UNKNOWN:
    if(gi8_RFID_STATUS == RFID_READER_PRESENT)
    {
      gi8_MSTATE = MSTATE_INIT;
    }

    break;

    case MSTATE_INIT:
    gi8_MSTATE = MSTATE_IDLE;
    OUT_RELAY_OFF;
    gi8_RFID_SCAN_ENABLE = 1;
    __varChargeStates = __initState; // Init State.......

    break;

    case MSTATE_IDLE:

      if(gi8_RFID_STATUS == RFID_NEW_CARD_DENIED)
      {
        gNeoPixelState  = NP_RFID_DENIED;
        gBuzzerState    = BUZSTATE_RFID_DENIED;

        gi8_NXT_MSTATE  = gi8_MSTATE;
        gi8_MSTATE      = MSTATE_WAIT;       
        gi8_RFID_STATUS = RFID_STATUS_UNKNOWN;
        gi6_WaitDelay   = 1000;
      }
      else
      if(gi8_RFID_STATUS == RFID_NEW_CARD_AUTHORIZED)
      {
        gNeoPixelState  = NP_RFID_AUTHORIZED;
        gBuzzerState    = BUZSTATE_RFID_AUTHORIZED;

        __varChargeStates = __rfidAuthIdle; // RFID Card Authorized in Idle State

        gi8_NXT_MSTATE  = MSTATE_PREPARING;
        gi8_MSTATE      = MSTATE_WAIT;       
        gi8_RFID_STATUS = RFID_STATUS_UNKNOWN;
        gi6_WaitDelay   = 1100;
        
      }
      else
      {
        gNeoPixelState = NP_IDLE_STATE;
      }

    break;

    case MSTATE_PREPARING: // Wait for Gun to Connect

      gNeoPixelState  = NP_PREPARING_STATE;     
      gi8_RFID_SCAN_ENABLE = 0;

      __varChargeStates = __PrepState; // In Preparing State 

      gi8_NXT_MSTATE  = MSTATE_PRE_CHARGING;
      gi8_MSTATE      = MSTATE_WAIT;       
      gi8_RFID_STATUS = RFID_STATUS_UNKNOWN;
      gi6_WaitDelay   = 5000; // Gun Detection and Detection Time is considered as 5 Seconds....

    break;

    case MSTATE_PRE_CHARGING: // Turn on Output Contactor & Wait for CURRENT TO SENSE.....

      if(__varChargeStates == __PrepState)
      {
        gNeoPixelState  = NP_PRECHARGE_STATE;   
        __varChargeStates = __outRelayOn; // Output relay On   

        gi8_NXT_MSTATE  = MSTATE_PRE_CHARGING;
        gi8_MSTATE      = MSTATE_WAIT;       
        gi8_RFID_STATUS = RFID_STATUS_UNKNOWN;
        gi6_WaitDelay   = 5000; // Minimum Current detection 

        OUT_RELAY_ON;


        
      }
      else
      if(__varChargeStates == __outRelayOn)
      {
        
        if(Current != 0 )
        {
          __varChargeStates = __senChargCurr; // Sensed Charging Current
          gi8_NXT_MSTATE  = MSTATE_PRO_CHARGING;
          gi8_MSTATE      = MSTATE_WAIT;       
          gi8_RFID_STATUS = RFID_STATUS_UNKNOWN;
          gi6_WaitDelay   = 10; // Minimum Current detection 

          PixelCounter = 0;
          

        }            
        else // Load is not connected.....
        {
          OUT_RELAY_OFF;
          gi8_MSTATE      = MSTATE_INIT; 
          __varChargeStates = __noChargCurr; // Not sensed Charging Current
        }         
      }      

    break;

    case MSTATE_PRO_CHARGING:       
      
      gi8_RFID_SCAN_ENABLE = 1;
      
      if(Current == 0)
      {
        log_i("Current(0) = %d", Current);
        if((__varChargeStates == __senChargCurr))
        {
          __varChargeStates = __evFinCharging; // EV Finished Charging/ Stopped Charging/ Rejected Charging..........
          gNeoPixelState  = NP_PRECHARGE_STATE;  
          gi8_NXT_MSTATE  = MSTATE_PRO_CHARGING;
          gi8_MSTATE      = MSTATE_WAIT;               
          gi6_WaitDelay   = 5000; // Minimum Current detection 
        }
        else
        if((__varChargeStates == __evFinCharging)) // Still Current == 0
        {
          OUT_RELAY_OFF;
          __varChargeStates = __outRelayOff;  // Turned Off Output relay............          
          gi8_NXT_MSTATE  = MSTATE_FIN_CHARGING;
          gi8_MSTATE      = MSTATE_WAIT;               
          gi6_WaitDelay   = 10; // Minimum Current detection 
        }
      }
      else
      if(Current > 0)
      {

        gNeoPixelState  = NP_PROCHARGE_STATE; 

        if(gi8_RFID_STATUS == RFID_NEW_CARD_DENIED)
        {
          gNeoPixelState  = NP_RFID_DENIED;
          gBuzzerState    = BUZSTATE_RFID_DENIED;

          gi8_NXT_MSTATE  = MSTATE_PRO_CHARGING;
          gi8_MSTATE      = MSTATE_WAIT;       
          gi8_RFID_STATUS = RFID_STATUS_UNKNOWN;
          gi6_WaitDelay   = 1000;
        }
        else
        if(gi8_RFID_STATUS == RFID_NEW_CARD_AUTHORIZED)
        {
          __varChargeStates = __rfidAuthProg;   // Rfid card Authorized in Progress Charging State     

          PixelCounter = 0;       
          gNeoPixelState  = NP_RFID_AUTHORIZED;             

          gBuzzerState    = BUZSTATE_RFID_AUTHORIZED;          

          gi8_NXT_MSTATE  = MSTATE_STP_CHARGING;
          gi8_MSTATE      = MSTATE_WAIT;       
          gi8_RFID_STATUS = RFID_STATUS_UNKNOWN;
          gi6_WaitDelay   = 1100;
          OUT_RELAY_OFF;             

        }

      }
      
      
    break;

    case MSTATE_STP_CHARGING: // Charging Stopped By user By means of RFID
    gNeoPixelState  = NP_STPCHARGE_STATE;
    gi8_RFID_SCAN_ENABLE = 0;
    if(__varChargeStates == __rfidAuthProg)
    {
      __varChargeStates = __userStopCharging; // User Stopped Charging
      gi8_NXT_MSTATE  = MSTATE_INIT;
      gi8_MSTATE      = MSTATE_WAIT;               
      gi6_WaitDelay   = 5000; 
    }
    break;

    case MSTATE_FIN_CHARGING: // Charging stopped by EV Charging Current = 0
    gNeoPixelState  = NP_FINCHARGE_STATE;
    if(__varChargeStates == __outRelayOff)
    {
      __varChargeStates = __evFinChargingFinal; // Ev Stopped Charging..........
      gi8_NXT_MSTATE  = MSTATE_INIT;
      gi8_MSTATE      = MSTATE_WAIT;               
      gi6_WaitDelay   = 5000; 
    }

    break;

    case MSTATE_WAIT:
      log_i("|---------Start Of Delay = %d---------|",gi6_WaitDelay );
      vTaskDelay(pdMS_TO_TICKS(gi6_WaitDelay)); // 1 sec delay for meter polling.......
      gi8_MSTATE = gi8_NXT_MSTATE;      
      log_i("|-------------End Of Delay-------------|");
      gNeoPixelState  = NP_BLANK;
      gBuzzerState    = BUZSTATE_OFF;
    break;

    default:
      gi8_MSTATE = MSTATE_UNKNOWN;
    break;

  }


}



/*********************EOF*********************************************/


