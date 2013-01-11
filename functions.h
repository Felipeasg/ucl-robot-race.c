#ifndef FUNCTIONS_INCLUDED
#define FUNCTIONS_INCLUDED

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

void stopMovement();

#endif
