/*******************************************************************************************************************************/
/*************************************** I N C L U D E S ***********************************************************************/

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Arduino.h>

Adafruit_MPU6050 mpu;

void setup() 
{

  /* Start serial communication for debuging */
  Serial.begin( 115200 );

  Serial.println( "Initializing MPU." );

  /* Initialize the MPU */
  if( !mpu.begin() )
  {
    Serial.println( "Failed to initialize MPU." );
  }
  else
  {
    Serial.println( "Initialized the MPU." );
  }

  mpu.setAccelerometerRange( MPU6050_RANGE_8_G );
  Serial.println( "Accelerometer set to +-8G." );

  mpu.setGyroRange( MPU6050_RANGE_500_DEG );
  Serial.println( "Gyroscope range set to +-500 deg/s." );

  mpu.setFilterBandwidth( MPU6050_BAND_5_HZ );
  Serial.println( "Filter bandwidth set to 5Hz." );

  delay( 10 );

}

void loop() 
{
  sensors_event_t acceleration;
  sensors_event_t gyroscope;
  sensors_event_t temperature;

  mpu.getEvent( &acceleration, &gyroscope, &temperature );

  Serial.println( "--------------------------------------------" );

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(acceleration.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(acceleration.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(acceleration.acceleration.z);
  Serial.println(" m/s^2");

  /* Calculate motor acceleration based on the Y */
  int8_t ay = acceleration.acceleration.y;
  uint8_t motor_accel = 127;
  /* Decide which direction */
  if( ay < 0 )
  {
    /* Rotate down */
    if( ay < -5 )
    {
      motor_accel = 255;
    }
    else
    {
    motor_accel += abs( ay ) * 25;
    }
  }
  else if( ay > 0 )
  {
    /* Rotate up */
    if( ay > 8 )
    {
      motor_accel = 0;
    }
    else
    {
      motor_accel -= ay * 15;
    }
  }

  Serial.println( "..................................." );
  Serial.print( "Acceleration request: " );
  Serial.println( motor_accel );
  Serial.println( "..................................." );



/* Calculate motor turn based on the X */
  int8_t ax = acceleration.acceleration.x;
  uint8_t motor_turn = 127;
  /* Decide which direction */
  if( ax < 0 )
  {
    /* Rotate right */
    if( ax < -5 )
    {
      motor_turn = 255;
    }
    else
    {
    motor_turn += abs( ax ) * 25;
    }
  }
  else if( ax > 0 )
  {
    /* Rotate left */
    if( ax > 5 )
    {
      motor_turn = 0;
    }
    else
    {
      motor_turn -= ax * 25;
    }
  }

  Serial.println( "..................................." );
  Serial.print( "Turn request: " );
  Serial.println( motor_turn );
  Serial.println( "..................................." );

  delay( 500 );

}