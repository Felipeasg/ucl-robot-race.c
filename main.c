#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>

#define DEBUG 1
#include "common.h"

// global vars holdin velocity

int main () {
  initSocket();

  sensors toBeTravelled1 = DEFAULT_SENSORS;
  toBeTravelled1.encodersL = 200;
  toBeTravelled1.encodersR = 200;
//  sensors toBeTravelled2 = {.encodersL = 80, .encodersR = 80 };
  encodersReset();
  // 
//  considerSlide(0, 0, 100, 100, &toBeTravelled1);
//  considerSlide(100, 100, 50, 50, &toBeTravelled2);
//  changeVelocity(100, 100, 50, 50, &toBeTravelled1, &toBeTravelled2);
//  stopMovement();

  sensors current = DEFAULT_SENSORS;
  sensors initial = DEFAULT_SENSORS;
  while(sensorsToBe(&current, &initial, &toBeTravelled1)) {
    moveAtVoltage(50, 50);
    encodersGet(&current);
  }


  // considerSlide(0, 0, 50, 50, &toBeTravelled1);
  // considerSlide(100, 100, 0, 0, &toBeTravelled1);
  // constAcceleration (0, 0, 100, 100, &toBeTravelled1, 20);

  return 0;
}