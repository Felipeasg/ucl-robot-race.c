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
#define RANGEUS 200-80
#define RANGESL 40-5
#define RANGESR 40-5
#define RANGEFL 100-5
#define RANGEFR 100-5

#include "common.h"

// global vars holdin velocity

// void reposition(bool minCase, bool maxCase, current, initial, toBeMin, toBeMax) {
//   while (1) {
//     minCase = !sensorsToBe(current, initial, toBeMin);
//     maxCase = sensorsToBe(current, initial, toBeMax);
//     if (!(minCase && maxCase) ) break;
// 
//     moveStraightAtVoltage(30);
//     rangeSGet(current);
//   }
// }

// void checking (sensors* current, status* us, status* rangeFL, status* rangeFR, status* rangeSL, status* rangeSR) {
//   // Ultrasound
//   if (toBe.us <= INFINITEUS) {
//   } else {
//     us->straight = true;
//   }
// 
//   // Front
//   if (toBe.rangeFL <= INFINITEFL) {
//     rangeFL->straight = true;
//   } else {
//     rangeFL->reposition = true;
//   }
// 
//   if (toBe.rangeFR <= INFINITEFR) {
//     rangeFL->straight = true;
//   } else {
//   }
// 
//   // Side
//   if (toBe.rangeSL <= INFINITESL) {
//   } else {
//   }
// 
//   if (toBe.rangeSR <= INFINITESR) {
//   } else {
//   }
// }


int reposition(sensors* current, int encodersL, int encodersR, int voltageL, int voltageR) {
  sensors toBe = DEFAULT_SENSORS;
  sensors initial = DEFAULT_SENSORS;
  encodersSet(&initial, current->encodersL, current->encodersR);
  encodersSet(&toBe, encodersL, encodersR);

  encodersGet(current);
  while (sensorsToBe(current, &initial, &toBe)) {
    moveAtVoltage(voltageL, voltageR);
    encodersGet(current);
  }
}

// 0 if staight, 1 if reposition, 2 if spinning, 3 if stopping
int shouldReposition(sensors* current) {
  status decision = DEFAULT_STATUS;

  printf("PARSE %i %i %i %i\n", current->rangeSL, current->rangeSR, current->rangeFL, current->rangeFR);

  // US out of range
  if (current->us >= RANGEUS) {
    if (current->rangeFL <= RANGEFL || current->rangeFR <= RANGEFR ) {decision.straight = true;}
    if (current->rangeSL <= RANGESL || current->rangeSR <= RANGESR ) {decision.straight = true;}
    if (decision.straight == true) return 0;

    if (current->rangeFL <= 100) {

      // Left pivot - Right is empty && SL is touching
      if (current->rangeFR == 100 && current->rangeSR == 40 && current->rangeSL <= 40 ) {
        reposition(current, 15, 30, 15, 30);
        return 1;
      }
    } else if (current->rangeFR <= 100) {

      // Right pivot - Left is empty && SR is touching
      if (current->rangeFL == 100 && current->rangeSL == 40 && current->rangeSR <= 40 ) {
        return 1;
      }
    }

  } else

  if (current->us < RANGEUS) {

    if (current->us <= 90 && current->rangeFL < 100) {
      reposition(current, 30, 15, 30, 15);
      return 1;
    }
    if (current->rangeFL <= RANGEFL || current->rangeFR <= RANGEFR ) {decision.straight = true;}
    if (current->rangeSL <= RANGESL || current->rangeSR <= RANGESR ) {decision.straight = true;}
    if (decision.straight == true) return 0;
    
    // TODO track previous position of SL SR and turn when it is not touching

    if (current->rangeFL <= 100) {

      // Left pivot - Right is empty && SL is touching
      if (current->rangeFR == 100 && current->rangeSR == 40 && current->rangeSL <= 40 ) {
        reposition(current, 15, 30, 15, 30);
        return 1;
      }
    } else if (current->rangeFR <= 100) {

      // Right pivot - Left is empty && SR is touching
      if (current->rangeFL == 100 && current->rangeSL == 40 && current->rangeSR <= 40 ) {
        return 1;
      }
    }
    
  }
  return 3;
}

int main () {
  initSocket();

  // sensors current = DEFAULT_SENSORS;
  //  sensors initial = DEFAULT_SENSORS;
  //  sensors toBe = DEFAULT_SENSORS;
  //  sensors toBeMin = DEFAULT_SENSORS;
  //  sensors toBeMax = DEFAULT_SENSORS;
  //  toBe.rangeSL = 10;
  //  toBeMax.rangeSL = 12;
  //  toBeMin.rangeSL = 8;
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
  
  // printf("toBE values: %i\n", toBe.rangeSL);
  // printf("toBEMax values: %i\n", toBeMax.rangeSL);
  // printf("toBEMin values: %i\n", toBeMin.rangeSL);
  // 
  // while  (1) {
  //   rangeSGet(&current);
  //   printf("values: %i, %i %i %i\n", current.rangeSL, toBe.rangeSL, toBeMin.rangeSL, toBeMax.rangeSL);
  //   printf("NOR max %d min %d\n", sensorsToBe(&current, &initial, &toBeMin), sensorsToBe(&current, &initial, &toBeMax));
  //   
  //   bool minCase, maxCase;
  //   while (1) {
  //     minCase = !sensorsToBe(&current, &initial, &toBeMin);
  //     maxCase = sensorsToBe(&current, &initial, &toBeMax);
  //     if (!(minCase && maxCase) ) break;
  // 
  //     moveStraightAtVoltage(30);
  //     rangeSGet(&current);
  //   }
  //   // reposition(minCase, maxCase);
  // }
  
  sensors current = DEFAULT_SENSORS;
  sensors limits = DEFAULT_SENSORS;
  
  status usStatus = DEFAULT_STATUS;
  status rangeFLStatus = DEFAULT_STATUS;
  status rangeFRStatus = DEFAULT_STATUS;
  status rangeSLStatus = DEFAULT_STATUS;
  status rangeSRStatus = DEFAULT_STATUS;
  
  int decision = 0;

  while (1) {
    fflush(stdout);
    printf("start looping\n");
    // Go straigtasdash
    usGet(&current);
    rangeFGet(&current);
    rangeSGet(&current);
    
    decision = shouldReposition(&current);
    fflush(stdout);
    printf("decision %i\n", decision);

    if (!decision) {
      moveAtVoltage(20, 20);
    }
    
    fflush(stdout);
    printf("finish looping\n");
  }

  return 0;
}

// TODO: clean the code
// TODO: add derivative & average 