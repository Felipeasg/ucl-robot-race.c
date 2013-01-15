#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <regex.h>

#define DEBUG 1
#include "common.h"

int main () {
  initSocket();
  
  sensors initial;
  sensors current;
  sensors toBe;


  toBe.encodersL = 199;
  toBe.encodersR = 199;
  encodersGet(&initial);
  encodersGet(&current);
  while (sensorsToBe(&current, &initial, &toBe, SMELR)) {
    turnOnSpotAtVoltage(20);
    encodersGet(&current);
  }

  toBe.encodersL = 500;
  toBe.encodersR = 500;
  encodersGet(&initial);
  encodersGet(&current);
  while (sensorsToBe(&current, &initial, &toBe, SMELR)) {
    moveStraightAtVoltage(20);
    encodersGet(&current);
  
  }
  stopMovement();

  return 0;
}