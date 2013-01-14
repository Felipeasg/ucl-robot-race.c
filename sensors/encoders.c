#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "../common.h"


void encordersCmd() {
  sprintf(buf, "S MELR\n");
  printf("S MELR\n");
  nextCmd();
}

void encodersGet(sensors* Sensors) {
  char* elaborated[80];
  encordersCmd();
  parseCmd(buf, elaborated, SMELR, Sensors);
}

int encodersParse(char* elaborated[], sensors* Sensors) {
  if (elaborated[2] == NULL || elaborated[3] == NULL ||
      !strcmp(elaborated[2],"") || !strcmp(elaborated[3],"") ) return 1;
  Sensors->encodersL = atoi(elaborated[2]);
  Sensors->encodersR = atoi(elaborated[3]); //TODO something better than atoi
  // it should be 0 for failure, 1 for silent, 2 for OK
  return 2;
}

bool encodersToBe(sensors* Sensors, sensors* initial, sensors* toBe) {
  int l,r;
  if (abs(Sensors->encodersL - initial->encodersL) >= toBe->encodersL) {l=1;}
  else {l=0;}
  if (abs(Sensors->encodersR - initial->encodersR) >= toBe->encodersR) {r=1;}
  else {r=0;}

  if (l == 1 && r == 1) {
    return false;
  }
  return true; 
}