/*******************************************************************************************************************************/
/*************************************** I N C L U D E S ***********************************************************************/

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

/*******************************************************************************************************************************/
/*************************************** D E F I N E S *************************************************************************/

/* Uncomment this define if you want to use 'println' the debugging data */
#define DEBUG_ON

/* BLE server name */
#define ac_BLE_SERVERNAME "BLE_TestServer"

/* Define service UUID. An UUID can be generated using the "https://www.uuidgenerator.net/" website */
#define ac_SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"

/*******************************************************************************************************************************/
/*************************************** V A R I A B L E S *********************************************************************/

/* Client connection tracker: 0 if isn't connected, 1 if a client is connected */
uint8_t ub_DeviceConnected = 0;

/* Variables for measuring time, this will be used to set the speed of updating the TurnCommand data */
uint16_t uw_LastTime = 0;
uint16_t uw_TimerDelay = 300;

/* This is a 8-bit variable that containt the TurnCommand information, the value ranges from 0 to 255
0 means turn full left, 127 means no turn, and 255 means turn full right. This is an 'analog' value. */
uint8_t ub_TurnCommand = 127;

/* Setup the TurnCommand characteristic, it's property and descriptor */
BLECharacteristic TurnCommandCharacteristics( "cba1d466-344c-4be3-ab3f-189f80dd7518", BLECharacteristic::PROPERTY_NOTIFY );
BLEDescriptor TurnCommandDescriptor( BLEUUID( ( uint16_t ) 0x2902 ) );

/*******************************************************************************************************************************/
/*************************************** F U N C T I O N   D E C L A R A T I O N S *********************************************/

/*******************************************************************************************************************************/
/*************************************** C L A S S E S *************************************************************************/

class v_ServerCallbackFunction: public BLEServerCallbacks
{
  void onConnect( BLEServer *p_Server )
  {
    ub_DeviceConnected = 1;
  };
  void onDisconnect( BLEServer *p_Server )
  {
    ub_DeviceConnected = 0;
  }
};

void setup() 
{
  /* Start serial communication */
  Serial.begin( 115200 );

  /* Create a BLE device */
  BLEDevice::init( ac_BLE_SERVERNAME );

  /* Set the BLE device as a server and assign a callback function */
  BLEServer *p_Server = BLEDevice::createServer();
  p_Server->setCallbacks( new v_ServerCallbackFunction() );

  /* Start the BLE server */
  BLEService *p_Service = p_Server->createService( ac_SERVICE_UUID );

  /* Create the TurnCommand characteristic */
  p_Service->addCharacteristic( &TurnCommandCharacteristics );

  /* Set the value of the descriptor of the TurnCommand characteristic */
  TurnCommandDescriptor.setValue( "TurnCommand" );
  /* Add a descriptor to the TurnCommand characteristic */
  TurnCommandCharacteristics.addDescriptor( new BLE2902() );

  /* Start the service */
  p_Service->start();

  /* Start adcertising so other devices can find it */
  BLEAdvertising *p_Advertising = BLEDevice::getAdvertising();
  p_Advertising->addServiceUUID( ac_SERVICE_UUID );
  p_Server->getAdvertising()->start();

  #ifdef DEBUG_ON
  Serial.println( "Waiting for a client to send a connection notification..." );
  #endif

}

void loop() 
{
  /* If a client is connected to this server */
  if( 1 == ub_DeviceConnected )
  {
    /* If we waited an arbitrary time */
    if( (millis() - uw_LastTime ) > uw_TimerDelay )
    {
      /* Update the TurnCommand */
      ub_TurnCommand += 1;
    }
    else
    {
      /* do nothing */
    }
  }
  else
  {
    /* do nothing */
  }
}

/*******************************************************************************************************************************/
/*************************************** F U N C T I O N S *********************************************************************/
