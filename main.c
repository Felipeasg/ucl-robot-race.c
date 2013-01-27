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

#define CHECKFRONT 1
#define CHECKSIDE 2
#define CHECKBACK 3

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

isTurningR = false,
isTurningL = false,

isRotatingL = false,
isRotatingR = false;

int properRotation = 0;
dist angleWall;

int backConsidered, backDif; dist backVal;
const int backOk=20, backRange=5, backMinimum=15;

int sideConsidered, sideDif, sideVal;
const int sideOk=30, sideRange=5, sideMinimum=8;

int frontConsidered, frontDif, frontVal;
const int frontOk=30, frontRange=5, frontMinimum=8;

request req = {
  .checkFront = true,
  .checkBack = true,
  .checkSide = true,
  .checkEncoders = true,
  
  .calculateBack = true,
  .calculateSide = true,
  .calculateFront = true,
  .setWallAuto = true
};

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
  backLisInfinite = (r.s.rangeSL == 38);
  backRisInfinite = (r.s.rangeSR == 38);
  backsAreInfinite = (backLisInfinite && backRisInfinite);
  backLisCloser = (r.s.rangeSL < r.s.rangeSR);
  backLRisk = (r.s.rangeSL < 15);
  backRRisk = (r.s.rangeSR < 15);
  setWall();
  return (dist){r.s.rangeSL,r.s.rangeSR};
}

double wallAngle (int b, int c, int A) {
  return asin( (c * sin(A)) / (sqrt( pow(b,2) + pow(c,2) - 2*b*c ) ) );
}

dist checkSide(bool accurate, int angleL, int angleR) {
  sensors horizontal;

  // dist encodersDiff;
  // encodersDiff.l =  r.l.sensors[r.l.index].encodersL - r.l.sensors[prevIndex(&r.l)].encodersL;
  // encodersDiff.r =  r.l.sensors[r.l.index].encodersR - r.l.sensors[prevIndex(&r.l)].encodersR;
  // double encodersAngle = angle(encodersDiff.l, encodersDiff.r)*180/M_PI;
  // printf("Encoders angle %lf\n", encodersAngle);
  
  // if (isRotatingL) initial = (dist){.l = -60, .r = 30}; // TODO to be automagically calculated
  // if (isRotatingR) initial = (dist){.l = -30, .r = 60};
  
  // Tracking horizontal
  moveILR(-45, 45);
  rangeFGet(&horizontal);
  printf("\n UUUUUUU %i %i\n\n", horizontal.encodersL, horizontal.encodersR);
  
  // Tracking wanted angle
  moveILR(angleL, angleR);
  rangeFGet(&r.s); // addLog(&r.s, &r.l);
  
  // Range trigs
  double rFRS = rangeFRSide(r.s.rangeFR);
  double rFLS = rangeFLSide(r.s.rangeFL);
  double rFRF = rangeFRFront(r.s.rangeFR);
  double rFLF = rangeFLFront(r.s.rangeFL);

  // Setting bools
  sidesAreEqual = ((r.s.rangeFL - r.s.rangeFL) <= 5);
  sideLInfinite = (r.s.rangeFL == 100);
  sideRInfinite = (r.s.rangeFR == 100 );
  sidesAreInfinite = (sideLInfinite == true && sideRInfinite == true);
  sideLisCloser = (r.s.rangeFL < r.s.rangeFR);
  sideLRisk = (rangeFLSide(r.s.rangeFL) < 10);
  sideRRisk = (rangeFRSide(r.s.rangeFR) < 10);

  // Calculate angle
  int angleDifL = angleL + 45;
  int angleDifR = angleR - 45;
  angleWall.l = (horizontal.encodersL != 100 && r.s.encodersL != 100) ? wallAngle(horizontal.encodersL, r.s.encodersL, angleDifL) : -1;
  angleWall.r = (horizontal.encodersR != 100 && r.s.encodersR != 100) ? wallAngle(horizontal.encodersR, r.s.encodersR, angleDifR) : -1;

  // Return distance and set wall if needed
  setWall();
  return (dist){ rangeFLSide(r.s.rangeFL), rangeFRSide(r.s.rangeFR)};
}

int setWall() {
  if (r.s.wall == 0) r.s.wall = (r.s.rangeSL <= r.s.rangeSR) ? LEFT : RIGHT;
  if (r.s.wall == 0) r.s.wall = (rangeFLSide(r.s.rangeFL) <= rangeFRSide(r.s.rangeFR)) ? LEFT : RIGHT;
}

void considerRotation(int direction) {
  if (isRotatingL == true && direction == LEFT) properRotation++;
  else if (isRotatingR == true && direction == RIGHT) properRotation++;
  else properRotation = 0;
  
  if (direction == LEFT) {
    isRotatingL = true; isRotatingR = false;
    isTurningR = false;
  } else if (direction == RIGHT) {
    isRotatingL = false; isRotatingR = true;
    isTurningL = false;
  } else {
    isRotatingL = false; isRotatingR = false;
    isTurningL = false;
    isTurningR = false;
  }
  
  if (!isTurningL && direction == LEFT && properRotation == 5) isTurningL = true;
  else if (!isTurningR && direction == RIGHT && properRotation == 5) isTurningR = true;
}

