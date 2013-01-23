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


void reposition(robot* r, int encodersL, int encodersR, int voltageL, int voltageR) {
  sensors toBe = DEFAULT_SENSORS;
  sensors initial = DEFAULT_SENSORS;
  encodersSet(&initial, r->s.encodersL, r->s.encodersR);
  encodersSet(&toBe, encodersL, encodersR);

  encodersGet(&r->s);
  while (sensorsToBe(&r->s, &initial, &toBe)) {
    moveAtVoltage(voltageL, voltageR);
    encodersGet(&r->s);
  }
}

// 0 if staight, 1 if reposition, 2 if spinning, 3 if stopping
int shouldReposition(robot* r) {
  sensors* s = &r->s;
  volts* v = &r->v;
  status decision = DEFAULT_STATUS;

  printf("PARSE sl%i sr%i fl%i fr%i us%i\n", s->rangeSL, s->rangeSR, s->rangeFL, s->rangeFR, s->us);

  if (s->us <= 100) {
    if (s->rangeFL < 100) {
      fflush(stdout);
      printf("close US, reposition FL\n");
      reposition(r, 30, 15, 30, 15);
      return 1;
    } else if (s->rangeFR < 100) {
      fflush(stdout);
      printf("close US, reposition FR\n");
      reposition(r, 15, 30, 15, 30);
      return 1;
    }
    return 0;
  }
  
  // If rangeFL/FR in range or rangeSL/SR in range
  if (s->rangeFL <= RANGEFL || s->rangeFR <= RANGEFR ) {decision.straight = true;}
  if (s->rangeSL <= RANGESL || s->rangeSR <= RANGESR ) {decision.straight = true;}
  if (decision.straight == true) {
    fflush(stdout);
    printf("we can go straight\n");
    return 0;
  }
  
  // TODO track previous position of SL SR and turn when it is not touching


    
    if (s->rangeFL >= 85) {
      if (s->rangeFR == 100 && s->rangeSR == 40) {
        fflush(stdout);
         printf("FL pivot\n");
         reposition(r, 15, 30, 15, 30);
         return 1;
      }
    }
    
    if (s->rangeFR >= 85) {
      if (s->rangeFR == 100 && s->rangeSR == 40) {
        fflush(stdout);
         printf("FR pivot\n");
         reposition(r, 15, 30, 15, 30);
         return 1;
      }
    }

    // LeftS pivot - Right is empty && SL is touching
    if (s->rangeFR == 100 && s->rangeSR == 40 && s->rangeSL < 40 ) {
      fflush(stdout);
      printf("SL pivot\n");
      reposition(r, 15, 30, 15, 30);
      return 1;
    }


    // Right pivot - Left is empty && SR is touching
    if (s->rangeFL == 100 && s->rangeSL == 40 && s->rangeSR < 40 ) {
      fflush(stdout);
      printf("SR pivot\n");
      reposition(r, 30, 15, 30, 15);
      return 1;
    }

    
  return 3;
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

void logsLastDifference(logs* l, sensors* New) {
  sensorsDifference(&l->sensors[l->index], &l->sensors[l->index-1], New);
}

void addLog(sensors* s, logs* l) {
  if (++(l->index) > 4 ) {
    if (l->empty == true) l->empty = false;
    l->index = 0;
  }
  memcpy(&l->sensors[l->index], s, sizeof(sensors));
  printf("index %i eg: %i \n", l->index, l->sensors[l->index].us);
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
  
  robot r = {.s= DEFAULT_SENSORS, .v= (volts){r: 20, l: 20}};
  sensors limits = DEFAULT_SENSORS;
  
  status usStatus = DEFAULT_STATUS;
  status rangeFLStatus = DEFAULT_STATUS;
  status rangeFRStatus = DEFAULT_STATUS;
  status rangeSLStatus = DEFAULT_STATUS;
  status rangeSRStatus = DEFAULT_STATUS;
  logs l;
  l.index = -1;
  l.empty = true;
  
  int decision = 0;

  while (1) {
    
    usGet(&r.s);
    rangeFGet(&r.s);
    rangeSGet(&r.s);
    addLog(&r.s, &l);
    
    // decision = shouldReposition(&r);
    // 
    // if (!decision) {
    //   move(&r.v);
    // }
    printf("{us: %i,rangeSL: %i,rangeSR: %i,rangeFL: %i,rangeFR: %i}",r.s.us,r.s.rangeSL,r.s.rangeSR,r.s.rangeFL,r.s.rangeFR);
  }

  return 0;
}

// TODO: clean the code
// TODO: add derivative & average 