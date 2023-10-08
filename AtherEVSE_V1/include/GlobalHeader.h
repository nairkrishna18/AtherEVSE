#include<Arduino.h> // included to identify  uint16_t..........


extern uint16_t gNeoPixelState;
extern uint8_t gStatusNeoPixel;

extern int8_t gMasterCardStatus;

extern int8_t gi8_RFID_STATUS;
extern uint16_t gBuzzerState;
extern int8_t gi8_MSTATE; // Main States of State Machine
extern int8_t gi8_NXT_MSTATE; // Previous Main States of State Machine for Wait State....

extern int8_t gi8_RFID_SCAN_ENABLE; // RFID Scan enable variable.........

extern int8_t PixelCounter;

extern float Energy;
extern float Power;
extern float Voltage;
extern float Current;
extern float Frequency;

extern boolean newData;



enum  // gNeoPixelState = NeoPixel States................
{ 
    NP_BLANK, 

    NP_ACW_WHITE_DIRCW,

    /*******Clock Wise Direction Animations***************/
    NP_ACF_RED_DIRCW, // NeoPixel Animation ColorFill Red Direction Clock Wise.....
    NP_ACF_GREEN_DIRCW,
    NP_ACF_BLUE_DIRCW,
    NP_ACF_YELLOW_DIRCW,
    NP_ACF_CYAN_DIRCW,
    NP_ACF_MAGENTA_DIRCW,
    NP_ACF_WHITE_DIRCW,
    NP_RFID_ERASING_EEPROM,
    NP_ACF_RED_DIRCW_10SEC, // Not used Yet
    NP_ACF_YELLOW_DIRCW_10SEC, // Not used Yet
    NP_ACF_YELLOW_DIRCW_2SEC, // used in Wait time for User Authentication........
    
    /*******Counter Clock Wise Direction Animations********/
    NP_ACF_RED_DIRCCW, // NeoPixel Animation ColorFill Red Direction Clock Wise.....
    NP_ACF_GREEN_DIRCCW,
    NP_ACF_BLUE_DIRCCW,
    NP_ACF_YELLOW_DIRCCW,
    NP_ACF_CYAN_DIRCCW,
    NP_ACF_MAGENTA_DIRCCW,
    NP_ACF_WHITE_DIRCCW,
    NP_ACF_RED_DIRCCW_10SEC, // Not USed Yet
    NP_ACF_YELLOW_DIRCCW_10SEC, // Not USed Yet
    NP_ACF_YELLOW_DIRCCW_2SEC, // used in Wait time for User Authentication........
    NP_ACF_GREEN_DIRCCW_2SEC, // used in Wait time for User Authentication........


    NP_MFRC_READER_ERROR,
    NP_RFID_WAIT_FOR_USER_CANCEL,
    NP_RFID_USER_CANCELLED,
    NP_RFID_WAIT_FOR_MASTER_CARD,

    NP_TRY_NEW_ANIMATION,

    NP_IDLE_STATE,
    NP_RFID_DENIED,
    NP_RFID_AUTHORIZED,
    NP_PREPARING_STATE,
    NP_PRECHARGE_STATE,
    NP_PROCHARGE_STATE,
    NP_FINCHARGE_STATE,
    NP_STPCHARGE_STATE,




    
    NP_RESET_PIXELS, // to clear all pixels and reset..
    
}; // anonymous enum (no name tag)

enum // Type of NeoPixel Animations..........
{    
    DIR_CW,
    DIR_CCW,
    DIR_VSPLIT_UP, // VERTICAL SPLIT...
    DIR_VSPLIT_DN, // VERTICAL SPLIT...

}; // anonymous enum (no name tag)

enum // gi8_RFID_STATUS = Various RFID States
{
    RFID_READER_ERROR = -10,

    RFID_NEW_CARD_DENIED = -2,
    RFID_MASTER_CARD_ABSENT = -1,    
    
    RFID_STATUS_UNKNOWN = 0,
    RFID_READER_PRESENT,
    RFID_MASTER_CARD_DETECTED,
    RFID_NEW_CARD_DETECTED,
    RFID_NEW_CARD_AUTHORIZED,    

};

enum // gi8_MSTATE =  Various Main States related to charger.....
{
    MSTATE_ERROR = -1,  // Error Condition..

    MSTATE_UNKNOWN = 0, // We dont know the state
    MSTATE_INIT,        // Initialization state, Mostly execute only once and move on
    MSTATE_IDLE,        // Charging waiting for any command for charging
    MSTATE_PREPARING,   // Charging command received Prepare for charging
    MSTATE_PRE_CHARGING,// Pre Charging , Waiting to connect Gun
    MSTATE_PRO_CHARGING,// Gun Connected and Charging current detected...
    MSTATE_STP_CHARGING,// Charging stopped by user using RFID card
    MSTATE_FIN_CHARGING,// Charging Finished by Ev SOC = 100%
    MSTATE_WAIT,        // Wait State.....
    
};

enum // __varChargeStates
{
    __noChargCurr = -1,
    __initState = 0,
    __rfidAuthIdle,
    __PrepState,
    __outRelayOn,
    __senChargCurr,
    __evFinCharging,
    __outRelayOff,
    __rfidAuthProg,
    __userStopCharging,
    __evFinChargingFinal,
    


};

