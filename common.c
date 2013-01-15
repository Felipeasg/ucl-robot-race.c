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

int abs (int value) {
  if (value < 0) return -value;
  return value;
}

int getProportion(int number, int proportion) {
  return (proportion/100)*number;
}

void constAcceleration (int initialL, int initialR, int finalL, int finalR, sensors* toBeInitial, sensors* toBeFinal) { //TODO pass left and right
  sensors current;
  sensors initial;

  encodersGet(&current);
  encodersGet(&initial);
  while(sensorsToBe(&current, &initial, toBeInitial)) { //TODO sensorsToBe instead
    moveAtVoltage(initialL, initialR);
    encodersGet(&current);
  }

  encodersGet(&initial);
  while(sensorsToBe(&current, &initial, toBeFinal)) {
    moveAtVoltage(finalL, finalR);
    encodersGet(&current);
  }
}

void initSocket() {
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    fprintf(stderr, "Failed to create socket1\n");
    exit(1);
  }
  
  s_addr.sin_family = AF_INET;
  s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  s_addr.sin_port = htons(55443);
  
  if (connect(sock, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0) {
    fprintf(stderr, "Failed to create socket2\n");
    exit(1);
  }

  sleep(2);
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

void nextCmd() {
  write(sock, buf, strlen(buf)); memset(buf, 0, 80); read(sock, buf, 80);
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
  int i = 0;
  elaborated[i] = strtok (buf," ");
  
  if (!strcmp(elaborated[i], ".\n")) return;
  while (elaborated[i++] != NULL) {
    elaborated[i] = strtok (NULL, " ");
  }
  
  #ifdef DEBUG
  printf("PARSE %s %s\n", elaborated[0], elaborated[1]);
  printf("PARSE %s %s\n", elaborated[2], elaborated[3]);
//  if (!funcNumber) return; // if 0 no function if 9 this function...
  #endif
  
  if (!strcmp(elaborated[0], "W")) {
    memset(elaborated, 0, 80);
    memset(buf, 0, 80);
    read(sock, buf, 80);
    return parseCmd (buf, elaborated, funcNumber, Sensors);
  }

  if (funcNumber == SMELR || (!strcmp(elaborated[0], "S") && !strcmp(elaborated[1], "MELR"))) {
    encodersParse(elaborated, Sensors);
  }
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