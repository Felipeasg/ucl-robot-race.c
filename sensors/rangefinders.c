#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>
#include "../common.h"


void rangeFCmd() {
  sprintf(buf, "S IFLR\n");
  #ifdef DEBUG
  printf("S IFLR\n");
  #endif
  nextCmd();
}

void rangeFGet(sensors* Sensors) {
  char* elaborated[80];
  rangeFCmd();
  parseCmd(buf, elaborated, SIFLR, Sensors);
}

void rangeFSet(sensors* Sensors, int l, int r) {
  Sensors->rangeFL = l;
  Sensors->rangeFR = r;
}

int rangeFOffsetAtAngle(int angle) {
  if (angle == 45) return 8;
}

int rangeFLSideDistance(int distance) {
  return cos(r.rangeAngles.l)*distance;
};
int rangeFRSideDistance(int distance) {
  return cos(r.rangeAngles.r)*distance;
};
int rangeFLFrontDistance(int distance){
  return sin(r.rangeAngles.l)*distance;
};
int rangeFRFrontDistance(int distance){
  return sin(r.rangeAngles.r)*distance;
};

int rangeFParse(char* elaborated[], sensors* Sensors) {
  if (elaborated[2] == NULL || elaborated[3] == NULL ||
      !strcmp(elaborated[2],"") || !strcmp(elaborated[3],"") ) return 1;

  rangeFSet(Sensors, gp2d12_ir_to_dist(atoi(elaborated[2]))-rangeFOffsetAtAngle(r.rangeAngles.l), gp2d12_ir_to_dist(atoi(elaborated[3]))-rangeFOffsetAtAngle(r.rangeAngles.r) );
  // it should be 0 for failure, 1 for silent, 2 for OK
  return 2;
}

/* encodersToBe
// return true when is not yet toBe, return false when it goes over toBe
*/

bool rangeFToBe(sensors* current, sensors* initial, sensors* toBe) {
  int l,r;

  if (sensorToBe(current->rangeFL, initial->rangeFL, toBe->rangeFL)) {l=0;}
  else {l=1;}
  if (sensorToBe(current->rangeFR, initial->rangeFR, toBe->rangeFR)) {r=0;}
  else {r=1;}

  if (l == 1 && r == 1) {
    return false;
  }
  return true; 
}


void rangeSCmd() {
    sprintf(buf, "S ISLR\n");
#ifdef DEBUG
    printf("S ISLR\n");
#endif
    nextCmd();
}

void rangeSGet(sensors* Sensors) {
    char* elaborated[80];
    rangeSCmd();
    parseCmd(buf, elaborated, SISLR, Sensors);
}

void rangeSSet(sensors* Sensors, int l, int r) {
    Sensors->rangeSL = l;
    Sensors->rangeSR = r;
}

int rangeSParse(char* elaborated[], sensors* Sensors) {
    if (elaborated[2] == NULL || elaborated[3] == NULL ||
        !strcmp(elaborated[2],"") || !strcmp(elaborated[3],"") ) return 1;
    
    rangeSSet(Sensors, gp2d120_ir_to_dist(atoi(elaborated[2]))-2, gp2d120_ir_to_dist(atoi(elaborated[3]))-2);
    // it should be 0 for failure, 1 for silent, 2 for OK
    return 2;
}

/* encodersToBe
 // return true when is not yet toBe, return false when it goes over toBe
 */

bool rangeSToBe(sensors* current, sensors* initial, sensors* toBe) {
    int l,r;
    
    if (sensorToBe(current->rangeSL, initial->rangeSL, toBe->rangeSL)) {l=0;}
    else {l=1;}
    if (sensorToBe(current->rangeSR, initial->rangeSR, toBe->rangeSR)) {r=0;}
    else {r=1;}
    
    if (l == 1 && r == 1) {
        return false;
    }
    return true; 
}

int gp2d120_ir_to_dist(int ir) {		//2 IR sensors on side
    int dist;

    if (ir >80)
       dist = (2914 / (ir + 4)) - 1;
    else dist = 40;
      return dist;
}

int gp2d12_ir_to_dist(int ir) {			//2 IR sensors on front
  int dist;

  if (ir >35)
     dist = (6787 / (ir - 3)) - 4;
  else dist = 200;
    return dist;
}