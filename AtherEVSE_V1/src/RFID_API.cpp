


#include "RFID_API.h"
#include <EEPROM.h>     // We are going to read and write PICC's UIDs from/to EEPROM
#include "GlobalHeader.h"
#include "main.h"




//Constants
#define EEPROM_SIZE 50 //12

#define wipeB       2    // Button pin for WipeMode

bool programMode = false;  // initialize programming mode to false

uint8_t successRead;    // Variable integer to keep if we have Successful Read from Reader
int8_t gMasterCardStatus = 0;    // Variable integer to keep if we have Successful Read from Reader
int8_t gi8_RFID_STATUS = RFID_STATUS_UNKNOWN;


byte storedCard[4];   // Stores an ID read from EEPROM
byte readCard[4];   // Stores scanned ID read from RFID Module
byte masterCard[4];   // Stores master card's ID read from EEPROM

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

uint8_t ShowReaderDetails(void) ; 
void init_Rfid_WipeCode(void);
void initCheckUpdateMasterCardStatus(void);



uint8_t initRFID(void)
{    
    // gNeoPixelState = NP_BLANK;
    uint8_t retStatus = 0;
    gi8_RFID_STATUS = RFID_STATUS_UNKNOWN;
    //Init EEPROM
    EEPROM.begin(EEPROM_SIZE);
    log_i("Initialized and Configured EEPROM, Size --> %d Bytes", EEPROM_SIZE);

    // put your setup code here, to run once:
    pinMode(wipeB, INPUT_PULLUP);   // Enable pin's pull up resistor

    log_i("Initializing MFRC522 Reader....");

    SPI.begin();			// Init SPI bus
    mfrc522.PCD_Init();		// Init MFRC522
    delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme    
    
    #if 0 //If you set Antenna Gain to Max it will increase reading distance    
      mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
    #endif

    #if 1
      retStatus = ShowReaderDetails(); // will be high only if valid reader is connected and communicated...........
      log_i("RFID Reader Status = %d", retStatus);      
      if(!retStatus)
      {
        gi8_RFID_STATUS = RFID_READER_ERROR;
        log_e("gi8_RFID_STATUS = RFID_READER_ERROR(%d)", gi8_RFID_STATUS);
      }
      
    #else
      mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
      log_i("Scan PICC to see UID, SAK, type, and data blocks...");
    #endif

    if(retStatus) // if Reader is Ok then call below functions
    {
      gi8_RFID_STATUS = RFID_READER_PRESENT;
      log_i("gi8_RFID_STATUS = RFID_READER_PRESENT(%d)", gi8_RFID_STATUS);
      initCheckUpdateMasterCardStatus();      
    }
  return retStatus;
}




// return 1 if sucess...................
// return 0 if failed to recognize reader.........
uint8_t ShowReaderDetails(void) 
{
  uint8_t retStatus = 0;
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  log_i("MFRC522 Software Version: 0x%x ", v);

  if (v == 0x91)
  {
    log_i(" Version = 1.0 ");
    retStatus = 1;
  }
  else if (v == 0x92)
  {
    log_i(" Version = 2.0 ");
    retStatus = 1;
  }
  else
  {
      log_e("Unknown Verison! --> Probably a chinese clone?");
  }
  
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) 
  {
    log_w("WARNING: Communication failure, is the MFRC522 properly connected?");
    log_e("SYSTEM HALTED: Check connections.");
    // Visualize system is halted
   
   // Note: Disabled in FreeRTOS, we cannot hold it here...................
   // while (true); // do not go further
  }
  return retStatus;
}

bool monitorWipeButton(uint32_t interval) 
{
  uint32_t now = (uint32_t)millis();
  while ((uint32_t)millis() - now < interval)  
  {
    // check on every half a second
    if (((uint32_t)millis() % 500) == 0) 
    {
      if (digitalRead(wipeB) != LOW)
        return false;
    }
  }
  return true;
}

