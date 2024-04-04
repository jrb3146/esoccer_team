#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <TCA9548A.h>
#include <MPU6050.h>
#include <IMUBase.h>
#include <SD.h>
#include <ADS1115.h> 

#define ADC_ADDRESS 0x48

#define IMU_ADDRESS 0x68    //Change to the address of the IMU
#define I2C_SPEED_400KHZ 400000
#define I2C_SPEED_100KHZ 100000

MPU6050 IMU0;   
MPU6050 IMU1;   
MPU6050 IMU2;   
MPU6050 IMU3;   

// Sensor data
AccelData   accelData0, accelData1, accelData2, accelData3; 
GyroData    gyroData0,  gyroData1,  gyroData2,  gyroData3;    
calData     calib0,     calib1,     calib2,     calib3;

ADS1115_WE ADC0 = ADS1115_WE(ADC_ADDRESS);


#define I2CMUX_IMU0_CHANNEL TCA_CHANNEL_2
#define I2CMUX_IMU1_CHANNEL TCA_CHANNEL_3
#define I2CMUX_IMU2_CHANNEL TCA_CHANNEL_4
#define I2CMUX_IMU3_CHANNEL TCA_CHANNEL_5
#define I2CMUX_ADC_CHANNEL  I2CMUX_IMU3_CHANNEL

TCA9548A I2CMux;

#ifdef MAIN_PROG_ENABLE // only one debug at a time, main is priority
  #define SD_ENABLE
  #define ADC_ENABLE
  #define IMU_ENABLE
  #undef LIVE_DATA_ENABLE
#endif

#ifdef MAIN_DEBUG // only one debug at a time, main is priority
  #undef IMU_DEBUG
	#undef I2C_DEBUG
	#undef ADC_DEBUG
  #undef LIVE_DATA_ENABLE
#endif

//void initIMU(MPU6050 &IMU, AccelData &accelData,  GyroData &gyroData, calData &calib);

void calibrateIMU(MPU6050& IMU, calData& calib)
{

    #ifdef VERBOSE
      delay(5000);
      Serial.println("Keep IMU level.");
      delay(5000);
      IMU.calibrateAccelGyro(&calib);
      Serial.println("Calibration done!");
      Serial.println("Accel biases X/Y/Z: ");
      Serial.print(calib.accelBias[0]);
      Serial.print(", ");
      Serial.print(calib.accelBias[1]);
      Serial.print(", ");
      Serial.println(calib.accelBias[2]);
      Serial.println("Gyro biases X/Y/Z: ");
      Serial.print(calib.gyroBias[0]);
      Serial.print(", ");
      Serial.print(calib.gyroBias[1]);
      Serial.print(", ");
      Serial.println(calib.gyroBias[2]);
      delay(5000);
    #else
      delay(10000);
      IMU.calibrateAccelGyro(&calib);
      delay(5000);
    #endif

    IMU.init(calib, IMU_ADDRESS);

}
void calibrateAll()
{
  I2CMux.openChannel(I2CMUX_IMU0_CHANNEL);
  calibrateIMU(IMU0, calib0);
  I2CMux.closeChannel(I2CMUX_IMU0_CHANNEL);

  I2CMux.openChannel(I2CMUX_IMU1_CHANNEL);
  calibrateIMU(IMU1, calib1);
  I2CMux.closeChannel(I2CMUX_IMU1_CHANNEL);

  I2CMux.openChannel(I2CMUX_IMU2_CHANNEL);
  calibrateIMU(IMU2, calib2);
  I2CMux.closeChannel(I2CMUX_IMU2_CHANNEL);


  I2CMux.openChannel(I2CMUX_IMU3_CHANNEL);
  calibrateIMU(IMU3,  calib3);
  I2CMux.closeChannel(I2CMUX_IMU3_CHANNEL);

}

