


#define NEW_BL_DATA_AVAILABLE    (SerialBT.available() > 0)
#define MAX_PARAM           18 // 18 maximum nine parameters
#define MAX_HOLDING_SIZE    (MAX_PARAM * 2)
#define MAX_DATA            (MAX_PARAM * 4 ) // 64


#define NUM_CHARS           (MAX_DATA+13) // MAX_DATA+13

#define STATE_SID 0 // SLAVE ID
#define STATE_FC  1 // FUNCTION CODE
#define STATE_NB  2 // NUMBER OF BYTES
#define STATE_SA  3 // STARTING ADDRESS
#define STATE_CS  4 // CHECKSUM
#define STATE_SF  5 // SUB FUNCTION

#define STATE_END 15 // END

#define SLAVE_BID 350 // S0 slave id S0 for broad cast messages
#define SLAVE_ID  351 // S1 slave id S1

#define F1_READ_HR        221 // read number of bytes from starting address mentioned
#define F2_WRITE_HR       222 // write number of bytes to starting address mentioned
#define F3_WR_RTC         223 // Configure Date and Time
#define F4_RD_RTC         224 // Read Current Data of RTC
#define F5_MANUAL_CONFIG  225 // Button Detection and Status
#define F6_AUTO_CONFIG    226 // Configure Loads
#define F7_NEOPIXEL       227 // Configure Loads


#define SF1 671 // s1 Read RTC Data    || Oxygen On Duration || Heater On Duration || Filter Start Time   || Light Start Time 1
#define SF2 672 // s2 Filter On Time   || Light Stop time 1
#define SF3 673 // s3 Light Start Time 2
#define SF4 674 // s4 Light Stop time 2
//#define SF5 675

#define LOAD_OXYGEN 281  // L1 OXYGEN LOAD1
#define LOAD_HEATER 282  // L2 HEATER LOAD2
#define LOAD_LIGHT  283  // L3 LIGHT  LOAD3
#define LOAD_FILTER 284  // L4 FILTER LOAD4

// #include "RTClib.h"

/*below mentioned parameters are used to load holding registers*/
enum eParameters 
{
  pOxygenOnDuration, // 0
  pHeaterOnDuration, // 1
  pFilterStartTime,  // 2
  pFilterOnDuration, // 3
  pLightStartTime1,  // 4
  pLightStopTime1,   // 5
  pLightStartTime2,  // 6
  pLightStopTime2,   // 7
  pLoadStatus1,      // 8 
};

//unsigned int holding_Reg[MAX_HOLDING_SIZE]; //18



#define OXYGEN_ON_DURATION_MM   HoldingBuff[0]
#define HEATER_ON_DURATION_MM   HoldingBuff[1]
#define FILTER_START_TIME       HoldingBuff[2]
#define FILTER_ON_DURATION_HR   HoldingBuff[3]
#define LIGHT_START_TIME1       HoldingBuff[4]
#define LIGHT_STOP_TIME1        HoldingBuff[5]
#define LIGHT_START_TIME2       HoldingBuff[6]
#define LIGHT_STOP_TIME2        HoldingBuff[7]
#define LOAD_STATUS_INT         HoldingBuff[8]
#define LOAD_MODE_INT           HoldingBuff[9]
#define AQUARIUM_TEMPERATURE    HoldingBuff[10]
#define SET_TEMPERATURE         HoldingBuff[11]
#define OXYGEN_ON_INSTANT       HoldingBuff[12]
#define OXYGEN_OFF_INSTANT      HoldingBuff[13]
#define HEATER_ON_INSTANT       HoldingBuff[14]
#define HEATER_OFF_INSTANT      HoldingBuff[15]
#define FILTER_ON_INSTANT       HoldingBuff[16]
#define FILTER_OFF_INSTANT      HoldingBuff[17]





/*************Function Declaration Below******************************/
void init_BluetoothClassic(void);
void BL_TEST(void);
void recvWithStartEndMarkers(void);
void parseBluetoothData(void);
/*********************************************************************/

