#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "queue.h"
#include "common.h"
#include "sensors/encoders.h"
#include "sensors/bumpers.h"
#include "sensors/rangefinders.h"
#include "sensors/us.h"
#include "sensors/sensors.h"


#define WHEELREVOLUTION 100 * 3.14159
#define WHEELDISTANCE 240

sensors DEFAULT_SENSORS = {
  .encodersL = 0,
  .encodersR = 0,
  .bumpersL = 0,
  .bumpersR = 0,
  .rangeFL = 0,
  .rangeFR = 0,
  .rangeSL = 0,
  .rangeSR = 0,
  .us = 0
};

status DEFAULT_STATUS = {
  .reposition = false,
  .spin = false,
  .straight = false
};

volts DEFAULT_VOLTS = {
  .l = 0,
  .r = 0
};

logs l = {.index = -1, .empty = true};

int sock = -1;

int abs (int value) {

  if (value < 0) return -value;
  return value;
}

int getProportion(int number, int proportion) {

  return (proportion/100)*number;
}

float getSlide(int m, int n) {
  float series1, series2;

  m = m/10;
  //printf("%i\n",m);
  n = n/10;
  // printf("%i\n",n);
  series1 = ((float)m/2)*(10+(float)m*10)/20;
  // printf("%f\n",series1);
  series2 = ((float)n/2)*(10+(float)n*10)/20;
  // printf("%f\n",series2);
  return abs(series2-series1);
}


void considerSlide(int fromVL, int fromVR, int toVL, int toVR, sensors* Sensors) {
  int slideVL = getSlide(fromVL, toVL);
  int slideVR = getSlide(fromVR, toVR);

  Sensors->encodersL -= slideVL;
  Sensors->encodersR -= slideVR;
}

void changeVelocity (int fromVL, int fromVR, int toVL, int toVR, sensors* toBeInitial, sensors* toBeFinal) { //TODO pass left and right
  sensors current = DEFAULT_SENSORS; // should be global
  sensors initial = DEFAULT_SENSORS;

  encodersGet(&current); //TODO they must be equal!
  printf("changeVelocity initial from %d:%d and encoders: %d:%d\n", fromVL, fromVR, current.encodersL, current.encodersR);
  fflush(stdout);
  encodersSet(&initial, current.encodersL, current.encodersR);
  while(sensorsToBe(&current, &initial, toBeInitial)) {
    moveAtVoltage(fromVL, fromVR);
    encodersGet(&current);
  }

  encodersGet(&current); //TODO they must be equal!
  printf("changeVelocity middle from %d:%d and encoders: %d:%d\n", toVL, toVR, current.encodersL, current.encodersR);
  fflush(stdout);
  encodersSet(&initial, current.encodersL, current.encodersR);
  while(sensorsToBe(&current, &initial, toBeFinal)) {
    moveAtVoltage(toVL, toVR);
    encodersGet(&current);
    if (toVL == 0 && toVR == 0) {break;}
  }
    printf("changeVelocity final from %d:%d and encoders: %d:%d\n", toVL, toVR, current.encodersL, current.encodersR);
}

void constAcceleration (int initialVL, int initialVR, int finalVL, int finalVR, sensors* toBe, int steps) {

  // TODO
  // iVL and fVL must be of the same sign
  
  // stop (or better, go normal) if initial and final are equal
  
  int newVL = 0;
  int newVR = 0;

  sensors current = DEFAULT_SENSORS, initial = DEFAULT_SENSORS, toBeStep = DEFAULT_SENSORS;
  sensorsGetOneStep(toBe, &toBeStep, steps);
  encodersSet(&toBeStep, toBeStep.encodersL/2, toBeStep.encodersR/2);

  int stepVL = abs(initialVL-finalVL)/steps;
  int stepVR = abs(initialVR-finalVR)/steps;
  bool positiveL = (initialVL < finalVL);
  bool positiveR = (initialVR < finalVR);

  printf("Steps (%d) %d:%d(%d:%d) at voltage %d:%d(%d:%d)  \n", steps, toBeStep.encodersL, toBeStep.encodersR, toBe->encodersL, toBe->encodersR, stepVL, stepVR, (initialVL-finalVL), (initialVR-finalVR));

  encodersGet(&current);
  encodersSet(&initial, current.encodersL, current.encodersR);
  newVL = initialVL;
  newVR = initialVR;

  while(1) {
    printf("%d-%d, %d-%d\n", finalVL, finalVR, newVL, newVR);

    /* Calculating new speed */
    if (positiveL) {
      if (newVL+stepVL <= finalVL) initialVL += stepVL;
      if (newVR+stepVR <= finalVR) initialVR += stepVR;
    } else {
      if (newVL-stepVL >= finalVL) initialVL -= stepVL;
      if (newVR-stepVL >= finalVR) initialVR -= stepVR;
    }
    
    // TODO implement NEXT so that consider slide is more accurate

    printf("%d-%d, %d-%d\n", finalVL, finalVR, newVL, newVR);

    if (newVL == 0 && newVR == 0) {
      if (positiveR || positiveL) { /*it should continue*/}
      else if (positiveR == false && positiveL == false) break;
    } else{
      considerSlide(newVL, newVR, initialVL, initialVR, &toBeStep);
      changeVelocity(newVL, newVR, initialVL, initialVR, &toBeStep, &toBeStep);
    }

    newVL = initialVL;
    newVR = initialVR;

    encodersGet(&current);
    printf("Encoders now at: %d:%d\n", current.encodersL, current.encodersR);
    if (!sensorsToBe(&current, &initial, toBe)) break;
  }
  encodersGet(&current);
  if (!sensorsToBe(&current, &initial, toBe)) {
    // continue move
  } else {

  }
}

