

#include "quanternion.h"
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>


#define SESSION_FILE_NAME(A) (String) "kick_" + #A + ".csv"
#define FLOAT_TO_INT(x) static_cast<int>(#x)

String const csvFileHeader = "Time,IMU1_AccelX,IMU1_AccelY,IMU1_AccelZ,IMU2_AccelX,IMU2_AccelY,IMU2_AccelZ,IMU3_AccelX,IMU3_AccelY,IMU3_AccelZ,IMU4_AccelX,IMU4_AccelY,IMU4_AccelZ";

// kick class

void kick::init()
{

    for(int i =0; i < NUM_IMUS; i++) {
        this->kickPoints_[i] = (kickPoint*) malloc(sizeof(kickPoint) * NUM_KICK_POINTS);
        if (this->kickPoints_[i] == NULL){
                
                // quit program 
        }
    }
    this->adcBuffer = (float*) malloc(sizeof(float) * NUM_KICK_POINTS);
    if (this->adcBuffer == NULL){
            
            // quit program 
    }


    this->totalTime = 0.0;
    this->kickEndingTime = 0.0;
    this->kickStartingTime = millis();
}

void kick::resetKick()
{
    // reset buffer pointers
    this->totalTime = 0.0;
    this->kickEndingTime = 0.0;
    this->kickStartingTime = 0.0; // millis();
}

void kick::storeKickPoint(kickPoint currPoint)
{
    this->kickEndingTime = millis();
    this->totalTime = kickEndingTime - kickStartingTime;
}



// session class
void session::init()
{
    this->num_kicks = 0;
    this->sessionEndingTime     = 0.0;
    this->sessionStartingTime   = 0.0;
}

void session::resetSession()
{
    this-> num_kicks = 0;
}

void session::incrementKicks()
{
    this->num_kicks ++;
}

void session::decrementKicks()
{
    this->num_kicks --;
}

bool session:: isWalking()
{
    // digital 
}

bool session::isMax()
{
    return this->num_kicks == MAX_KICKS_SESSION;
}

/*
    .csv format

    Time IMU1_AccelX IMU1_AccelY IMU1_AccelZ ...

    2.2   4.9           3.8           4.9
     ...

*/


void session::writeKickSD(kick currKick)
{
 // micro sd card stuff   
    #ifdef SD_ENABLE
    File dataFile = SD.open(SESSION_FILE_NAME(0), FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
        // write headers
        dataFile.println(csvFileHeader);
        // write data
      String line = "";

      kickPoint *kickPointPtrTemp[NUM_IMUS] = { currKick.kickPoints_[0], 
                                                currKick.kickPoints_[1],
                                                currKick.kickPoints_[2],
                                                currKick.kickPoints_[3] };

      for(int i = 0; i < NUM_KICK_POINTS; i++){

        // seperate buffer for time ??
        //dataFile.print(); // time
        dataFile.print(",");
        for(int j = 0; j < NUM_IMUS; j++){
            //dataFile.print(); // imuX X
            dataFile.print(",");
            //dataFile.print(); // imuX Y
            dataFile.print(",");
            //dataFile.print(); // imuX Z
            dataFile.print(",");
        }
        
      }

    }// if the file isn't open, pop up an error:
    else {
      Serial.println("Error opening " + SESSION_FILE_NAME(0));
    }
    dataFile.close();
    #endif

}