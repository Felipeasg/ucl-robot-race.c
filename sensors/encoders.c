#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "../common.h"


void encodersCmd() {
  sprintf(buf, "S MELR\n");
  #ifdef DEBUG
  printf("S MELR\n");
  #endif
  nextCmd();
}

void encodersGet(sensors* Sensors) {
  char* elaborated[80];
  encodersCmd();
  parseCmd(buf, elaborated, SMELR, Sensors);
}

void encodersSet(sensors* Sensors, int l, int r) {
  Sensors->encodersL = l;
  Sensors->encodersR = r;
  //TODO something better than atoi
}

int encodersParse(char* elaborated[], sensors* Sensors) {
  if (elaborated[2] == NULL || elaborated[3] == NULL ||
      !strcmp(elaborated[2],"") || !strcmp(elaborated[3],"") ) return 1;

  encodersSet(Sensors, atoi(elaborated[2]), atoi(elaborated[3]));
  // it should be 0 for failure, 1 for silent, 2 for OK
  return 2;
}

void encodersReset() {
  sprintf(buf, "C RME\n");
  #ifdef DEBUG
  printf("C RME\n");
  #endif
  nextCmd();
}

/* encodersToBe
// return true when is not yet toBe, return false when it goes over toBe
*/

bool encodersToBe(sensors* current, sensors* initial, sensors* toBe) {
  int l,r;

  if (sensorToBe(current->encodersL, initial->encodersL, toBe->encodersL)) {l=0;}
  else {l=1;}
  if (sensorToBe(current->encodersR, initial->encodersR, toBe->encodersR)) {r=0;}
  else {r=1;}

  if (l == 1 && r == 1) {
    return false;
  }
  return true; 
}