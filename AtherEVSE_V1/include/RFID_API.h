#include <SPI.h>
#include <MFRC522.h>


 #define RST_PIN         22 //27 //9          // Configurable, see typical pin layout above
 #define SS_PIN          0 // 10         // Configurable, see typical pin layout above


uint8_t initRFID(void);



void funcRFIDAuthorize(uint8_t __enable);

