#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "common.h"
#include "sensors/encoders.h"

#define WHEELREVOLUTION 100 * 3.14159
#define WHEELDISTANCE 240

int sock = -1;

int abs (int value) {
  if (value < 0) return -value;
  return value;
}

int getProportion(int number, int proportion) {
  return (proportion/100)*number;
}

void changeVelocity (int fromVL, int fromVR, int toVL, int toVR, sensors* toBeInitial, sensors* toBeFinal) { //TODO pass left and right
  sensors current; // should be global
  sensors initial;

  encodersGet(&current); //TODO they must be equal!
  printf("changeVelocity initial from %d:%d and encoders: %d:%d\n", fromVL, fromVR, current.encodersL, current.encodersR);
  fflush(stdout);
  encodersSet(&initial, current.encodersL, current.encodersR);
  while(sensorsToBe(&current, &initial, toBeInitial)) {
    moveAtVoltage(fromVL, fromVR);
    encodersGet(&current);
  }
  
  encodersGet(&current); //TODO they must be equal!
  printf("changeVelocity initial from %d:%d and encoders: %d:%d\n", toVL, toVR, current.encodersL, current.encodersR);
  fflush(stdout);
  encodersSet(&initial, current.encodersL, current.encodersR);
  while(sensorsToBe(&current, &initial, toBeFinal)) {
    moveAtVoltage(toVL, toVR);
    encodersGet(&current);
  }
}

void sensorsGetOneStep(sensors* Sensors, sensors* New, int steps) {
  // TODO use sensorsSet instead
  if (Sensors->encodersL != 0) New->encodersL = Sensors->encodersL/steps;
  if (Sensors->encodersR != 0) New->encodersR = Sensors->encodersR/steps;
}

void constAcceleration (int initialVL, int initialVR, int finalVL, int finalVR, sensors* toBe, int steps) {

  // iVL and fVL must be of the same sign
  
  // stop (or better, go normal) if initial and final are equal

  int stepVL = abs(initialVL-finalVL)/steps;
  int stepVR = abs(initialVR-finalVR)/steps;
  bool positiveL = (initialVL < finalVL);
  bool positiveR = (initialVR < finalVR);
  int newVL;
  int newVR;
  sensors current;
  sensors initial;
  sensors toBeStep;
  sensorsGetOneStep(toBe, &toBeStep, steps);

  encodersGet(&current);
  encodersSet(&initial, current.encodersL, current.encodersR);
  while(1) {
    newVL = initialVL;
    newVR = initialVR;

    changeVelocity((newVL-stepVL), (newVR-stepVR), newVL, newVR, &toBeStep, &toBeStep);
    
    /* Calculating new speed */
    if (positiveL) {
      if (newVL+stepVL <= finalVL) initialVL += stepVL;
      if (newVR+stepVR <= finalVR) initialVR += stepVR;
    } else {
      if (newVL-stepVL >= finalVL) initialVL -= stepVL;
      if (newVR-stepVL >= finalVR) initialVR -= stepVR;
    }

    encodersGet(&current);
    if (!sensorsToBe(&current, &initial, toBe)) return;
  }
}

int initSocket() {
  struct sockaddr_in s_addr;
  if (sock != -1) {
    close(sock);
    sock = -1;
  }

  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    fprintf(stderr, "Failed to create socket1\n");
    exit(1);
  }
  
  while (1) {
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    s_addr.sin_port = htons(55443);

    if (connect(sock, (struct sockaddr *) &s_addr, sizeof(s_addr)) >= 0) {
      /* connection succeeded */
      printf("Connection succeeded\n");
      return sock;
      return;
    }
    sleep(1);
    printf(".");
    fflush(stdout);
  }
}

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
  writeCmd(buf, strlen(buf)); memset(buf, 0, 80); read(sock, buf, 80);
}

bool sensorToBe(int current, int initial, int toBe ) {
  int l,r;

  if (abs(abs(current) - abs(initial)) >= abs(toBe)) // TODO
    return false;

  return true; 
}

bool sensorsToBe(sensors* Sensors, sensors* initial, sensors* toBe) {
  // TODO SENSOR CHECKING
  bool encoders;
  if (toBe->encodersL != 0 || toBe->encodersR != 0) {
    encoders = encodersToBe(Sensors, initial, toBe);
  }
  
  if (!encoders) return false;
  else return true;
}

void parseCmd (char* buf, char* elaborated[], int funcNumber, sensors* Sensors) {
  // implement expectations
  int i = 0;
  int sensorInvolved = 0;
  elaborated[i] = strtok (buf," ");
  
  if (!strcmp(elaborated[i], ".\n")) return;
  while (elaborated[i++] != NULL) {
    elaborated[i] = strtok (NULL, " ");
  }
//  if (!funcNumber) return; // if 0 no function if 9 this function...
  
  if (!strcmp(elaborated[0], "W")) {
    memset(elaborated, 0, 80);
    memset(buf, 0, 80);
    read(sock, buf, 80);
    return parseCmd (buf, elaborated, funcNumber, Sensors);
  }

  if (funcNumber == SMELR || (!strcmp(elaborated[0], "S") && !strcmp(elaborated[1], "MELR"))) {
    encodersParse(elaborated, Sensors);
    sensorInvolved++;
  }

  if (sensorInvolved == 0)
    printf("PARSE %s %s %s %s\n", elaborated[0], elaborated[1], elaborated[2], elaborated[3]);
}

void moveAtVoltage(int voltage1, int voltage2) {
  stopIf(!inLimit(voltage1) || !inLimit(voltage2));
  sprintf(buf, "M LR %i %i\n", voltage1, voltage2);
  #ifdef DEBUG
  printf("M LR %i %i\n", voltage1, voltage2);
  #endif
  nextCmd();
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