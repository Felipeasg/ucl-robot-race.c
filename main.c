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


bool
frontsAreEqual = false,
frontLInfinite = false,
frontRInfinite = false,
frontsAreInfinite = false,
frontLisCloser = false,
frontLRisk = false,
frontRRisk = false,

sidesAreEqual = false,
sideLInfinite = false,
sideRInfinite = false,
sidesAreInfinite = false,
sideLisCloser = false,
sideLRisk = false,
sideRRisk = false,

backLisInfinite = false,
backRisInfinite = false,
backsAreInfinite = false,
backLisCloser = false,
backLRisk = false,
backRRisk = false,

isRotatingL = false,
isRotatingR = false;

double checkFront(bool accurate) {

  moveILR(60,-60);
  rangeFGet(&r.s);
  frontsAreEqual = ((r.s.rangeFL - r.s.rangeFL) <= 5);
  frontLInfinite = (r.s.rangeFL == 100);
  frontRInfinite = (r.s.rangeFR == 100 );
  frontsAreInfinite = (frontLInfinite == true && frontRInfinite == true);
  frontLisCloser = (r.s.rangeFL < r.s.rangeFR);
  frontLRisk = (r.s.rangeFL < 20);
  frontRRisk = (r.s.rangeFR < 20);
  
  if (frontsAreEqual && frontsAreInfinite && !accurate) return 100;
  else if (frontsAreEqual && !frontsAreInfinite) return rangeFRFront(r.s.rangeFR);
  else if (!frontsAreEqual && !accurate) return min(rangeFLFront(r.s.rangeFL),rangeFRFront(r.s.rangeFR));
  
  usGet(&r.s);
  return min(r.s.us, min(rangeFLFront(r.s.rangeFL),rangeFRFront(r.s.rangeFR)));
}

dist checkBack(bool accurate) {
  rangeSGet(&r.s);
  dist back = (dist){r.s.rangeSL,r.s.rangeSR};
  backLisInfinite = (r.s.rangeSL == 38);
  backRisInfinite = (r.s.rangeSR == 38);
  backsAreInfinite = (backLisInfinite && backRisInfinite);
  backLisCloser = (r.s.rangeSL < r.s.rangeSR);
  backLRisk = (r.s.rangeSL < 20);
  backRRisk = (r.s.rangeSR < 20);
  setWall();
  return back;
}

dist checkSide(bool accurate, int angleL, int angleR) {

  // dist encodersDiff;
  // encodersDiff.l =  r.l.sensors[r.l.index].encodersL - r.l.sensors[prevIndex(&r.l)].encodersL;
  // encodersDiff.r =  r.l.sensors[r.l.index].encodersR - r.l.sensors[prevIndex(&r.l)].encodersR;
  // double encodersAngle = angle(encodersDiff.l, encodersDiff.r)*180/M_PI;
  // printf("Encoders angle %lf\n", encodersAngle);
  
  // if (isRotatingL) initial = (dist){.l = -60, .r = 30}; // TODO to be automagically calculated
  // if (isRotatingR) initial = (dist){.l = -30, .r = 60};
  
  rangeFGet(&r.s); // addLog(&r.s, &r.l);
  moveILR(angleL, angleR);
  
  // range triangulation
  double rFRS = rangeFRSide(r.s.rangeFR);
  double rFLS = rangeFLSide(r.s.rangeFL);
  double rFRF = rangeFRFront(r.s.rangeFR);
  double rFLF = rangeFLFront(r.s.rangeFL);
//  printf("ANGLE %i rangeFL %i rangeFR %i  rFLS %lF rFRS %lF rFLF %lF rFRF %lF\n" , r.s.rangeFRAngle, r.s.rangeFL, r.s.rangeFR, rFLS, rFRS, rFLF, rFRF);
//  printf("rangeSL %i rangeSR %i\n", r.s.rangeSL, r.s.rangeSR);

  sidesAreEqual = ((r.s.rangeFL - r.s.rangeFL) <= 5);
  sideLInfinite = (r.s.rangeFL == 100);
  sideRInfinite = (r.s.rangeFR == 100 );
  sidesAreInfinite = (sideLInfinite == true && sideRInfinite == true);
  sideLisCloser = (r.s.rangeFL < r.s.rangeFR);
  sideLRisk = (rangeFLSide(r.s.rangeFL) < 20);
  sideRRisk = (rangeFRSide(r.s.rangeFR) < 20);

  setWall();
  return (dist){ rangeFLSide(r.s.rangeFL), rangeFRSide(r.s.rangeFR)};
}

int setWall() {
  if (r.s.wall == 0) r.s.wall = (r.s.rangeSL <= r.s.rangeSR) ? LEFT : RIGHT;
  if (r.s.wall == 0) r.s.wall = (rangeFLSide(r.s.rangeFL) <= rangeFRSide(r.s.rangeFR)) ? LEFT : RIGHT;
}

volts setVoltage(volts speed, dist scale) {
  volts nextV = {.l = (30 + speed.l) * scale.l, .r = (30 + speed.r) * scale.r };
  if (nextV.l < nextV.r) { isRotatingL = false; isRotatingR = true; }
  else if (nextV.l > nextV.r) {isRotatingL = true; isRotatingR = false; }
  else { isRotatingL = false; isRotatingR = false; }
  return nextV;
}

int main () {
  initSocket();
  encodersReset();
  sensors initial;
  double toFront;
  dist toBack;
  dist toSide;
  volts speed;
  dist scale;

  while(1) {
    // This is a new cycle, first check and then move
    cTrail();
    memcpy(&initial, &r.s, sizeof(sensors));
    moveAtVoltage(r.v.r, r.v.r);

    // Check sensors
    toFront = checkFront(ACCURATE);
    toBack = checkBack(ACCURATE);
    toSide = checkSide(ACCURATE, 0, 0);

    // This gives me the difference between the wheels
  
    int dif = 40 - toSide.l;
    int val = dif;
    if (val < 5 && val > -5) { val = 0; }
    if (val < -15) {val = -15; }

    speed = (volts){val,-val};
    
    r.v = setVoltage(speed, scale);
    printf("Speed chosen: %i %i\n", r.v.l, r.v.r );

    // toTravel -= slideE(currentV.r, nextV.l);
    // toTravelR -= slideE(currentV.r, nextV.r);
    moveAtVoltage(r.v.l, r.v.r);
    encodersGet(&r.s); addLog(&r.s, &r.l);


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
