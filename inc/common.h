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

extern sensors DEFAULT_SENSORS;
extern status DEFAULT_STATUS;
extern volts DEFAULT_VOLTS;
extern request DEFAULT_REQUEST;
extern robot r;

extern bool
frontsAreEqual,
frontLInfinite,
frontRInfinite,
frontsAreInfinite,
frontLisCloser,
frontLRisk,
frontRRisk,

sidesAreEqual,
sideLInfinite,
sideRInfinite,
sidesAreInfinite,
sideLisCloser,
sideLRisk,
sideRRisk,

backLisInfinite,
backRisInfinite,
backsAreInfinite,
backLisCloser,
backLRisk,
backRRisk,

isTurningR,
isTurningL,

isRotatingL,
isRotatingR;

extern int properRotation;

extern dist angleWall, backVal, sideVal;

extern int backConsidered, backDif, sideConsidered, sideDif, frontConsidered, frontDif, frontVal;
extern const int backOk, backRange, backMinimum, sideOk, sideRange, sideMinimum, frontOk, frontRange, frontMinimum;


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

bool sensorsToBe(sensors* Sensors, sensors* initial, sensors* toBe);

void cTrail();
double min(double one, double two);
void setWall();
volts setVoltage(volts speed, dist scale);
#endif


// 12/3
// 9/4
