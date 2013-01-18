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

void reposition(bool minCase, bool maxCase) {
  
}

int main () {
  initSocket();

  sensors current = DEFAULT_SENSORS;
  sensors initial = DEFAULT_SENSORS;
  sensors toBe = DEFAULT_SENSORS;
  sensors toBeMin = DEFAULT_SENSORS;
  sensors toBeMax = DEFAULT_SENSORS;
  toBe.rangeSL = 10;
  toBeMax.rangeSL = 12;
  toBeMin.rangeSL = 8;
  bool minCase, maxCase;
  // sensors toBeTravelled1 = DEFAULT_SENSORS;
  // toBeTravelled1 = (sensors){.encodersL = 800, .encodersR = 800 };

//  sensors toBeTravelled2 = {.encodersL = 80, .encodersR = 80 };
  encodersReset();

 // considerSlide(0, 0, 100, 100, &toBeTravelled1);
 // considerSlide(100, 100, 50, 50, &toBeTravelled2);
 // changeVelocity(100, 100, 50, 50, &toBeTravelled1, &toBeTravelled2);
 // stopMovement();

  // considerSlide(0, 0, 100, 100, &toBeTravelled1);
  // considerSlide(100, 100, 0, 0, &toBeTravelled1);
  // constAcceleration (0, 0, 100, 100, &toBeTravelled1, 20);
  // stopMovement();
  // encodersGet(&current);
  
  printf("toBE values: %i\n", toBe.rangeSL);
  printf("toBEMax values: %i\n", toBeMax.rangeSL);
  printf("toBEMin values: %i\n", toBeMin.rangeSL);
  while  (1) {
    rangeSGet(&current);
    printf("values: %i, %i %i %i\n", current.rangeSL, toBe.rangeSL, toBeMin.rangeSL, toBeMax.rangeSL);
    
    printf("NOR max %d min %d\n", sensorsToBe(&current, &initial, &toBeMin), sensorsToBe(&current, &initial, &toBeMax));
    
    while (1) {
      minCase = !sensorsToBe(&current, &initial, &toBeMin);
      maxCase = sensorsToBe(&current, &initial, &toBeMax);
      if (!(minCase && maxCase) ) break;

      moveStraightAtVoltage(30);
      rangeSGet(&current);
    }
    reposition(minCase, maxCase);
  }
  

  return 0;
}