void init_Rfid_WipeCode(void)
{
  #if 1
  log_i("Check Wipe Button Status"); // Show some feedback that the wipe button did not pressed for 15 seconds    
  //Wipe Code - If the Button (wipeB) Pressed while setup run (powered on) it wipes EEPROM
  if (digitalRead(wipeB) == LOW) 
  {  // when button pressed pin should get low, button connected to ground
    
    log_i("Wipe Button Pressed");
    log_i("You have 10 seconds to Cancel");
    log_i("This will be remove all records and cannot be undone");
    
    gNeoPixelState = NP_RFID_WAIT_FOR_USER_CANCEL;   // Yellow Color Fill wait for user to cancel Clearing Master Data......... 
    
    // Blocking delay used in init loop..........
    vTaskDelay(pdMS_TO_TICKS(10000)); // delay of 10s is created.
    
    if (digitalRead(wipeB) == LOW) 
    {    // If button still be pressed, wipe EEPROM
      log_i("Start Wiping EEPROM............");

      gNeoPixelState = NP_RFID_ERASING_EEPROM; // BLUE Color Chase for 2seconds....
     
      
      for (uint16_t x = 0; x < EEPROM.length(); x = x + 1) 
      {    //Loop end of EEPROM address
        if (EEPROM.read(x) == 0) 
        {              //If EEPROM address 0
          // do nothing, already clear, go to the next address in order to save time and reduce writes to EEPROM
        }
        else 
        {
          EEPROM.write(x, 0);       // if not write 0 to clear, it takes 3.3mS
        }
      }
      vTaskDelay(pdMS_TO_TICKS(2000)); // delay of 10s is created.
      log_i("EEPROM Successfully Wiped.........");
      EEPROM.commit();
    }
    else 
    {
      gNeoPixelState = NP_RFID_USER_CANCELLED; // COLOR FILL CYAN......
      // gNeoPixelState = NP_TRY_NEW_ANIMATION;

      vTaskDelay(pdMS_TO_TICKS(2000)); // delay of 10s is created.
      log_i("Wiping Cancelled By User......"); // Show some feedback that the wipe button did not pressed for 15 seconds      
    }
  } /// End of Wipe Routine
  else
  {
    log_i("Wipe Button Not Pressed...."); // Show some feedback that the wipe button did not pressed for 15 seconds  
  }
  #endif
}

