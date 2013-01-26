#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include <math.h>
#include "common.h"
#include "robot.h"
#include "rangefinders.h"

#define ACCURATE true
#define INACCURATE false

bool rangesAreEqual = false, rangesAreInfinite= false, rangeLInfinite= false, rangeRInfinite= false, isRotatingL= false, isRotatingR = false;

double checkFront(bool accurate) {

  moveILR(60,-60);
  sleep(1);
  rangeFGet(&r.s);
  rangesAreEqual = (r.s.rangeFL - r.s.rangeFL) <= 5 ? true : false;
  rangeLInfinite = r.s.rangeFL == 100 ? true : false;
  rangeRInfinite = r.s.rangeFR == 100 ? true : false;
  rangesAreInfinite = rangeLInfinite == true && rangeRInfinite == true ? true : false;
  
  if (rangesAreEqual && rangesAreInfinite && !accurate) return 100;
  else if (rangesAreEqual && !rangesAreInfinite) return rangeFRFront(r.s.rangeFR);
  else if (!rangesAreEqual && !accurate) return min(rangeFLFront(r.s.rangeFL),rangeFRFront(r.s.rangeFR));
  
  usGet(&r.s);
  return min(r.s.us, min(rangeFLFront(r.s.rangeFL),rangeFRFront(r.s.rangeFR)));
}

dist checkSide(bool accurate, int i) {
  dist initial = {.l = -45, .r =45};
  
  if (isRotatingL) initial = (dist){.l = -60, .r = 30}; // TODO to be automagically calculated
  if (isRotatingR) initial = (dist){.l = -30, .r = 60};
  
  sleep(1);
  rangeFGet(&r.s); addLog(&r.s, &r.l);
  moveILR(initial.l+i*10, initial.r-i*10);
  
  // range triangulation
  double rFRS = rangeFRSide(r.s.rangeFR);
  double rFLS = rangeFLSide(r.s.rangeFL);
  double rFRF = rangeFRFront(r.s.rangeFR);
  double rFLF = rangeFLFront(r.s.rangeFL);
  printf("ANGLE %i rangeFL %i rangeFR %i  rFLS %lF rFRS %lF rFLF %lF rFRF %lF\n" , r.s.rangeFRAngle, r.s.rangeFL, r.s.rangeFR, rFLS, rFRS, rFLF, rFRF);
  printf("rangeSL %i rangeSR %i\n", r.s.rangeSL, r.s.rangeSR);

  return (dist){ rangeFLSide(r.s.rangeFL), rangeFRSide(r.s.rangeFR)};
}

int setWall() {
  if (r.s.wall == 0) r.s.wall = (r.s.rangeSL <= r.s.rangeSR) ? LEFT : RIGHT;
  printf("\n1wall: %i sl %i sr %i\n\n", r.s.wall, r.s.rangeSL, r.s.rangeSR);
  if (r.s.wall == 0) r.s.wall = (rangeFLSide(r.s.rangeFL) <= rangeFRSide(r.s.rangeFR)) ? LEFT : RIGHT;
  printf("\n2wall: %i\n\n", r.s.wall);
}

int main () {
  initSocket();
  encodersReset();
  int scaleVal = 1, i, j;
  double toTravel, toTravelR;
  dist toTravelL;
  sensors initial = DEFAULT_SENSORS;
  volts speed = {.l=0,.r=0}, scale = {.l=0,.r=0};
  r.v = (volts){.l=30, .r=30};

  while(1) {  cTrail();
    memcpy(&initial, &r.s, sizeof(sensors));
    toTravel = checkFront(ACCURATE)/T2CM;
    
    sleep(1); rangeSGet(&r.s); setWall();
    for (j=0; j <5; j++) {
      toTravelL = checkSide(ACCURATE, j); setWall();
    }
    printf("Let's see %lF\n", toTravelL.l);

    // encodersGet(&r.s);
    // rangeSGet(&r.s);
    // 
    // fflush(stdout); printf(KRED "I can travel %i (%lF encoders)\n"KNRM,r.s.us, toTravel/T2CM);
    // 
    // printLogs(&r.l);
    // 
    // r.v.l = 30;
    // r.v.r = 30;
    // volts currentV = {r.v.l,r.v.r};
    // volts nextV;
    // while (sensorToBe(r.s.encodersL, initial.encodersL, toTravel)) {
    //   nextV =  (volts){.l = (r.v.l + speed.l) * scale.l, .r = (r.v.r - speed.r) * scale.r };
    //   toTravel -= slideE(currentV.r, nextV.l);
    //   toTravelR -= slideE(currentV.r, nextV.r);
    //   memcpy(&currentV, &nextV, sizeof(volts));
    //   moveAtVoltage(currentV.l, currentV.r);
    // 
    //   encodersGet(&r.s);
    //   addLog(&r.s, &r.l);
    //   printf("Actual: %i and %lF (%lF)\n", r.s.encodersL-initial.encodersL, (double)500/(r.s.encodersL-initial.encodersL), toTravel);
    // }
  }

  return 0;
}
