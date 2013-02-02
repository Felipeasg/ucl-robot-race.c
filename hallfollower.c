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

#include "robot.h"
#include "check.h"

double ratios() {
  
  // int differenceBack = r.s.rangeSR - r.s.rangeSL; int differenceSide = r.s.rangeFR - r.s.rangeFL;
  // double ratioBack = (double) r.s.rangeSR / r.s.rangeSL;
  double ratioSide = (double) r.s.rangeFR / r.s.rangeFL;

  // dist expectDifferenceBack = { 0 - r.s.rangeSL, 0 - r.s.rangeSR}; dist expectDifferenceSide = {27 - r.s.rangeFL,27 - r.s.rangeFR};
  // dist expectRatioBack = {27.0 / r.s.rangeSL,27.0 / r.s.rangeSR}; dist expectRatioSide = {27.0 / r.s.rangeFL,27.0 / r.s.rangeFR};

  // printf("PARSE sl%i sr%i fl%i fr%i us%i\n", r.s.rangeSL, r.s.rangeSR, r.s.rangeFL, r.s.rangeFR, r.s.us);
  return ratioSide;
  
}

void record (sensors **history, sensors *ptr) {

  sensors *recording = (sensors*)malloc(sizeof(sensors));

  *recording = *ptr;
  recording->next = *history;

  *history = recording;

}

void passage_drive (sensors **history, int speed) {
  
  double ratio = ratios();

  volts voltage;
  voltage.l = ratio < 1.0 ? (double)speed*ratio : speed;
  voltage.r = ratio > 1.0 ? (double)speed/ratio : speed;

  move(&voltage);

  encodersGet(&r.s);
  record(history, &r.s);
  
}

void playback (sensors **history, int speed) {

  sensors initial;
  dist todo;
  volts voltage;

  encodersGet(&initial);

  reposition(&r, 400, 400, 20, -20);

  r.s = DEFAULT_SENSORS;
  encodersReset();

  while (*history)
  {
      do {
          todo.l = initial.encodersL - r.s.encodersR > (*history)->encodersL;
          todo.r = initial.encodersR - r.s.encodersL > (*history)->encodersR;
          
          voltage = (volts){ speed * (todo.r ? 1 : 0), speed * (todo.l ? 1 : 0) };

          move(&voltage);

          encodersGet(&r.s);
          
      } while (todo.r || todo.l);
      
      *history = (*history)->next;
  }
}

void dead_end(sensors **history, int speed) { usGet(&r.s); rangeFGet(&r.s); rangeSGet(&r.s); addLog(&r.s, &r.l);

  moveILR(-16,16);
  
  while (r.s.us > 15) { usGet(&r.s); rangeFGet(&r.s); rangeSGet(&r.s);

    passage_drive(history, speed);
    
    addLog(&r.s, &r.l);
  }

}


int main () {
  
  initSocket();
  
  encodersReset();

  sensors *history = NULL;
  
  dead_end(&history, 20);
  playback(&history, 20);
  
  return 0;
}
