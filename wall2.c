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

#include "rangefinders.h"
#include "encoders.h"
#include "bumpers.h"
#include "us.h"

#define DEBUG 1
#define RANGEUS 200-8
#define RANGESL 40-5
#define RANGESR 40-5
#define RANGEFL 100-5
#define RANGEFR 100-5

#define MINUS 200-170
#define MINSL 40-30
#define MINSR 40-30
#define MINFL 100-70
#define MINFR 100-70

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
  }
}

// 0 if staight, 1 if reposition, 2 if spinning, 3 if stopping
int shouldReposition(robot* r) {
  sensors* s = &r->s;
  volts* v = &r->v;
  status decision = DEFAULT_STATUS;

  printf("PARSE sl%i sr%i fl%i fr%i us%i\n", s->rangeSL, s->rangeSR, s->rangeFL, s->rangeFR, s->us);

  if ((s->us <= 100) && (s->us > MINUS)) {
    if ((s->rangeFL < 100) && (s->rangeFL > MINFL) && (s->rangeFR >=30)) {
      fflush(stdout);
      printf("close US, reposition FL (case 1) \n");
      reposition(r, 40, 15, 40, 15);
      return 1;
    } else if ((s->rangeFR < 100) && (s->rangeFR > MINFR) && (s->rangeFL >= 30)) {
      fflush(stdout);
      printf("close US, reposition FR (case 1) \n");
      reposition(r, 15, 40, 15, 40);
      return 1;
    } else if ((s->rangeFL < 100) && (s->rangeFL > MINFL) && (s->rangeFR == 100)) {
      fflush(stdout);
      printf("close US, reposition FR (case 2)\n");
      moveAtVoltage(30,15);
      return 1;
    } else if ((s->rangeFR < 100) && (s->rangeFR > MINFR) && (s->rangeFL == 100)) {
      fflush(stdout);
      printf("close US, reposition FR (case 2)\n");
      moveAtVoltage(15,30);
      return 1;
    }

    return 0;
  }

  //TODO to make it turn in the right direction, work out log of last Side sensor.
  //consider cases where all sensors are max, what happened before?
  
  if(((s->rangeFL <= MINFL) && (s->rangeFR <= MINFR) && (s-> us <= MINUS)) || (s-> us <= MINUS)) /*(s-> rangeSL <= 10)*/    {
    fflush(stdout);
    printf("Rotate Clockwise");
    moveAtVoltage(30,-30);
    return 2;
  }

  if(((((sqrt(2))/2)*(s->rangeFL)) <= (s->rangeSL)) && (((sqrt(2))/2)*(s-> rangeFL) <= ((sqrt(2))/2)*MINFL)){
    fflush(stdout);
    printf("level out by clockwise rotation");
    moveAtVoltage(20,-20);
    return 2;
  }

  if(((((sqrt(2))/2)*(s->rangeFR)) <= (s->rangeSR)) && (((sqrt(2))/2)*(s-> rangeFR) <= ((sqrt(2))/2)*MINFR)){
    fflush(stdout);
    printf("level out by anti-clockwise rotation");
    moveAtVoltage(-20,20);
    return 2;
  }
  
  // If rangeFL/FR in range or rangeSL/SR in range
 if (((s->rangeFL <= RANGEFL) && (s->rangeFL >= MINFL) && (s->rangeSL== 40)) || ((s->rangeFR <= RANGEFR) && (s->rangeFR >= MINFR)) && (s->rangeSR == 40)) {decision.straight = true;}
 /* if (((s->rangeSL <= RANGESL) && (s->rangeSL >= MINSL)) || ((s->rangeSR <= RANGESR) && (s->rangeSR >= MINSR))) {decision.straight = true;}*/
  if (decision.straight == true) {
    fflush(stdout);
    printf("we can go straight\n");
    return 0;
  }


  // TODO track previous position of SL SR and turn when it is not touching


    // LeftS pivot - Right is empty && SL is touching
    if (s->rangeFR == 100 && s->rangeSR == 40 && s->rangeSL < 40 && s->rangeFL>30 ) {
      fflush(stdout);
      printf("SL pivot\n");
      moveAtVoltage(25,30);
      return 1;
    }


    //Right pivot - Left is empty && SR is touching
        if (s->rangeFL == 100 && s->rangeSL == 40 && s->rangeSR < 40 && s->rangeFR>30 ) {
         fflush(stdout);
         printf("SR pivot\n");
         moveAtVoltage(30,25);
          return 1;
          }

  return 3;
}

int main () {
  initSocket();
  robot r = {.s= DEFAULT_SENSORS, .v= (volts){r: 100, l: 100}};

  while (1) {
    
    usGet(&r.s);
    rangeFGet(&r.s);
    rangeSGet(&r.s);

    if (!shouldReposition(&r)) {
      move(&r.v);
    }
  }

    encodersReset();

  return 0;
}




// TODO: clean the code
// TODO: add derivative & average 
