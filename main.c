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

bool rangesAreEqual, rangesAreInfinite, rangeLInfinite, rangeRInfinite;

double checkFront(bool accurate) {
  moveILR(60,-60);
  sleep(1);
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

double checkSide(bool accurate) {
  
  bool rangesAreEqual, rangesAreInfinite, rangeLInfinite, rangeRInfinite;
  moveILR(60,-60);
  sleep(1);
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
  int speed = 0, i;
  double toTravel, toTravelR;
  sensors initial = DEFAULT_SENSORS;

  while(1) {  cTrail();
    memcpy(&initial, &r.s, sizeof(sensors));
    toTravel = checkFront(ACCURATE)/T2CM;
    
    r.v = (volts){.l=30, .r=30};

    encodersGet(&r.s);
    rangeSGet(&r.s);

    fflush(stdout); printf(KRED "I can travel %i (%lF encoders)\n"KNRM,r.s.us, toTravel/T2CM);

    printLogs(&r.l);

    r.v.l = 30;
    r.v.r = 30;
    volts currentV = {r.v.l,r.v.r};
    while (sensorToBe(r.s.encodersL, initial.encodersL, toTravel)) {
      toTravel -= slideE(currentV.r, r.v.l+speed);
      toTravelR -= slideE(currentV.r, r.v.r-speed);
      currentV =  (volts){.l = r.v.l + speed, .r = r.v.r - speed };
      moveAtVoltage(currentV.l, currentV.r);

      encodersGet(&r.s);
      addLog(&r.s, &r.l);
      printf("Actual: %i and %lF (%lF)\n", r.s.encodersL-initial.encodersL, (double)500/(r.s.encodersL-initial.encodersL), toTravel);
    }
  }

  return 0;
}
