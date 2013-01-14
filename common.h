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

void moveAtVoltage(int voltage1, int voltage2);
void moveStraightAtVoltage(int voltage);

void turnAtVoltage(int voltage1, int voltage2);
void turnOnSpotAtVoltage(int voltage);

bool sensorToBe(int current, int initial, int toBe );
void stopMovement();

#endif


// 12/3
// 9/4