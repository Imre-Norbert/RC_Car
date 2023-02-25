/*******************************************************************************************************************************/
/*************************************** I N C L U D E S ***********************************************************************/

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
//#include <BLEServer.h>
#include <BLE2902.h>

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

/* SOMETHING USED FOR THE NOTIFICATIONS, LOOK IT UP! */
const uint8_t aub_notificationOn[ 2 ] = {0x1, 0x0};
const uint8_t aub_notificationOff[ 2 ] = {0x0, 0x0};

/* The characteristic that we want to read */
static BLERemoteCharacteristic *TurnCommandCharacteristic;

/* Variable to store the Turn Command */
uint8_t ub_TurnCommand = 0;

/* Flag that gets set when a new turn command reading is available */
uint8_t ub_NewTurnCommandReading = 0;

/*******************************************************************************************************************************/
/*************************************** F U N C T I O N   D E C L A R A T I O N S *********************************************/

/* This function is used to connect to the Server */
uint8_t ub_ConnectToServer( BLEAddress pAddress );

/* This is the callback function of the TurnCommand. This gets called when the server sends a new turn command reading */
static void v_TurnCommandNotifyCallback( BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify );

/*******************************************************************************************************************************/
/*************************************** C L A S S E S *************************************************************************/

/* Callback function that gets called, when another device's advertisement has been received */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks 
{
    void onResult( BLEAdvertisedDevice advertisedDevice ) 
    {
        /* Check if the name of the advertiser matches the one we are looking for */
        if ( advertisedDevice.getName() == ac_BLE_SERVERNAME ) 
        { 
            /* Stop the scanning because the Server was found */
            advertisedDevice.getScan()->stop(); 

            /* Store the address of the server */
            pServerAddress = new BLEAddress( advertisedDevice.getAddress() );
            
            /* We are ready to connect */
            ub_doConnect = 1; 
        
            /* Print some debug data */
            #ifdef DEBUG_ON
            Serial.println( " - Device found. Connecting." );
            #endif
        }
    }
};

void setup() 
{
    /* Start the serial communication */
    Serial.begin( 115200 );

    /* Print some debug data */
    #ifdef DEBUG_ON
    Serial.println( " - Starting Arduino BLE Client " );
    #endif

    /* Initialize the BLE device */
    BLEDevice::init( "" );

    /* LOOK UP WHAT THE FOLLOWING SERIES OF COMMANDS DOES! */
    
    /* Retrieve the scanner */
    BLEScan *pBLEScan = BLEDevice::getScan();
    /* Set the callback function that will be called when a new device is detected */
    pBLEScan->setAdvertisedDeviceCallbacks( new MyAdvertisedDeviceCallbacks() );
    /* Specify that we want active stanning */
    pBLEScan->setActiveScan( true );
    /* Specify the scanning time in seconds */
    pBLEScan->start( 30 );
}

void loop() 
{
    /* Check if we are ready to connect */
    if( ub_doConnect == 1 )
    {
        /* If we connected successfully to the server */
        if( 1 == ub_ConnectToServer( *pServerAddress ) )
        {
            /* Print some debug data */
            #ifdef DEBUG_ON
            Serial.println( " - Connected to the BLE server" );
            #endif

            /* Activate the Notify property of each characteristic */
            TurnCommandCharacteristic->getDescriptor( BLEUUID( ( uint16_t )0x2902 ) )->writeValue( ( uint8_t* )aub_notificationOn, 2, true );

            /* Change the connection tracking variable */
            ub_connected = 1;
        }
        else
        {
            /* Print some debug data */
            #ifdef DEBUG_ON
            Serial.println( " - We have failed to connect to the server; Restart your device to scan for nearby BLE server again." );
            #endif
        }

        /* Now that we successfully connected, we dont need to be ready to connect */
        ub_doConnect = 0;
    }

    /* If new readings are available, print to the screen */
    if( ub_NewTurnCommandReading == 1 )
    {
        /* Print some debug data */
        #ifdef DEBUG_ON
        Serial.print( " -  Turn Command:" );
        Serial.println( ub_TurnCommand );
        #endif
    }

    /* Delay a bit between loops */
    delay( 1000 );
}

/*******************************************************************************************************************************/
/*************************************** F U N C T I O N S *********************************************************************/

/* This function is used to connect to the Server */
uint8_t ub_ConnectToServer( BLEAddress pAddress )
{
    /* Initialize the return value placeholder to 0 */
    uint8_t ub_RetVal = 0;

    /* Create a BLE client */
    BLEClient *pClient = BLEDevice::createClient();

    /* Connect to the remote server */
    pClient->connect( pAddress );

    /* Print some debug data */
    #ifdef DEBUG_ON
    Serial.println( " - Connected to server" );
    #endif

    /* Search for a reference of the sercive that we are intrested in on the BLE server */
    BLERemoteService *pRemoteService = pClient->getService( bleuuid_Service );

    /* Error checking */
    if( pRemoteService == nullptr )
    {
        /* Print some debug data */
        #ifdef DEBUG_ON
        Serial.println( " - Failed to find the service UUID" );
        #endif
    }
    else
    {
        /* Search for a reference of the characteristic that we are intrested in */
        TurnCommandCharacteristic = pRemoteService->getCharacteristic( bleuuid_Characteristic );

        /* Error checking */
        if( TurnCommandCharacteristic == nullptr )
        {
            /* Print some debug data */
            #ifdef DEBUG_ON
            Serial.println( " - Failed to find the characteristic UUID" );
            #endif
        }
        else
        {
            /* Print some debug data */
            #ifdef DEBUG_ON
            Serial.println( " - Found the service and the characteristic UUID" );
            #endif

            /* Assign callback functions for the characteristics */
            TurnCommandCharacteristic->registerForNotify( v_TurnCommandNotifyCallback );
            
            /* Update the return value */
            ub_RetVal = 1;
        }
    }

    /* return the status */
    return ub_RetVal;
}

/* This is the callback function of the TurnCommand. This gets called when the server sends a new turn command reading */
static void v_TurnCommandNotifyCallback( BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify )
{
    /* Store the new TurnCommand data into a local variable */
    /* NOT SURE THIS WORKS LIKE THIS( IN THE EXAMPLE IS SOME char* SHIT ) */
    ub_TurnCommand = *pData;

    /* Update the variable that notifies of a new Turn command value */
    ub_NewTurnCommandReading = 1;
}

