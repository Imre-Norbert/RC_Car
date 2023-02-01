/*******************************************************************************************************************************/
/*************************************** I N C L U D E S ***********************************************************************/

#include <Arduino.h>
#include <BLEDevice.h>
//#include <BLEUtils.h>
//#include <BLEServer.h>
//#include <BLE2902.h>

/*******************************************************************************************************************************/
/*************************************** D E F I N E S *************************************************************************/

/* Uncomment this define if you want to use 'println' the debugging data */
#define DEBUG_ON

/* Define the name of the server specified in the server source code */
#define ac_BLE_SERVERNAME "BLE_TestServer"

/* This is the UUID of the service that we want to read */
static BLEUUID bleuuid_Service( "91bad492-b950-4226-aa2b-4ede9fa42f59" );

/* This is the UUID of the characteristic that we want to read */
static BLEUUID bleuuid_Characteristic( "cba1d466-344c-4be3-ab3f-189f80dd7518" );

/*******************************************************************************************************************************/
/*************************************** V A R I A B L E S *********************************************************************/

/* Variable that tells if we shoud begin connecting */
static uint8_t ub_doConnect = 0;

/* Variable that tells if the connection is up */
static uint8_t ub_connected = 0;

/* Address of the peripheral device. Address will be found during scanning... */
static BLEAddress *pServerAddress;

/* The characteristic that we want to read */
static BLERemoteCharacteristic *TurnCommandCharacteristic;

/* Variable to store the Turn Command */
uint8_t ub_TurnCommand = 0;

/* Flag that gets set when a new turn command reading is available */
uint8_T ub_NewTurnCommandReading = 0;

/*******************************************************************************************************************************/
/*************************************** F U N C T I O N   D E C L A R A T I O N S *********************************************/

/*******************************************************************************************************************************/
/*************************************** C L A S S E S *************************************************************************/

void setup() 
{
;
}

void loop() 
{
;
}

/*******************************************************************************************************************************/
/*************************************** F U N C T I O N S *********************************************************************/
