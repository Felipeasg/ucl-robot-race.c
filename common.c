#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "common.h"



#define WHEELREVOLUTION 100 * 3.14159

sensors DEFAULT_SENSORS = {
  .encodersL = 0,
  .encodersR = 0,
  .bumpersL = 0,
  .bumpersR = 0,
  .rangeFL = 0,
  .rangeFR = 0,
  .rangeFLAngle = 0,
  .rangeFRAngle = 0,
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

int sock = -1;

robot r = {
  .s = {
    .encodersL = 0,
    .encodersR = 0,
    .bumpersL = 0,
    .bumpersR = 0,
    .rangeFL = 0,
    .rangeFR = 0,
    .rangeSL = 0,
    .rangeSR = 0,
    .us = 0
  },
  .v = (volts){r: 20, l: 20},
  .l = (logs){.index = -1, .empty = true, .wall=-1}
};

double absDouble(double i) {
  if (i<0) return -i;
  return i;
}

bool inLimit(int voltage) { if (voltage <= 127 && voltage >= -127) return true; else return false; }

int abs (int value) {
  if (value < 0) return -value;
  return value;
}

int getProportion(int number, int proportion) {
  return (proportion/100)*number;
}


void considerSlide(int fromVL, int fromVR, int toVL, int toVR, sensors* Sensors) {
  int slideVL = slideE(fromVL, toVL);
  int slideVR = slideE(fromVR, toVR);

  Sensors->encodersL -= slideVL;
  Sensors->encodersR -= slideVR;
}

void changeVelocity (int fromVL, int fromVR, int toVL, int toVR, sensors* toBeInitial, sensors* toBeFinal) { //TODO pass left and right
  sensors current = DEFAULT_SENSORS;
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

void sensorsGetOneStep(sensors* Sensors, sensors* New, int steps) {

  // TODO use sensorsSet instead
  if (Sensors->encodersL != 0) New->encodersL = Sensors->encodersL/steps;
  if (Sensors->encodersR != 0) New->encodersR = Sensors->encodersR/steps;
  if (Sensors->rangeSL != 0) New->rangeSL = Sensors->rangeSL/steps;
  if (Sensors->rangeSR != 0) New->rangeSR = Sensors->rangeSR/steps;
  if (Sensors->rangeFL != 0) New->rangeFL = Sensors->rangeFL/steps;
  if (Sensors->rangeFR != 0) New->rangeFR = Sensors->rangeFR/steps;
  if (Sensors->us != 0) New->us = Sensors->us/steps;
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

void sensorsDifference(sensors* last, sensors* prev, sensors* new) {
  new->encodersL = last->encodersL - prev->encodersL;
  new->encodersR = last->encodersR - prev->encodersR;
  new->rangeSL = last->rangeSL - prev->rangeSL;
  new->rangeSR = last->rangeSR - prev->rangeSR;
  new->rangeFL = last->rangeFL - prev->rangeFL;
  new->rangeFR = last->rangeFR - prev->rangeFR;
  new->us = last->us - prev->us;
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

bool sensorToBe(int current, int initial, int toBe ) {

  if (abs(abs(current) - abs(initial)) >= abs(toBe)) // TODO
    return false;

  return true; 
}

bool sensorsToBe(sensors* Sensors, sensors* initial, sensors* toBe) {
  // TODO SENSOR CHECKING
  bool encoders = false, bumpers = false, rangeF = false, rangeS = false, us = false;

  if (toBe->encodersL != 0 || toBe->encodersR != 0) { encoders = encodersToBe(Sensors, initial, toBe); }
  if (toBe->rangeFL != 0 || toBe->rangeFR != 0) { rangeF = rangeFToBe(Sensors, initial, toBe); }
  if (toBe->rangeSL != 0 || toBe->rangeSR != 0) { rangeS = rangeSToBe(Sensors, initial, toBe); }
  if (toBe->us != 0) { us = usToBe(Sensors, initial, toBe); }
  bumpers = bumpersToBe(Sensors, toBe); //TODO improve this

  if (!encoders && !rangeF && !rangeS && !bumpers && !us) return false;
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
  if (funcNumber == SBFLR || (!strcmp(elaborated[0], "S") && !strcmp(elaborated[1], "SBFLR"))) {
    bumpersParse(elaborated, Sensors);
    sensorInvolved++;
  }
  if (funcNumber == SIFLR || (!strcmp(elaborated[0], "S") && !strcmp(elaborated[1], "SIFLR"))) {
    rangeFParse(elaborated, Sensors);
    sensorInvolved++;
  }
  if (funcNumber == SISLR || (!strcmp(elaborated[0], "S") && !strcmp(elaborated[1], "SISLR"))) {
    rangeSParse(elaborated, Sensors);
    sensorInvolved++;
  }
  if (funcNumber == SUS || (!strcmp(elaborated[0], "S") && !strcmp(elaborated[1], "US"))) {
    usParse(elaborated, Sensors);
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

void cTrail() {
  sprintf(buf, "C TRAIL\n");
  #ifdef DEBUG
  printf("C TRAIL\n");
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

void logsLastDifference(logs* l, sensors* New) {
  int current = l->index;
  int previous = (l->index-1 >= 0) ? l->index-1 : 19;
  sensorsDifference(&l->sensors[l->index], &l->sensors[l->index-1], New);
}

void addLog(sensors* s, logs* l) {
  if (++(l->index) > 19 ) {
    if (l->empty == true) l->empty = false;
    l->index = 0;
  }
  memcpy(&l->sensors[l->index], s, sizeof(sensors));
  //printf("index %i eg: %i \n", l->index, l->sensors[l->index].us);
}

void printLogs(logs* l) {
  int i;
  int current = (l->empty == true) ? 0 : l->index+1;
  for (i=0; i < 20; i++) {
    printf("Log %i at index %i range: %i at angle: %i\n", i, current, l->sensors[current].rangeFL, l->sensors[current].rangeFLAngle);
    if (l->empty == true && current == l->index) break;
    if (l->empty == false && current == 19) current = -1;
    current++;
    
  }
}
// TODO
// turn very slowly
// move as fast as he can.