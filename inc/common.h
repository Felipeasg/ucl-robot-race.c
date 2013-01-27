#ifndef FUNCTIONS_INCLUDED
#define FUNCTIONS_INCLUDED

#define SMELR 1
#define SBFLR 2
#define SIFLR 3
#define SISLR 4
#define SUS 5

#define LEFT 1
#define RIGHT 2

#define KRED  "\x1B[31m"
#define KNRM  "\x1B[0m"
#define KGRN  "\x1B[32m"


char buf[80];

typedef struct {
  int encodersL;
  int encodersR;
  int bumpersL;
  int bumpersR;
  int rangeFL;
  int rangeFR;
  int rangeFLAngle;
  int rangeFRAngle;
  int rangeSL;
  int rangeSR;
  int us;
  int wall;
} sensors;

typedef struct {
  bool spin;
  bool reposition;
  bool straight;
} status;

typedef struct {
  int l;
  int r;
} volts;

typedef struct {
  double l;
  double r;
} dist;

typedef struct {
  sensors sensors[20];
  int index;
  bool empty;
  int wall;
} logs;

typedef struct {
  sensors* sensors[20];
  int index;
  bool empty;
  int wall;
  } ptrLogs;

typedef struct {
  sensors s;
  volts v;
  logs l;
} robot;

typedef struct {
<<<<<<< HEAD:inc/common.h
  bool checkFront;
  bool checkBack;
  bool checkSide;
  bool checkEncoders;
  
  bool calculateSpeed;
  bool calculateBack;
  bool calculateFront;
  bool calculateSide;
  
  bool setWallAuto;
  bool setWall;

  int considerSide;
} request;
=======
  sensors sensors[5];
  int index;
  bool empty;
  bool wall;
} logs;
>>>>>>> 3bc8e3c27e3b0568d0311c901159394c11bafc79:common.h

extern sensors DEFAULT_SENSORS;
extern status DEFAULT_STATUS;
extern volts DEFAULT_VOLTS;
<<<<<<< HEAD:inc/common.h
extern request DEFAULT_REQUEST;
extern robot r;
=======
extern logs l;
extern int sock;
>>>>>>> 3bc8e3c27e3b0568d0311c901159394c11bafc79:common.h


int abs (int);
double absDouble(double i);
void constAcceleration (int, int, int, int, sensors*, int);

int getProportion(int, int);

int initSocket();
void nextCmd();

void infraOutFront(int, int);
void moveAtVoltage(int, int);
void moveStraightAtVoltage(int);

void turnOnSpotAtVoltage(int);

void stopMovement();


void cTrail();
double min(double one, double two);
#endif


// 12/3
// 9/4
