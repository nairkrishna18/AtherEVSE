#ifndef __BL_CLASSIC_CPP
    #define __BL_CLASSIC_CPP
    #include "BL_Classic.h"
#endif


#include "BluetoothSerial.h"
#include "GlobalHeader.h"

BluetoothSerial SerialBT;


unsigned int HoldingBuff[MAX_PARAM+1];
byte stateParse = 0;
//int iRxTotalTimeCount;
int slaveId;
int numByte,iLoadNumber;
int startAddress,iLoadStatus;
int functionCode;
int SubFunction;

byte bLoadManualbits; // used to manualy control load by using individual bits
int iRxTotalTimeCount, iRxHours, iRxMinutes, iData4; 

 // max length of received command is 20 chrs not including the \0 character

char receivedChars[NUM_CHARS];
char transmitChars[NUM_CHARS];
boolean newData = false;
boolean newData1 = false;

boolean neo1 = false;

/******************************************************************************************/

void init_BluetoothClassic(void)
{
     SerialBT.begin("ATHER_EVSE"); //Bluetooth device name
     log_i("Bluetooth Device Created & Initialized..........");
     log_i("Search & Connect To < ATHER_EVSE >");

    for(int i=0; i<18; i++)
    {
        HoldingBuff[i] = i+1;        
    }

}


void BL_TEST(void)
{
    const uint8_t Buf[] = " Aarav Krishna Nair\n";

     SerialBT.write(Buf,sizeof(Buf));

    if (Serial.available()) {
        SerialBT.write(Serial.read());
    }
    if (SerialBT.available()) {
        Serial.write(SerialBT.read());
    }
}


void recvWithStartEndMarkers(void)
{
    static bool recvInProgress = false;
    static unsigned char ndx = 0;
    char startMarker = '[';
    char endMarker = ']'; 
    char _ReceivedChars;

    // if (SerialBT.available() > 0) // bluetooth data is available to receive
    if (NEW_BL_DATA_AVAILABLE) // bluetooth data is available to receive
    {
        _ReceivedChars = SerialBT.read(); // read a byte
        log_i("_ReceivedChars = %c",_ReceivedChars);
        if (recvInProgress == true) // received start marker ready to receive data
        {
            if (_ReceivedChars != endMarker) // untill end marker is received ]
            {
                receivedChars[ndx] = _ReceivedChars; // store data in buffer                
                ndx++; // update ndx
                if (ndx >= NUM_CHARS) 
                { 
                    ndx = NUM_CHARS - 1; 
                }
            }
            else // received end marker ]
            {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false; 
                ndx = 0;
                newData = true; // ready to parse data
                stateParse = STATE_SID; // 0 Use this to state machine initialization......
            }
        }
        else 
        if (_ReceivedChars == startMarker) // untill we receive start marker [ 
        { 
            recvInProgress = true; 
        }
    }
}

unsigned int convertToNumber1( byte startPos, byte NumByte)
 {
    unsigned int tmp = 0;
    if(NumByte == 1)
    {
      tmp = (receivedChars[startPos]-48);    
    }
    else
    if(NumByte == 2)
    {
        tmp = (receivedChars[startPos]-48)          * 10  ;
        tmp = tmp + (receivedChars[startPos+1]-48)        ;
    }
    else
    if(NumByte == 3)
    {
      tmp = (receivedChars[startPos]-48)          * 100 ;
      tmp = tmp + (receivedChars[startPos+1]-48)  * 10  ;
      tmp = tmp + receivedChars[startPos+2]-48          ;         
    }
    else
    if(NumByte == 4)
    {
      tmp =       (receivedChars[startPos  ]-48)  * 1000;
      tmp = tmp + (receivedChars[startPos+1]-48)  * 100 ;
      tmp = tmp + (receivedChars[startPos+2]-48)  * 10  ;  
      tmp = tmp + (receivedChars[startPos+3]-48)        ;         
    }       
    return tmp;
  }

char convertToAscii(int Data, int pos, char numDigits)
{
char Ascii;
if(numDigits == 1)
{
    Ascii   = Data + 48;
}
else
if(numDigits == 2)
{
        
        if(pos == 0) Ascii   = (Data/10)+48;
    else  if(pos == 1) Ascii   = ((Data%10))+48;    
}
else
if(numDigits == 3)
{
        if(pos == 0) Ascii   = (Data/100)+48;
    else  if(pos == 1) Ascii   = ((Data%100)/10)+48;
    else  if(pos == 2) Ascii   = ((Data%10))+48;    
}
else
if(numDigits == 4)
{
        if(pos == 0) Ascii   = (Data/1000)+48;
    else  if(pos == 1) Ascii   = ((Data%1000)/100)+48;
    else  if(pos == 2) Ascii   = ((Data%100)/10)+48;    
    else  if(pos == 3) Ascii   = ((Data%10))+48;    
}        
return Ascii;            
}
///////////////////////// STATE MACHINE FUNCTIONS//////////////////////////////////////////////   
// Slave ID --> SID
/////////////////////////////////////////////////SLAVE ID////////////////////////////////////////
void funcSTATE_SID(void) // 0
{    
    slaveId = convertToNumber1(0,2); // 2byte slave id from location 0
    
    log_i("slaveId = %d", slaveId);
    
    if((slaveId == SLAVE_ID)||(slaveId == SLAVE_BID))// 351 S1 or 350 S0 FC broadcast slave id
    {
        stateParse = STATE_FC; // 1 
    }
    else // Not received S so ignore
    {                
        log_e("Error Wrong Slaveid --> %d",slaveId);                
        slaveId = 0xff; // reset slave Id
        stateParse = STATE_END; // 15
    }
}

