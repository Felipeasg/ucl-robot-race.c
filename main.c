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

double min(double one, double two) {
  return (one < two) ? one : two;
}

double checkFront(bool accurate) {
  bool rangesAreEqual, rangesAreInfinite, rangeLInfinite, rangeRInfinite;
  moveILR(60,-60);
  rangeFGet(&r.s);
  rangesAreEqual = (r.s.rangeFL - r.s.rangeFL) <= 5 ? true : false;
  rangeLInfinite = r.s.rangeFL == 92 ? true : false;
  rangeRInfinite = r.s.rangeFR == 92 ? true : false;
  rangesAreInfinite = rangeLInfinite == true && rangeRInfinite == true ? true : false;
  
  if (rangesAreEqual && rangesAreInfinite && !accurate) return 92;
  else if (rangesAreEqual && !rangesAreInfinite) return rangeFRFront(r.s.rangeFR);
  else if (!rangesAreEqual && !accurate) return min(rangeFLFront(r.s.rangeFL),rangeFRFront(r.s.rangeFR));
  
  usGet(&r.s);
  return min(r.s.us, min(rangeFLFront(r.s.rangeFL),rangeFRFront(r.s.rangeFR)));
}

int main () {
  initSocket();
  encodersReset();
  int count, speed = 0, i;
  double toTravel, toTravelL, toTravelR, newTravel, tempTravel;
  toTravelL = toTravelR = newTravel = tempTravel = -1;
  sensors initial = DEFAULT_SENSORS;
  

    // counts = 120;
    // printf("Expected: %lF and slides %lF,%i, %lf, %lf\n", count * 30* 0.156, slideE(0,30), 30, T2CM, 30/T2CM);

  while(1) {
    cTrail();
    memcpy(&initial, &r.s, sizeof(sensors));
    encodersGet(&r.s);
    toTravel = checkFront(ACCURATE)/T2CM;
    rangeSGet(&r.s);
    fflush(stdout);
    printf(KRED "I can travel %i (%lF encoders)\n"KNRM,r.s.us, toTravel/T2CM);
    
    for (i=1; i<10; i++) {
      rangeFGet(&r.s); addLog(&r.s, &r.l); moveILR(-45+i*10, 45-i*10);
      double rFRS = rangeFRSide(r.s.rangeFR); double rFLS = rangeFLSide(r.s.rangeFL);
      double rFRF = rangeFRFront(r.s.rangeFR); double rFLF = rangeFLFront(r.s.rangeFL);
      
    }

    
    printLogs(&r.l);

    count = 0;
    r.v.l = 30;
    r.v.r = 30;
    volts currentV = {r.v.l,r.v.r};
    while (sensorToBe(r.s.encodersL, initial.encodersL, toTravel)) {
      toTravel -= slideE(currentV.r, r.v.l+speed);
      toTravelR -= slideE(currentV.r, r.v.r-speed);
      currentV.l = r.v.l + speed;
      currentV.r = r.v.r - speed;
      moveAtVoltage(30, 30);
      rangeFGet(&r.s);
      encodersGet(&r.s);
      addLog(&r.s, &r.l);
      printf("Actual: %i and %i %lF (%lF)\n", r.s.encodersL-initial.encodersL, count, (double)500/(r.s.encodersL-initial.encodersL), toTravel);
      count++;
    }
  }
    

  
  printf("Actual: %i\n", r.s.encodersL);

  return 0;
}