uint8_t getID() 
{
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {   //Since a PICC placed get Serial and continue
    return 0;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  Serial.println("\n---------------PRINT UID-------------------");
  Serial.println(F("Scanned PICC's UID:"));
  for ( uint8_t i = 0; i < 4; i++) 
  {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println("");
  Serial.println("------------------------------------------\n");
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}

void initCheckUpdateMasterCardStatus(void)
{
  // Check if master card defined, if not let user choose a master card
  // This also useful to just redefine the Master Card
  // You can keep other EEPROM records just write other than 143 to EEPROM address 1
  // EEPROM address 1 should hold magical number which is '143'

  int MagicNumber1 = EEPROM.read(1);
  log_i("MagicNumber1(EE) = %d", MagicNumber1);
  
  if (EEPROM.read(1) != 143) // if master card is not defined then.......
  {
    delay(4000);
    gNeoPixelState = NP_RFID_WAIT_FOR_MASTER_CARD;
    log_i("No Master Card Defined");
    log_i("Scan A PICC to Define as Master Card");
    gMasterCardStatus = -1; // Master Card not defined......
    log_i("gMasterCardStatus(Master Card Not Defined) = %d", gMasterCardStatus);

    do 
    {
      // gNeoPixelState = NP_ACF_RED_DIRCCW;
      // gNeoPixelState = NP_TRY_NEW_ANIMATION;

      successRead = getID();            // sets successRead to 1 when we get read from reader otherwise 0
//      digitalWrite(blueLed, LED_ON);    // Visualize Master Card need to be defined
      delay(200);
//      digitalWrite(blueLed, LED_OFF);
      delay(200);
    }
    while (!successRead);                  // Program will not go further while you not get a successful read

    gNeoPixelState = NP_ACF_GREEN_DIRCW;

    for ( uint8_t j = 0; j < 4; j++ ) 
    {        // Loop 4 times
      EEPROM.write( 2 + j, readCard[j] );  // Write scanned PICC's UID to EEPROM, start from address 3
      delay(10);
      
    }
    EEPROM.write(1, 143);                  // Write to EEPROM we defined Master Card.
    delay(100);
    EEPROM.commit();
    
    log_i("Master Card Defined");
    int MagicNumber2 = EEPROM.read(20);
    log_i("MagicNumber2(EE) = %d", MagicNumber2);
  } // Check Master Card Defined Status ends here..........

  Serial.println("\n--------------PRINT UID--------------");
  Serial.print("Master Card's UID : ");
  for ( uint8_t i = 0; i < 4; i++ ) // Read Master Card's UID from EEPROM
  {          
    masterCard[i] = EEPROM.read(2 + i);    // Write it to masterCard
    Serial.print(masterCard[i], HEX);
  }
  Serial.println(" ");
  Serial.println("------------------------------------\n");
  gMasterCardStatus = 1;
  log_i("gMasterCardStatus(Master Card Defined) = %d", gMasterCardStatus);
  log_i("Everything is ready");
  log_i("Waiting PICCs to be scanned");
  
  // gNeoPixelState = NP_ACW_WHITE_DIRCW;
  // gNeoPixelState = NP_TRY_NEW_ANIMATION;


}

void funcCheckUpdateMasterCardStatus(void)
{
  static uint8_t WipeButtonStatus;
  //  do
  {

    if(WipeButtonStatus == 0) 
    {
      successRead = getID();  // sets successRead to 1 when we get read from reader otherwise 0
    }

    if(successRead == 1) // Master Card Scanned 
    {     
      WipeButtonStatus = 0; // it can be wiped again.........
      log_i("WipeButtonStatus(successRead == 1) = %d",WipeButtonStatus);
    }
    // When device is in use if wipe button pressed for 10 seconds initialize Master Card wiping
    if ((WipeButtonStatus == 0)&&(digitalRead(wipeB) == LOW)) // button pressed
    { // Check if button is pressed
      // Visualize normal operation is iterrupted by pressing wipe button Red is like more Warning to user
      // digitalWrite(redLed, LED_ON);  // Make sure led is off
      // digitalWrite(greenLed, LED_OFF);  // Make sure led is off
      // digitalWrite(blueLed, LED_OFF); // Make sure led is off
      // Give some feedback
      WipeButtonStatus = 1; // User Initialized  Wiping...........
      log_i("WipeButtonStatus = %d",WipeButtonStatus);
      log_i("Wipe Button Pressed");
      log_i("Master Card will be Erased! in 10 seconds");
      gNeoPixelState = NP_RFID_WAIT_FOR_USER_CANCEL;   // Yellow Color Fill wait for user to cancel Clearing Master Data......... 
      /////// Blocking delay used in init loop..........
      vTaskDelay(pdMS_TO_TICKS(10000)); // delay of 10s is created.
      
      
      if (digitalRead(wipeB) == LOW) 
      {    // If button still be pressed, wipe EEPROM        
        WipeButtonStatus = 2; // user confirmed wiping.........
        EEPROM.write(1, 0);                  // Reset Magic Number.
        
        log_i("Master Card Erased from device");
        log_i("Please Reset or Release Wipe Button to Re-Program Master Card");
        EEPROM.commit();
        log_i("WipeButtonStatus = %d",WipeButtonStatus);
        
      }
      else
      {
        WipeButtonStatus = 3; // user cancelled wiping
        log_i("User Released Wipe Button --> Wiping Cancelled"); // Show some feedback that the wipe button did not pressed for 15 seconds   
        log_i("WipeButtonStatus = %d",WipeButtonStatus);    
      }
      
    }
    else
    if((digitalRead(wipeB) == HIGH) &&(WipeButtonStatus == 3))
    {
      log_i("Master Card Erase Cancelled --> Wiping Cancelled"); // Show some feedback that the wipe button did not pressed for 15 seconds     
       gNeoPixelState = NP_RFID_USER_CANCELLED; // COLOR FILL CYAN......
       vTaskDelay(pdMS_TO_TICKS(2000)); // delay of 10s is created.
       WipeButtonStatus = 0;
       log_i("WipeButtonStatus = %d",WipeButtonStatus);
    }
    else
    if((digitalRead(wipeB) == LOW) &&(WipeButtonStatus == 2))
    {
      log_i("Please Release Wipe Button to Reset ESP32....."); // Show some feedback that the wipe button did not pressed for 15 seconds   
      WipeButtonStatus = 4;  
      log_i("WipeButtonStatus = %d",WipeButtonStatus);
    }
    else
    if((WipeButtonStatus == 4)&&(digitalRead(wipeB) == HIGH))
    {
      log_i("Restarting ESP32......");
      vTaskDelay(pdMS_TO_TICKS(2000)); // delay of 10s is created.
      ESP. restart();
    }

    if (programMode) 
    {

      // cycleLeds();              // Program Mode cycles through Red Green Blue waiting to read a new card
      // log_w("Add Code for cycle Led using NeoPixel.......");
    }
    else 
    {
      // normalModeOn();     // Normal mode, blue Power LED is on, all others are off
      // log_w("Add Code for Normal Mode Led using NeoPixel.......");
    }
    
  }
  // while (!successRead);   //the program will not go further while you are not getting a successful read
      
      
}

///////////////////////////////////////// Check Bytes   ///////////////////////////////////
bool checkTwo ( byte a[], byte b[] ) 
{   
  for ( uint8_t k = 0; k < 4; k++ ) 
  {   // Loop 4 times
    if ( a[k] != b[k] ) 
    {     // IF a != b then false, because: one fails, all fail
       return false;
    }
  }
  return true;  
}
////////////////////// Check readCard IF is masterCard   ///////////////////////////////////
// Check to see if the ID passed is the master programing card
bool isMaster( byte test[] ) 
{
  return checkTwo(test, masterCard);
}

//////////////////////////////////////// Read an ID from EEPROM //////////////////////////////
void readID( uint8_t number ) 
{
  uint8_t start = (number * 4 ) + 2;    // Figure out starting position
  for ( uint8_t i = 0; i < 4; i++ ) 
  {     // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i);   // Assign values read from EEPROM to array
  }
}


///////////////////////////////////////// Find ID From EEPROM   ///////////////////////////////////
bool findID( byte find[] ) 
{
  uint8_t count = EEPROM.read(0);     // Read the first Byte of EEPROM that
  for ( uint8_t i = 1; i < count; i++ ) 
  {    // Loop once for each EEPROM entry
    readID(i);          // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) 
    {   // Check to see if the storedCard read from EEPROM
      return true;
    }
    else 
    {    // If not, return false
      
    }
  }
  return false;
}

//////////////////////////////////////// Success Remove UID From EEPROM  ///////////////////////////////////
// Flashes the blue LED 3 times to indicate a success delete to EEPROM
void successDelete() 
{
  // digitalWrite(blueLed, LED_OFF);   // Make sure blue LED is off
  // digitalWrite(redLed, LED_OFF);  // Make sure red LED is off
  // digitalWrite(greenLed, LED_OFF);  // Make sure green LED is off
  // delay(200);
  // digitalWrite(blueLed, LED_ON);  // Make sure blue LED is on
  // delay(200);
  // digitalWrite(blueLed, LED_OFF);   // Make sure blue LED is off
  // delay(200);
  // digitalWrite(blueLed, LED_ON);  // Make sure blue LED is on
  // delay(200);
  // digitalWrite(blueLed, LED_OFF);   // Make sure blue LED is off
  // delay(200);
  // digitalWrite(blueLed, LED_ON);  // Make sure blue LED is on
  // delay(200);
}


///////////////////////////////////////// Write Failed to EEPROM   ///////////////////////////////////
// Flashes the red LED 3 times to indicate a failed write to EEPROM
void failedWrite() 
{
  // digitalWrite(blueLed, LED_OFF);   // Make sure blue LED is off
  // digitalWrite(redLed, LED_OFF);  // Make sure red LED is off
  // digitalWrite(greenLed, LED_OFF);  // Make sure green LED is off
  // delay(200);
  // digitalWrite(redLed, LED_ON);   // Make sure red LED is on
  // delay(200);
  // digitalWrite(redLed, LED_OFF);  // Make sure red LED is off
  // delay(200);
  // digitalWrite(redLed, LED_ON);   // Make sure red LED is on
  // delay(200);
  // digitalWrite(redLed, LED_OFF);  // Make sure red LED is off
  // delay(200);
  // digitalWrite(redLed, LED_ON);   // Make sure red LED is on
  // delay(200);
}
///////////////////////////////////////// Write Success to EEPROM   ///////////////////////////////////
// Flashes the green LED 3 times to indicate a successful write to EEPROM
void successWrite() 
{
  // digitalWrite(blueLed, LED_OFF);   // Make sure blue LED is off
  // digitalWrite(redLed, LED_OFF);  // Make sure red LED is off
  // digitalWrite(greenLed, LED_OFF);  // Make sure green LED is on
  // delay(200);
  // digitalWrite(greenLed, LED_ON);   // Make sure green LED is on
  // delay(200);
  // digitalWrite(greenLed, LED_OFF);  // Make sure green LED is off
  // delay(200);
  // digitalWrite(greenLed, LED_ON);   // Make sure green LED is on
  // delay(200);
  // digitalWrite(greenLed, LED_OFF);  // Make sure green LED is off
  // delay(200);
  // digitalWrite(greenLed, LED_ON);   // Make sure green LED is on
  // delay(200);
}
///////////////////////////////////////// Find Slot   ///////////////////////////////////
uint8_t findIDSLOT( byte find[] ) 
{
  uint8_t count = EEPROM.read(0);       // Read the first Byte of EEPROM that
  for ( uint8_t i = 1; i <= count; i++ ) 
  {    // Loop once for each EEPROM entry
    readID(i);                // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) 
    {   // Check to see if the storedCard read from EEPROM
      // is the same as the find[] ID card passed
      return i;         // The slot number of the card
    }
  }
}

///////////////////////////////////////// Remove ID from EEPROM   ///////////////////////////////////
void deleteID( byte a[] ) 
{
  if ( !findID( a ) ) 
  {     // Before we delete from the EEPROM, check to see if we have this card!
    failedWrite();      // If not
    log_i("Failed! There is something wrong with ID or bad EEPROM");
  }
  else 
  {
    uint8_t num = EEPROM.read(0);   // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t slot;       // Figure out the slot number of the card
    uint8_t start;      // = ( num * 4 ) + 6; // Figure out where the next slot starts
    uint8_t looping;    // The number of times the loop repeats
    uint8_t j;
    uint8_t count = EEPROM.read(0); // Read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT( a );   // Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--;      // Decrement the counter by one
    EEPROM.write( 0, num );   // Write the new count to the counter
    for ( j = 0; j < looping; j++ ) 
    {         // Loop the card shift times
      EEPROM.write( start + j, EEPROM.read(start + 4 + j));   // Shift the array values to 4 places earlier in the EEPROM
    }
    for ( uint8_t k = 0; k < 4; k++ ) 
    {         // Shifting loop
      EEPROM.write( start + j + k, 0);
    }
    successDelete();
    log_i("Succesfully removed ID record from EEPROM");
    EEPROM.commit();
  }
}
///////////////////////////////////////// Add ID to EEPROM   ///////////////////////////////////
void writeID( byte a[] ) 
{
  if ( !findID( a ) ) 
  {     // Before we write to the EEPROM, check to see if we have seen this card before!
    uint8_t num = EEPROM.read(0);     // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t start = ( num * 4 ) + 6;  // Figure out where the next slot starts
    num++;                // Increment the counter by one
    EEPROM.write( 0, num );     // Write the new count to the counter
    for ( uint8_t j = 0; j < 4; j++ ) 
    {   // Loop 4 times
      EEPROM.write( start + j, a[j] );  // Write the array values to EEPROM in the right position
    }
    successWrite();
    log_i("Succesfully added ID record to EEPROM");
    EEPROM.commit();
  }
  else 
  {
    failedWrite();
    log_i("Failed! There is something wrong with ID or bad EEPROM");
  }
}

/////////////////////////////////////////  Access Granted    ///////////////////////////////////
void granted ( uint16_t setDelay) 
{
  // digitalWrite(blueLed, LED_OFF);   // Turn off blue LED
  // digitalWrite(redLed, LED_OFF);  // Turn off red LED
  // digitalWrite(greenLed, LED_ON);   // Turn on green LED
//  digitalWrite(relay, LOW);     // Unlock door!
log_i("Unlocked Door!......");
  
  delay(setDelay);          // Hold door lock open for given seconds
//  digitalWrite(relay, HIGH);    // Relock door
  log_i("Relocked Door!......");
  delay(1000);            // Hold green LED on for a second
}

///////////////////////////////////////// Access Denied  ///////////////////////////////////
void denied() 
{
  // digitalWrite(greenLed, LED_OFF);  // Make sure green LED is off
  // digitalWrite(blueLed, LED_OFF);   // Make sure blue LED is off
  // digitalWrite(redLed, LED_ON);   // Turn on red LED
  delay(1000);
}

void funcRFID_ProgramAndNormalMode(uint8_t __successRead)
{
  if(__successRead)
  {
    if (programMode) 
    {
      if ( isMaster(readCard) ) 
      { //When in program mode check First If master card scanned again to exit program mode
        log_i("Master Card Scanned");
        log_i("Exiting Program Mode");
        log_i("-----------------------------");
        programMode = false;
        return;
      }
      else 
      {
        if ( findID(readCard) ) 
        { // If scanned card is known delete it
          log_i("I know this PICC, removing...");
          deleteID(readCard);
          log_i("-------------------------------------------");
          log_i("Scan a PICC to ADD or REMOVE to EEPROM");
        }
        else 
        {                    // If scanned card is not known add it
          log_i("I do not know this PICC, adding...");
          writeID(readCard);
          log_i("-------------------------------------------");
          log_i("Scan a PICC to ADD or REMOVE to EEPROM");
        }
      }
    }
    else //Normal Mode.........
    {
      if ( isMaster(readCard)) 
      {    // If scanned card's ID matches Master Card's ID - enter program mode
        programMode = true;
        gi8_RFID_STATUS = RFID_MASTER_CARD_DETECTED;   
        log_i("gi8_RFID_STATUS = RFID_MASTER_CARD_DETECTED(%d)", gi8_RFID_STATUS);     

        log_i("Hello Master - Entered Program Mode");
        uint8_t count = EEPROM.read(0);   // Read the first Byte of EEPROM that
        log_i("I have (%d) record(s) on EEPROM", count);     // stores the number of ID's in EEPROM
        log_i("Scan a PICC to ADD or REMOVE to EEPROM");
        log_i("Scan Master Card again to Exit Program Mode");
        log_i("---------------------------------------------");
      }
      else 
      {
        if ( findID(readCard) ) 
        { // If not, see if the card is in the EEPROM
          gi8_RFID_STATUS = RFID_NEW_CARD_AUTHORIZED;   
          log_i("gi8_RFID_STATUS = RFID_NEW_CARD_AUTHORIZED(%d)", gi8_RFID_STATUS);   
          log_i("Welcome, You shall pass");
          // granted(300);         // Open the door lock for 300 ms
        }
        else 
        {      // If not, show that the ID was not valid
          log_i("You shall not pass");
          gi8_RFID_STATUS = RFID_NEW_CARD_DENIED;   
          log_i("gi8_RFID_STATUS = RFID_NEW_CARD_DENIED(%d)", gi8_RFID_STATUS);  
          // denied();
        }
      }
    }
  }

}

void funcRFIDAuthorize(uint8_t __enable)
{
  if(__enable)
  {
    funcCheckUpdateMasterCardStatus();
    funcRFID_ProgramAndNormalMode(successRead);
  }
  
}