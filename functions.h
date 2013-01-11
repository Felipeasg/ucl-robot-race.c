#ifndef FUNCTIONS_INCLUDED
#define FUNCTIONS_INCLUDED

struct sensors {
  int encoders[2];
};
typedef void (*event_cb_t)(const struct event *evt, void *userdata);


void initSocket();
void nextCmd();

void moveAtVoltage(int voltage1, int voltage2);
void moveStraightAtVoltage(int voltage);

void turnAtVoltage(int voltage1, int voltage2);
void turnOnSpotAtVoltage(int voltage);

void stopMovement();

#endif