void initADC()
{
 if(!ADC0.init()){
    #ifdef VERBOSE
    Serial.println("ADS1115 not connected!");
    #endif
  }

  /* Set the voltage range of the ADC to adjust the gain
   * Please note that you must not apply more than VDD + 0.3V to the input pins!
   * 
   * ADS1115_RANGE_6144  ->  +/- 6144 mV
   * ADS1115_RANGE_4096  ->  +/- 4096 mV
   * ADS1115_RANGE_2048  ->  +/- 2048 mV (default)
   * ADS1115_RANGE_1024  ->  +/- 1024 mV
   * ADS1115_RANGE_0512  ->  +/- 512 mV
   * ADS1115_RANGE_0256  ->  +/- 256 mV
   */
  ADC0.setVoltageRange_mV(ADS1115_RANGE_6144); //comment line/change parameter to change range

  /* Set the inputs to be compared
   *  
   *  ADS1115_COMP_0_1    ->  compares 0 with 1 (default)
   *  ADS1115_COMP_0_3    ->  compares 0 with 3
   *  ADS1115_COMP_1_3    ->  compares 1 with 3
   *  ADS1115_COMP_2_3    ->  compares 2 with 3
   *  ADS1115_COMP_0_GND  ->  compares 0 with GND
   *  ADS1115_COMP_1_GND  ->  compares 1 with GND
   *  ADS1115_COMP_2_GND  ->  compares 2 with GND
   *  ADS1115_COMP_3_GND  ->  compares 3 with GND
   */
  ADC0.setCompareChannels(ADS1115_COMP_3_GND); //comment line/change parameter to change channel

  /* Set number of conversions after which the alert pin asserts
   * - or you can disable the alert 
   *  
   *  ADS1115_ASSERT_AFTER_1  -> after 1 conversion
   *  ADS1115_ASSERT_AFTER_2  -> after 2 conversions
   *  ADS1115_ASSERT_AFTER_4  -> after 4 conversions
   *  ADS1115_DISABLE_ALERT   -> disable comparator / alert pin (default) 
   */
  //adc.setAlertPinMode(ADS1115_ASSERT_AFTER_1); //uncomment if you want to change the default

  /* Set the conversion rate in SPS (samples per second)
   * Options should be self-explaining: 
   * 
   *  ADS1115_8_SPS 
   *  ADS1115_16_SPS  
   *  ADS1115_32_SPS 
   *  ADS1115_64_SPS  
   *  ADS1115_128_SPS (default)
   *  ADS1115_250_SPS 
   *  ADS1115_475_SPS 
   *  ADS1115_860_SPS 
   */
  // adc.setConvRate(ADS1115_8_SPS); //uncomment if you want to change the default

  /* Set continuous or single shot mode:
   * 
   *  ADS1115_CONTINUOUS  ->  continuous mode
   *  ADS1115_SINGLE     ->  single shot mode (default)
   */
  ADC0.setMeasureMode(ADS1115_CONTINUOUS); //comment line/change parameter to change mode

   /* Choose maximum limit or maximum and minimum alert limit (window) in Volt - alert pin will 
   *  assert when measured values are beyond the maximum limit or outside the window 
   *  Upper limit first: setAlertLimit_V(MODE, maximum, minimum)
   *  In max limit mode the minimum value is the limit where the alert pin assertion will be  
   *  cleared (if not latched)  
   * 
   *  ADS1115_MAX_LIMIT
   *  ADS1115_WINDOW
   * 
   */
  //adc.setAlertModeAndLimit_V(ADS1115_MAX_LIMIT, 3.0, 1.5); //uncomment if you want to change the default
  
  /* Enable or disable latch. If latch is enabled the alert pin will assert until the
   * conversion register is read (getResult functions). If disabled the alert pin assertion will be
   * cleared with next value within limits. 
   *  
   *  ADS1115_LATCH_DISABLED (default)
   *  ADS1115_LATCH_ENABLED
   */
  //adc.setAlertLatch(ADS1115_LATCH_ENABLED); //uncomment if you want to change the default

  /* Sets the alert pin polarity if active:
   *  
   * ADS1115_ACT_LOW  ->  active low (default)   
   * ADS1115_ACT_HIGH ->  active high
   */
  //adc.setAlertPol(ADS1115_ACT_LOW); //uncomment if you want to change the default
 
  /* With this function the alert pin will assert, when a conversion is ready.
   * In order to deactivate, use the setAlertLimit_V function  
   */
  //adc.setAlertPinToConversionReady(); //uncomment if you want to change the default
  #ifdef VERBOSE
  Serial.println("ADS1115 Example Sketch - Continuous Mode");
  Serial.println("All values in volts");
  Serial.println();
  #endif
}

float readADC(ADS1115_MUX channel) {
  float voltage = 0.0;
  ADC0.setCompareChannels(channel);
  voltage = ADC0.getResult_V(); // alternative: getResult_mV for Millivolt
  return voltage;
}

void initIMU (MPU6050& IMU, calData& calib)
{
  int err = -1;
    Serial.print(" initializing IMU");
    err = IMU.init(calib, IMU_ADDRESS);
    
    if (err != 0) {
      #ifdef MAIN_DEBUG
        // some kind of visual indicator
      #endif
        #ifdef VERBOSE
        Serial.println("");
        Serial.print("Error initializing IMU: ");
        Serial.println(IMU.IMUName());
        Serial.print("Error code: ");
        Serial.println(err);
        Serial.print("I2C Port :");
        Serial.print(I2CMux.readRegister());
        delay(5000);
        #endif
      
    }
}


void printAccel(MPU6050& IMU, AccelData& accelData)
{ 
  for (int i = 0; i < 10; i++)
  {
    IMU.update();
    IMU.getAccel(&accelData);
    Serial.println((String)("----- Time:  -----", millis()));
    Serial.println("Accel. Data: ");
    Serial.print("X : ");
    Serial.println(accelData.accelX);
    Serial.print("Y : ");
    Serial.println(accelData.accelY);
    Serial.print("Z : ");
    Serial.println(accelData.accelZ);
    delayMicroseconds(100);
  }
}