///////////////////////////////////////////////////////FUNCTION CODE//////////////////////////////////////////////////////
void funcSTATE_FC(void) // 1 FUNCTION CODE
{
    functionCode = convertToNumber1(2,2); // 2 byte funciton code from location 2     

    log_i("Function Code = %d", functionCode);

    switch(functionCode)
    {
        case F1_READ_HR:
            transmitChars[0] = receivedChars[0]; // slave id S
            transmitChars[1] = receivedChars[1]; // slave id Number
            transmitChars[2] = receivedChars[2]; // Function code F
            transmitChars[3] = receivedChars[3]; // Function code Number    
            stateParse = STATE_NB; // 2  
        break;
        
        case F2_WRITE_HR:
            transmitChars[0] = receivedChars[0]; // slave id S
            transmitChars[1] = receivedChars[1]; // slave id Number
            transmitChars[2] = receivedChars[2]; // Function code F
            transmitChars[3] = receivedChars[3]; // Function code Number    
            stateParse = STATE_NB; // 2      
        break;
        
        case F5_MANUAL_CONFIG:
            transmitChars[0] = receivedChars[0]; // slave id S
            transmitChars[1] = receivedChars[1]; // slave id Number
            transmitChars[2] = receivedChars[2]; // Function code F
            transmitChars[3] = receivedChars[3]; // Function code Number    
            stateParse = STATE_NB; // 2                      
        break;

        case F4_RD_RTC:
            transmitChars[0] = receivedChars[0]; // slave id S
            transmitChars[1] = receivedChars[1]; // slave id Number
            transmitChars[2] = receivedChars[2]; // Function code F
            transmitChars[3] = receivedChars[3]; // Function code Number                    
            transmitChars[4] = receivedChars[4]; // Sub function number of bytes hi
            transmitChars[5] = receivedChars[5]; // Sub function number of bytes lo   
            stateParse = STATE_SF; // 5     
        break;
        
        case F7_NEOPIXEL:
            transmitChars[0] = receivedChars[0]; // slave id S
            transmitChars[1] = receivedChars[1]; // slave id Number
            transmitChars[2] = receivedChars[2]; // Function code F
            transmitChars[3] = receivedChars[3]; // Function code Number                     
            transmitChars[4] = receivedChars[4]; // Sub function number of bytes hi
            transmitChars[5] = receivedChars[5]; // Sub function number of bytes lo  
            stateParse = STATE_SF; // 5    
        break;

        case F3_WR_RTC:
            stateParse = STATE_SA; // 3   
        break;
        
        case F6_AUTO_CONFIG:
            stateParse = STATE_SA; // 3   
        break;

        default :
            log_e("Error Function code --> %d",functionCode);
            stateParse = STATE_END;
        break;     
        
    }       
}
//////////////////////////////////////////////NUMBER OF BYTES//////////////////////////////////////////////
void funcSTATE_NB(void)
{
    switch(functionCode)
    {
        case F1_READ_HR:
            numByte = convertToNumber1(4,2); // 2 byte Number of Bytes from location 4
            log_i("Number Of Bytes = %d", numByte);
            // if((numByte < MAX_DATA)&&(numByte > 0)) // F1 || F2 
            if((numByte <= MAX_PARAM)&&(numByte > 0)) // F1 || F2 
            {            
                transmitChars[4] = receivedChars[4]; // number of bytes hi
                transmitChars[5] = receivedChars[5]; // number of bytes lo         
                stateParse = STATE_SA; // 3
            }
            else
            {
                log_e("Error in Number Of bytes --> %d", numByte);
                stateParse = STATE_END;
            }
        break;
        
        case F2_WRITE_HR:
            numByte = convertToNumber1(4,2); // 2 byte Number of Bytes from location 4
            log_i("Number Of Bytes = %d", numByte);
            // if((numByte < MAX_DATA)&&(numByte > 0)) // F1 || F2 
            if((numByte <= MAX_PARAM)&&(numByte > 0)) // F1 || F2 
            {            
                transmitChars[4] = receivedChars[4]; // number of bytes hi
                transmitChars[5] = receivedChars[5]; // number of bytes lo         
                stateParse = STATE_SA; // 3
            }
            else
            {
                log_e("Error in Number Of bytes --> %d", numByte);
                stateParse = STATE_END;
            }
        break;

        case F6_AUTO_CONFIG:
            numByte = convertToNumber1(8,1); // 2 byte Number of Bytes from location 8
            log_i("Number Of Bytes(F6_AUTO_CONFIG) = %d",numByte);
            stateParse = STATE_SF; // to store data as per sub function of load
        break;

        case F5_MANUAL_CONFIG:
            iLoadNumber = convertToNumber1(4,2); // 2 byte Number of Bytes from location 4      
            transmitChars[4] = receivedChars[4]; // Load Number L
            transmitChars[5] = receivedChars[5]; // Load Number ID               
            log_i("Load Number (F5_MANUAL_CONFIG) = %d",iLoadNumber);
            stateParse = STATE_SA;
        break;

        default :
        log_e("Recheck Function code --> %d",functionCode);
        stateParse = STATE_END;   
        break;
    }
   
}

