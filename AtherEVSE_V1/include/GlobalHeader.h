#include<Arduino.h> // included to identify  uint16_t..........


extern uint16_t gNeoPixelState;
extern uint8_t gStatusNeoPixel;

extern int8_t gMasterCardStatus;

extern int8_t gi8_RFID_STATUS;

enum  // NeoPixel States................
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
    
    NP_RESET_PIXELS, // to clear all pixels and reset..
    
}; // anonymous enum (no name tag)

enum // NeoPixel Animations
{    
    DIR_CW,
    DIR_CCW,
    DIR_VSPLIT_UP, // VERTICAL SPLIT...
    DIR_VSPLIT_DN, // VERTICAL SPLIT...

}; // anonymous enum (no name tag)

enum // Various RFID States
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