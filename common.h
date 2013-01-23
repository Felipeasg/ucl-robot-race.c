#ifndef FUNCTIONS_INCLUDED
#define FUNCTIONS_INCLUDED

#define SMELR 1
#define SBFLR 2
#define SIFLR 3
#define SISLR 4
#define SUS 5


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
  int us;
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
  sensors s;
  volts v;
} robot;

typedef struct {
  sensors sensors[5];
  int index;
  bool empty;
  bool wall;
} logs;

extern sensors DEFAULT_SENSORS;
extern status DEFAULT_STATUS;
extern volts DEFAULT_VOLTS;
extern logs l;
extern int sock;


int abs (int);
void constAcceleration (int, int, int, int, sensors*, int);

int getProportion(int, int);

int initSocket();
void nextCmd();

void infraOutFront(int, int);
void moveAtVoltage(int, int);
void moveStraightAtVoltage(int);

void turnOnSpotAtVoltage(int);

void stopMovement();

#endif


// 12/3
// 9/4
