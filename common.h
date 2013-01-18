#ifndef FUNCTIONS_INCLUDED
#define FUNCTIONS_INCLUDED

#define SMELR 1
#define SBFLR 2
#define SIFLR 3
#define SISLR 3


char buf[80];

typedef struct {
  int encodersL;
  int encodersR;
  int bumpersL;
  int bumpersR;
  int rangeFL;
  int rangeFR;
  int rangeSL;
  int rangeSR;
} sensors;

extern sensors DEFAULT_SENSORS;


int abs (int);
void constAcceleration (int, int, int, int, sensors*, int);

int getProportion(int, int);

int initSocket();
void nextCmd();

void infraOutFront(int, int);
void moveAtVoltage(int, int);
void moveStraightAtVoltage(int);

void turnOnSpotAtVoltage(int);

bool sensorToBe(int, int, int);
bool sensorsToBe(sensors*, sensors*, sensors*);
void stopMovement();

#endif


// 12/3
// 9/4
