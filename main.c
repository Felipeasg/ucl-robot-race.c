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

int backConsidered, backDif; dist backVal= {999,999};
const int backOk=20, backRange=5, backMinimum=15;

<<<<<<< HEAD
int sideConsidered, sideDif; dist sideVal = {999,999};
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

void setWall() {
  if (r.s.rangeSL != r.s.rangeSR) {
    if (r.s.wall == 0) {r.s.wall = (r.s.rangeSL < r.s.rangeSR) ? LEFT : RIGHT; printf("\n\n\tWall set. \n\n");}
=======
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


// TODO
// - noise detection
// - derivative / change in sensors
// - proportion
// - rewrite reposition and shouldReposition

void reposition(robot* r, int encodersL, int encodersR, int voltageL, int voltageR) {
  sensors toBe = DEFAULT_SENSORS;
  sensors initial = DEFAULT_SENSORS;
  encodersSet(&initial, r->s.encodersL, r->s.encodersR);
  encodersSet(&toBe, encodersL, encodersR);

  encodersGet(&r->s);
  while (sensorsToBe(&r->s, &initial, &toBe)) {
    moveAtVoltage(voltageL, voltageR);
    encodersGet(&r->s);
>>>>>>> 3bc8e3c27e3b0568d0311c901159394c11bafc79
  }
  // if (r.s.wall == 0) r.s.wall = (r.s.rangeFL <= r.s.rangeFR) ? LEFT : RIGHT;
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

dist checkBack(bool accurate) {
  rangeSGet(&r.s);
  backLisInfinite = (r.s.rangeSL == 38);
  backRisInfinite = (r.s.rangeSR == 38);
  backsAreInfinite = (backLisInfinite && backRisInfinite);
  backLisCloser = (r.s.rangeSL < r.s.rangeSR);
  backLRisk = (r.s.rangeSL < 10);
  backRRisk = (r.s.rangeSR < 10);
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
  /*/printf("\n UUUUUUU %i %i\n\n", horizontal.rangeFL, horizontal.rangeFR);/**/
  
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
  sideLRisk = (rangeFLSide(r.s.rangeFL) < 15);
  sideRRisk = (rangeFRSide(r.s.rangeFR) < 15);

  // Calculate angle
  int angleDifL = angleL + 45;
  int angleDifR = angleR - 45;
  angleWall.l = (horizontal.rangeFL != 100 && r.s.rangeFL != 100) ? wallAngle(horizontal.rangeFL, r.s.rangeFL, angleDifL) : -1;
  angleWall.r = (horizontal.rangeFR != 100 && r.s.rangeFR != 100) ? wallAngle(horizontal.rangeFR, r.s.rangeFR, angleDifR) : -1;

  // Return distance and set wall if needed
  setWall();
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

volts setVoltage(volts speed, dist scale) {
  volts nextV = (volts){.l = (30 + speed.l) * scale.l, .r = (30 + speed.r) * scale.r };
  if (nextV.l < nextV.r) { considerRotation(LEFT); }
  else if (nextV.l > nextV.r) { considerRotation(RIGHT); }
  else { considerRotation(0); }
  return nextV;
}

int main () {
  initSocket();
<<<<<<< HEAD
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
      
      // In case of no risk
      /**/printf("frontLRisk %d frontLRisk %d sideLRisk %d sideRRisk %d backLRisk %d backRRisk %d wall %d\n", frontLRisk, frontLRisk, sideLRisk, sideRRisk, backLRisk, backRRisk, r.s.wall);/**/
      if (!frontLRisk && !frontLRisk && !sideLRisk && !sideRRisk && !backLRisk && !backRRisk) {

        // Continues if no wall has been followed
        if (r.s.wall == 0) { backVal = (dist) {0,0}; /**/printf("No risks. No wall. Go straight\n"); }/**/
        
        // You are in a deep corner, you may want to turn proportionally to distance.
        else if (r.s.wall == LEFT && sideLInfinite && !backLisInfinite) { /**/printf("No risks. Left wall. Touching back. Turn\n");/**/
          backConsidered = toBack.l;
          backDif = 15 - backConsidered; /*/printf("backDif %i\n", backDif);/**/
          backVal = (dist) {backDif/1.5,backDif/1.5};
          // Never happened, this means it touches if very far
          if (backDif < -10) backVal = (dist){-15, -15};
          
        // You are right corner
        } else if (r.s.wall == RIGHT && sideLInfinite && !backLisInfinite) {/**/printf("No Risks. Right Wall is being followed %i.\n", r.s.wall);/**/ backVal = (dist){999, 999};}
        // You are
        else {backVal = (dist){999, 999};}
        
      } else { /**/printf("Risks, skip.\n");/**/ backVal = (dist){999, 999}; }
      printf("backVal %lF %lF\n", backVal.l, backVal.r);// TODO check the other side tooooooo!
      req.calculateBack = false;
    }
    
    
    // might be used for scale in tunnels to moderate velocity
    if (req.calculateSide == true) {
=======
  robot r = {.s= DEFAULT_SENSORS, .v= (volts){r: 20, l: 20}};
>>>>>>> 3bc8e3c27e3b0568d0311c901159394c11bafc79

      sideConsidered = toSide.l;
      sideDif = 40 - sideConsidered; /*/printf("sideDif %i\n", sideDif);/**/
      sideVal = (dist) {sideDif, sideDif};
      if (-5 <= sideDif && sideDif <= 5) { sideVal = (dist){0,0}; /*/ printf("side in Range\n"); /**/ }
      if (sideDif < -20) sideVal = (dist){-15,15};
    
<<<<<<< HEAD
      /**/printf("sideVal %lF %lF\n", sideVal.l, sideVal.r);/**/ // TODO check the other side tooooooo!
      req.calculateSide = false;
    }
    
    // might be used for scale in tunnels to moderate velocity
    if (req.calculateFront == true) {

      frontConsidered = toFront;
      frontDif = frontOk - frontConsidered; /*/printf("frontDif %i\n", frontDif);/**/
      frontVal = frontDif;
      if (-frontRange <= frontDif && frontDif <= frontRange) { frontVal = 0; /*/printf("front in Range\n");/**/ }
      if (frontVal <= -frontMinimum) { }
    
      /*/printf("frontVal %i\n", frontVal);/**/ // TODO check the other side tooooooo!
      req.calculateFront = false;
=======
    usGet(&r.s);
    rangeFGet(&r.s);
    rangeSGet(&r.s);
    logsAdd(&r.s);

    if (!shouldReposition(&r)) {
      move(&r.v);
>>>>>>> 3bc8e3c27e3b0568d0311c901159394c11bafc79
    }


    
    // Calculate scale
    
    // Check for risk
      // - recalculate speed

    if (frontVal != 999) speed = (volts){frontVal,-frontVal};
    if (sideVal.l != 999 && sideVal.r != 999) speed = (volts){sideVal.l,-sideVal.r};
    if (backVal.l != 999 && backVal.r != 999) speed = (volts){backVal.l,-backVal.r};

    // Assign voltage
    r.v = setVoltage(speed, scale);


    // Finally arrived here
    moveAtVoltage(r.v.l, r.v.r);
    printf("*  Speed chosen: %i %i\n\n", r.v.l, r.v.r );
    req = DEFAULT_REQUEST;
  }
  
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

  return 0;
}
