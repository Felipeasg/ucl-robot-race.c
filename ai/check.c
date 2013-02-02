#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "common.h"
#include "rangefinders.h"
#include "us.h"
#include "robot.h"
#include "check.h"


double checkFrontStatic(bool accurate) {

  usGet(&r.s);
  frontRisk = (r.s.us < 20) ;
  return r.s.us;
}

dist checkSideStatic(bool accurate) {
  moveILR(-30, 0);
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
  sideLRisk = (r.s.rangeFL < 15);
  sideRRisk = (r.s.rangeFR < 15);
  sideLFar = (r.s.rangeFL > 30);
  sideRFar = (r.s.rangeFR < 40);

  // Calculate angle
  setWall();
  printf( "checkSide %lF %lF %i %i\n", rangeFLSide(r.s.rangeFL), rangeFRSide(r.s.rangeFR), r.s.rangeFL, r.s.rangeFR );
  return (dist){ r.s.rangeFL, r.s.rangeFR};
}

dist checkBack(bool accurate) {
  rangeSGet(&r.s);
  backAreEqual = ((r.s.rangeFL - r.s.rangeFL) <= 5);
  backLisInfinite = (r.s.rangeSL == 38);
  backRisInfinite = (r.s.rangeSR == 38);
  backsAreInfinite = (backLisInfinite && backRisInfinite);
  backLisCloser = (r.s.rangeSL < r.s.rangeSR);
  backLRisk = (r.s.rangeSL < 8);
  backRRisk = (r.s.rangeSR < 8);
  setWall();
  return (dist){r.s.rangeSL,r.s.rangeSR};
}

double checkFront(bool accurate) {

  moveILR(60,-60);
  rangeFGet(&r.s);
  frontsAreEqual = ((r.s.rangeFL - r.s.rangeFL) <= 5);
  frontLInfinite = (r.s.rangeFL == 100);
  frontRInfinite = (r.s.rangeFR == 100 );
  frontsAreInfinite = (frontLInfinite == true && frontRInfinite == true);
  frontLisCloser = (r.s.rangeFL < r.s.rangeFR);
  frontLRisk = (rangeFLFront(r.s.rangeFL) < 15);
  frontRRisk = (rangeFRFront(r.s.rangeFR) < 15);
  
  if (frontsAreEqual && frontsAreInfinite && !accurate) return 100;
  else if (frontsAreEqual && !frontsAreInfinite) return rangeFRFront(r.s.rangeFR);
  else if (!frontsAreEqual && !accurate) return min(rangeFLFront(r.s.rangeFL),rangeFRFront(r.s.rangeFR));
  
  usGet(&r.s);
  return min(r.s.us, min(rangeFLFront(r.s.rangeFL),rangeFRFront(r.s.rangeFR)));
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
  /*/printf("\n UUUUUUU %i %i\n\n", horizontal.rangeFL, horizontal.rangeFR);/**/
  
  // Tracking wanted angle
  moveILR(angleL, angleR);
  rangeFGet(&r.s); // addLog(&r.s, &r.l);
  printf("\n checkSide %d \n", r.s.rangeFL);
  
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
  sideLRisk = (rangeFLSide(r.s.rangeFL) < 15);
  sideRRisk = (rangeFRSide(r.s.rangeFR) < 15);

  // Calculate angle
  int angleDifL = angleL + 45;
  int angleDifR = angleR - 45;
  angleWall.l = (horizontal.rangeFL != 100 && r.s.rangeFL != 100) ? wallAngle(horizontal.rangeFL, r.s.rangeFL, angleDifL) : -1;
  angleWall.r = (horizontal.rangeFR != 100 && r.s.rangeFR != 100) ? wallAngle(horizontal.rangeFR, r.s.rangeFR, angleDifR) : -1;

  // Return distance and set wall if needed
  setWall();
  printf( "checkSide %lF %lF %i %i\n", rangeFLSide(r.s.rangeFL), rangeFRSide(r.s.rangeFR), r.s.rangeFL, r.s.rangeFR );
  return (dist){ rangeFLSide(r.s.rangeFL), rangeFRSide(r.s.rangeFR)};
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