void funcSTATE_SA(void)
{
    switch(functionCode)
    {
        case F1_READ_HR:
        uint8_t c1;
        startAddress = convertToNumber1(6,2); // 2 byte starting address from location 6
        log_i("Starting Address Of Holding Reg = %d",startAddress);

        if((startAddress+numByte) > MAX_DATA) // 5+9 = 14 > 10 error
        {
            stateParse = STATE_END;            
            log_e("Error Data Mismatch: SA+NM(%d) > MAX_DATA(%d)",(startAddress+numByte), MAX_DATA);            
        }
        else
        if(startAddress < MAX_DATA) //ok within range
        {
            transmitChars[6]  = receivedChars[6]; // starting address hi
            transmitChars[7]  = receivedChars[7]; // starting address hi
            
            for(c1 = 0; c1<numByte; c1++) // splitting single int parameter to 4 ascii values
            {
                byte cVal1;

                cVal1 = c1 * 4;
                
                transmitChars[cVal1+8]   = convertToAscii(HoldingBuff[c1], 0, 4);
                transmitChars[cVal1+9]   = convertToAscii(HoldingBuff[c1], 1, 4);
                transmitChars[cVal1+10]  = convertToAscii(HoldingBuff[c1], 2, 4);
                transmitChars[cVal1+11]  = convertToAscii(HoldingBuff[c1], 3, 4);                
            }
            
            log_i("Final Transmit Message = %s", transmitChars);
        }
        else
        {
            log_e("-------Error Data Mismatch: SA+NM(%d) > MAX_DATA(%d)",(startAddress+numByte), MAX_DATA);            
        }
        stateParse = STATE_CS;
        break;
        
        case F2_WRITE_HR:

        break;
        
        case F3_WR_RTC:

        break;
        
        case F5_MANUAL_CONFIG:

        break;

        case F6_AUTO_CONFIG:

        break;

        default:
            log_e("Recheck Function code --> %d",functionCode);
            stateParse = STATE_END;
        break;
    }

}

///////////////////////////////////////CHECKSUM//////////////////////////////////////
void funcSTATE_CS(void)
{
    log_i("stateParse = STATE_CS");    
    stateParse = STATE_END;
}

  ////////////////////////////////////////////ENDSTATE/////////////////////////////////
  void funcSTATE_END(void)
  {
    const uint8_t StartMarker[] = "<";
    const uint8_t EndMarker[] = ">";
    
    if(slaveId != SLAVE_BID) // not for broadcast messages
    {
      SerialBT.write(StartMarker,1);
      SerialBT.write((const uint8_t*)transmitChars,sizeof(transmitChars));  
      SerialBT.write(EndMarker,1);
    }

    for(char cval1 = 0; cval1<=MAX_DATA; cval1++) // clear TX and RX buffers
    {
      transmitChars[cval1] = 0;
      receivedChars[cval1] = 0;
    }
    
    newData       = false;
    newData1      = true;
    slaveId       = 0xFF;
    functionCode  = 0xFF;
    startAddress  = 0xFF;
    numByte       = 0xFF;
    SubFunction   = 0xFF;

    /********************DISPLAY FINAL HOLDING REGISTER************************/
    // displayHoldingBuffer(MAX_PARAM);
    // copyHoldingBuffer2EEPROM(MAX_PARAM);
    log_i("Above Two functions Pending.........");      
    
    /**************************************************************************/
    
    log_i("stateParse = STATE_END");       
  }



void parseBluetoothData(void)
{    
    int tmp;

    switch (stateParse)
    {        
        case STATE_SID: // 0        
          funcSTATE_SID();        
        break;

        case STATE_FC: // 1
          funcSTATE_FC();  
        break;

        case STATE_NB: // 2        
          funcSTATE_NB();
        break;

        case STATE_SA: // 3  
          funcSTATE_SA();
        break;

        case STATE_CS: // 4
          funcSTATE_CS();
        break;

        case STATE_SF: // 5
        //   funcSTATE_SF();
        break;

        case STATE_END: // 15        
          funcSTATE_END();
        break;

        default :

        break;
    }

}
