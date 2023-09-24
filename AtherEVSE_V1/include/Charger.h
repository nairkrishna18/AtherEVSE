


/************************Trigger Pin or Point************************************/
#define FOTA_SWITCH                5//   2 //(39) // Cannot use External interrupt pins they may need piullup to configure the same..
#define FOTA_TRIGGER_DETECTED     (digitalRead(FOTA_SWITCH))
#define FOTA_TRIGGER_CLEARED      (!digitalRead(FOTA_SWITCH))
/*********************************************************************************/

// void funcChargerStateMachine(void);
void initCreateTask(void);

void funcComState(void);
void funcMainStateMachine(void);
