#ifndef FUNCTIONS_INCLUDED
#define FUNCTIONS_INCLUDED

#define SMELR 1

char buf[80];
struct sockaddr_in s_addr;
int sock;

typedef struct {
  int encodersL;
  int encodersR;
} sensors;

void initSocket();
void nextCmd();

void moveAtVoltage(int, int);
void moveStraightAtVoltage(int);

void turnAtVoltage(int, int);
void turnOnSpotAtVoltage(int);

bool sensorToBe(int, int, int);
void stopMovement();

#endif


// 12/3
// 9/4