
#ifndef QUANTERNION_H
#define QUANTERNION_H

#define MAX_KICKS_SESSION 40
#define NUM_KICK_POINTS 100 // 1000 points per kick
#define NUM_SENSORS 5
#define NUM_IMUS 4


struct kickPoint {
  float accel_x;
  float accel_y;
  float accel_z;
  long  time;
};



class kick
{
  public:
    void init();
    void resetKick();
    void storeKickPoint(kickPoint currPoint);
    kickPoint *kickPoints_[NUM_IMUS];

  private:
    // kickPoint *kickPoints_[NUM_IMUS - 1];
    float *adcBuffer; 
    unsigned long kickStartingTime;
    unsigned long kickEndingTime;
    unsigned long totalTime;

};

class session
{
  public:
    void init();
    void resetSession();
    void incrementKicks();
    void decrementKicks();
    bool isWalking();
    bool isMax();

    void writeKickSD(kick currKick); // microSD
    void writeCurrKickBT(); // bluetooth

  private:
    int num_kicks;
    float sessionStartingTime;
    float sessionEndingTime;


};




#endif