volts setVoltage(volts speed, dist scale) {
  volts nextV = (volts){.l = (30 + speed.l) * scale.l, .r = (30 + speed.r) * scale.r };
  if (nextV.l < nextV.r) { considerRotation(LEFT); }
  else if (nextV.l > nextV.r) { considerRotation(RIGHT); }
  else { considerRotation(0); }
  return nextV;
}

int main () {
  initSocket();
  encodersReset();
  sensors initial;
  double toFront;
  dist toBack = {0,0};
  dist toSide = {0,0};
  volts speed = {30,30};
  dist scale = {1,1};

  while(1) {
    // This is a new cycle, first check and then move
    cTrail();
    memcpy(&initial, &r.s, sizeof(sensors));

    if (req.setWallAuto) {
      if (r.s.wall == LEFT) req.considerSide = LEFT;
      else if (r.s.wall == RIGHT) req.considerSide = RIGHT;
      else req.considerSide = 0;
    }

    // Check sensors
    if (req.checkFront == true) { toFront = checkFront(ACCURATE); printf("toFront %lF ", toFront); req.checkFront = false; }
    if (req.checkBack == true) { toBack = checkBack(ACCURATE); printf("toBack %lF %lF ", toBack.l, toBack.r); req.checkBack = false; }
    if (req.checkSide == true) { toSide = checkSide(ACCURATE, 0, 0); printf("toSide %lF %lF ", toSide.l, toSide.r); req.checkSide == false; }
    if (req.checkEncoders == true) { encodersGet(&r.s);  printf("encoders %i %i\n", r.s.encodersL, r.s.encodersR);}
    // if (req.saveLogs == true) addLog(&r.s, &r.l);

    
    // Calculate speed
    // if (req.calculateSpeed) 
    int dif = 40 - toSide.l;
    int val = cbrt(dif);

    // might be used for scale in tunnels to moderate velocity
    
    // const int backOk=20, backRange=5, backMinimum=8;
    if (req.calculateBack == true) {

      backConsidered = toBack.l;
      backDif = 30 - backConsidered; printf("backDif %i\n", backDif);
      
      // Wall is too close
      backVal = (dist) {backDif,backDif};
      
      // Wall is in range
      if (15 <= backConsidered && backConsidered <= 30) { backVal = (dist){0,0}; printf("back in Range\n"); }

      // Wall is far
      if (backConsidered <= 37) {

        if (r.s.wall == 0) {
          // Front is clear go straight if wall is not set
          backVal = (dist){0,0}; printf("frontsAreInfinite therefore can go Straight\n");
        }

        // Side exists, go straight
        // if (!sideLInfinite && !sideLRisk) {backVal = (dist){0,0}; printf("sidesAreInfinite therefore can go Straight\n");}
      }
      
      // Wall is infinite
      if (backConsidered == 38)
    
      printf("backVal %lF %lF\n", backVal.l, backVal.r);// TODO check the other side tooooooo!
      req.calculateBack = false;
    }
    
    
    // might be used for scale in tunnels to moderate velocity
    if (req.calculateSide == true) {

      sideConsidered = toSide.l;
      sideDif = sideOk - sideConsidered; printf("sideDif %i\n", sideDif);
      sideVal = sideDif;
      if (-sideRange <= sideDif && sideDif <= sideRange) { sideVal = 0; /**/ printf("side in Range\n"); /**/ }
      if (sideVal <= -sideMinimum) { }
    
      printf("sideVal %i\n", sideVal);// TODO check the other side tooooooo!
      req.calculateSide = false;
    }
    
    // might be used for scale in tunnels to moderate velocity
    if (req.calculateFront == true) {

      frontConsidered = toFront;
      frontDif = frontOk - frontConsidered; printf("frontDif %i\n", frontDif);
      frontVal = frontDif;
      if (-frontRange <= frontDif && frontDif <= frontRange) { frontVal = 0; printf("front in Range\n"); }
      if (frontVal <= -frontMinimum) { }
    
      printf("frontVal %i\n", frontVal);// TODO check the other side tooooooo!
      req.calculateFront = false;
    }


    
    // Calculate scale
    
    // Check for risk
      // - recalculate speed

    speed = (volts){backVal.l,-backVal.r};

    // Assign voltage
    r.v = setVoltage(speed, scale);


    // Finally arrived here
    moveAtVoltage(r.v.l, r.v.r);
    printf("*  Speed chosen: %i %i\n", r.v.l, r.v.r );
    req = DEFAULT_REQUEST;
  }

  return 0;
}
