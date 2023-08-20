#include "Charger.h"

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

  vTaskDelay(pdMS_TO_TICKS(300)); // 1 sec delay for meter polling.......
}
/*****************************************************************************/
#endif // Modbus Functionality Ends here..........

void initModbus(void)
{
    #if 1 // InitModbusFunction..............
        MBUS_HW_SERIAL.begin(9600, SERIAL_8N1, MBUS_RXD_PIN, MBUS_TXD_PIN);
        MBUS_HW_SERIAL.printf("\nI am Modbus Terminal @9600bps\n");
        mb.begin(&MBUS_HW_SERIAL, RXTX_PIN);
        mb.master();
    #endif

}

void funcChargerStateMachine(void)
{
    #if 1
      func_ReadHoldingRegister();
    #endif
}