void setup()
{
    Serial.begin(9600);
    Wire.begin();
    Wire.setClock(100000);
    #ifdef MAIN_DEBUG
    // wait for button
    //while(Serial.readString() != "0"){
      Serial.println("Waiting for start...");
      delay(2000);
    //}

    // soon 
    // while(analogRead(2) < 512);
    
    //Serial.println("Starting Program...");
    #endif

    
    calib1 = { 0 };
    calib2 = { 0 };
    calib3 = { 0 };


  


}


#ifdef LIVE_DATA_ENABLE
void loop()
{
  I2CMux.begin(Wire);
  I2CMux.closeAll();
  Wire.setClock(I2C_SPEED_400KHZ);
  delay(100);
  I2CMux.scanAll();
  Wire.setClock(I2C_SPEED_100KHZ);
  delay(100);
    // In (fast) movement

  #ifdef VERBOSE
  I2CMux.scanAll();

  Serial.println("Opening Channel for IMU 0");
  I2CMux.openChannel(I2CMUX_IMU0_CHANNEL);
  initIMU(IMU0. calib0);
  Serial.println("Closing Channel for IMU 0");
  I2CMux.closeChannel(I2CMUX_IMU0_CHANNEL); 

  Serial.println("Opening Channel for IMU 1");
  I2CMux.openChannel(I2CMUX_IMU1_CHANNEL);
  delay(50);
  initIMU(IMU1, calib1);
  I2CMux.closeChannel(I2CMUX_IMU1_CHANNEL); // IMU1
  Serial.println("Closing Channel for IMU 0");
  delay(100);

  Serial.println("Opening Channel for IMU 2");
  I2CMux.openChannel(I2CMUX_IMU2_CHANNEL); // IMU2
  initIMU(IMU2, calib2);
  Serial.println("Closing Channel for IMU 1");
  I2CMux.closeChannel(I2CMUX_IMU2_CHANNEL); // IMU2

  Serial.println("Opening Channel for IMU 3");
  I2CMux.openChannel(I2CMUX_IMU3_CHANNEL); // IMU1
  initIMU(IMU3, calib3); //, accelData3, gyroData3, calib3);
  Serial.println("Closing Channel for IMU 2");
  I2CMux.closeChannel(I2CMUX_IMU3_CHANNEL); // IMU1
  #else
  I2CMux.scanAll();

  I2CMux.openChannel(I2CMUX_IMU0_CHANNEL);
  delay(50);
  initIMU(IMU0, calib0);
  delay(50);
  I2CMux.closeChannel(I2CMUX_IMU0_CHANNEL); 

  I2CMux.openChannel(I2CMUX_IMU1_CHANNEL);
  delay(50);
  initIMU(IMU1, calib1);
  delay(50);
  I2CMux.closeChannel(I2CMUX_IMU1_CHANNEL);

  I2CMux.openChannel(I2CMUX_IMU2_CHANNEL); 
  delay(50);
  initIMU(IMU2, calib2);
  delay(50);
  I2CMux.closeChannel(I2CMUX_IMU2_CHANNEL);

  I2CMux.openChannel(I2CMUX_IMU3_CHANNEL); 
  delay(50);
  initIMU(IMU3, calib3);
  delay(50);
  I2CMux.closeChannel(I2CMUX_IMU3_CHANNEL);

  #endif

  
  #ifdef CALIBRATE_ENABLE
  calibrateAll();
  #endif

  I2CMux.openChannel(I2CMUX_ADC_CHANNEL); 
  if(!ADC0.init()){
    #ifdef VERBOSE
    Serial.println("ADS1115 not connected!");
    #endif
  }
  I2CMux.closeChannel(I2CMUX_ADC_CHANNEL); 

  while (true) {
    I2CMux.openChannel(I2CMUX_IMU0_CHANNEL);
    printAccel(IMU0, accelData0);
    I2CMux.closeChannel(I2CMUX_IMU0_CHANNEL);
    delay(10);

    I2CMux.openChannel(I2CMUX_IMU1_CHANNEL);
    printAccel(IMU1, accelData1);
    I2CMux.closeChannel(I2CMUX_IMU1_CHANNEL);
    delay(10);

    I2CMux.openChannel(I2CMUX_IMU2_CHANNEL);
    printAccel(IMU2, accelData2);
    I2CMux.closeChannel(I2CMUX_IMU2_CHANNEL);
    delay(10);

    I2CMux.openChannel(I2CMUX_IMU3_CHANNEL);
    printAccel(IMU3, accelData3);
    float voltage = 0.0;
    Serial.print("0: ");
    voltage = readADC(ADS1115_COMP_3_GND);
    Serial.println(voltage);

    I2CMux.closeChannel(I2CMUX_IMU3_CHANNEL);
    delay(10);
  }

}

#endif

#ifdef MAIN_PROG_ENABLE
void loop()
{
    I2CMux.begin(Wire);
    I2CMux.closeAll();

 

}

#endif


void read_swing_loop()
{
    // 
    // 
    // Pressure sensor triggered?
}