int initSocket() {
  struct sockaddr_in s_addr;

  if (sock != -1) {
    printf("Sock is already open\n");
    close(sock);
    sock = -1;
  }

  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    fprintf(stderr, "Failed to create socket1\n");
    exit(1);
  }
  
  while (1) {
    printf("Sock is in while\n");
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    s_addr.sin_port = htons(55443);

    if (connect(sock, (struct sockaddr *) &s_addr, sizeof(s_addr)) >= 0) {
      /* connection succeeded */
      printf("Connection succeeded\n");
      return sock;
    }
    sleep(1);
    printf(".");
    fflush(stdout);
  }
}

/*
void infraOutFront(int infraFrontL, int infraFrontR) {
  stopIf(!inAngle(infraFrontL) || !inAngle(infraFrontR));
	sprintf(buf, "I LR %i %i\n", infraFrontL, infraFrontR);
	#ifdef DEBUG
	printf("I LR %i %i\n",infraFrontL, infraFrontR);
	#endif
	nextCmd();
}


bool inAngle(int angleIR) {
  if (angleIR <=90 && angleIR >= -90)
    return true;

  return false;
}
*/

bool inLimit(int voltage) {

  if (voltage <= 127 && voltage >= -127)
    return true;
  
  return false;
}

void stopIf (bool status) {

  if (status) exit(1);
}

/* Level abstraction: 0 start */

int writeCmd(char *msg, int len) {

  if (write(sock, msg, len) <= 0) {
    /* the write failed - likely the robot was switched off - attempt
       to reconnect and reinitialize */
    initSocket();
    return 0;
  } else {
    return 1;
  }
}

int readCmd(char* buf, int bufsize) {
  int val;

  fd_set read_fdset;
  fd_set except_fdset;
  struct timeval tv;
  tv.tv_sec = 2;
  tv.tv_usec = 0;
  FD_ZERO(&read_fdset);
  FD_ZERO(&except_fdset);
  FD_SET(sock, &read_fdset);
  FD_SET(sock, &except_fdset);
  if (select(sock+1, &read_fdset, NULL, &except_fdset, &tv) == 0) {
    /* we've waited 2 seconds and got no response - too long - conclude
       the socket is dead */
    printf("timed out waiting response\n");
    initSocket();
    return 0;
  }
  if (FD_ISSET(sock, &except_fdset)) {
    initSocket();
    return 0;
  }
  
  //assert(FD_ISSET(sock, &read_fdset));
  val = read(sock, buf, bufsize);
  if (val > 0) {
  } else {
    /* the write failed - likely the robot was switched off - attempt
       to reconnect and reinitialize */
    initSocket();
  }
  return val;
}


void nextCmd() {
  writeCmd(buf, strlen(buf)); memset(buf, 0, 80); read(sock, buf, 80); // TODO talk with prof
}

void moveAtVoltage(int voltage1, int voltage2) {

  stopIf(!inLimit(voltage1) || !inLimit(voltage2));
  sprintf(buf, "M LR %i %i\n", voltage1, voltage2);
  #ifdef DEBUG
  printf("M LR %i %i\n", voltage1, voltage2);
  #endif
  nextCmd();
}

void move(volts* v) {
  moveAtVoltage(v->l, v->r);
}

void stopMovement() {

  sprintf(buf, "M LR 0 0\n");
  #ifdef DEBUG
  printf("M LR 0 0\n");
  #endif
  nextCmd();
}
void stopMovementWhen(bool condition) {

  if (condition) stopMovement();
}

/* Level abstraction: 0 end*/

/* Level abstraction: 1 start */
void turnOnSpotAtVoltage(int voltage) {

  moveAtVoltage(voltage, -voltage);
  nextCmd();
}

void moveStraightAtVoltage(int voltage) {

  moveAtVoltage(voltage, voltage);
}
/* Level abstraction: 1 end */


// TODO
// turn very slowly
// move as fast as he can.