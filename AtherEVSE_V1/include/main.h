// #include <Arduino.h>
// /*************************Format for coms data structure***************************************/
// struct ComsDataStructure
// {
    
//     int8_t  Status;
// //   String   sg_var1; // New firmware version after update
  
//    #if 1 // Use along with sError for Charger and Energy meter related error
//     union
//     {
//         unsigned int u16_var2, u16_var3, u16_var4, u16_var5; // to clear or set entire field
//         struct
//         {
//             unsigned Bitb0     : 1 ;
//             unsigned Bitb1     : 1 ;
//             unsigned Bitb2     : 1 ;
//             unsigned Bitb3     : 1 ;
//             unsigned Bitb4     : 1 ;
//             unsigned Bitb5     : 1 ;
//             unsigned Bitb6     : 1 ;
//             unsigned Bitb7     : 1 ;
//             unsigned Bitb8     : 1 ;
//             unsigned Bitb9     : 1 ;
//             unsigned Bitb10    : 1 ;
//             unsigned Bitb11    : 1 ;
//             unsigned Bitb12    : 1 ;
//             unsigned Bitb13    : 1 ;
//             unsigned Bitb14    : 1 ;
//             unsigned Bitb15    : 1 ;
//         };
//     }b_Var1, b_Var2, b_Var3, b_Var4;
//     #endif

// }sRFID, sNeoPixel, sEnergyMeter,sChargerStates;


// /*************
//  * -1 _MFRC Reader Absent/ Not Connected
//  * 0  _MFRC Reader Status Unknown
//  * 1  _MFRC Reader Status Ok
//  * 
// */
// #define gi8_RFID_STATUS    sRFID.Status     // Negative Numbers are different Errors/Warnings/Faults Positive Numbers are Status........

// #define gi8_NP_STATUS      sNeoPixel.Status 
// #define gi8_EM_STATUS      sEnergyMeter.Status 


