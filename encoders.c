#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "common.h"


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

void encodersParse(char* elaborated[], sensors* Sensors) {
  Sensors->encodersL = atoi(elaborated[2]);
  Sensors->encodersR = atoi(elaborated[3]);
}

bool encordersToBe(sensors* Sensors, sensors* initial, sensors* toBe) {
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