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

int main () {
  initSocket();
  encodersReset();
  int count, speed;
  sensors initial = DEFAULT_SENSORS;
  

    // counts = 120;
    // printf("Expected: %lF and slides %lF,%i, %lf, %lf\n", count * 30* 0.156, slideE(0,30), 30, T2CM, 30/T2CM);

  while(1) {
    memcpy(&initial, &r.s, sizeof(sensors));
    encodersGet(&r.s);
    usGet(&r.s);
    
    // rangeFGet(&r.s); addLog(&r.s, &r.l);
    // moveILR(45, 45);
    // rangeFGet(&r.s); addLog(&r.s, &r.l);
    // moveILR(0, 0);
    // rangeFGet(&r.s); addLog(&r.s, &r.l);
    // moveILR(-45, -45);
    // rangeSGet(&r.s); addLog(&r.s, &r.l);
    int i;
    for (i=0; i < 21; i++) {
      r.s.rangeFL = i;
      addLog(&r.s, &r.l);
    }
    
    printLogs(&r.l);

    count = 0;
    while (sensorToBe(r.s.encodersL, initial.encodersL, r.s.us/T2CM)) {
      moveAtVoltage(30, 30);

      rangeFGet(&r.s);
      moveILR(45, 45);
      rangeFGet(&r.s);
      moveILR(0, 0);
      rangeFGet(&r.s);
      moveILR(-45, -45);
      rangeSGet(&r.s);

      encodersGet(&r.s);
      printf("Actual: %i and %i %lF\n", r.s.encodersL-initial.encodersL, count, (double)500/(r.s.encodersL-initial.encodersL));
      count++;
    }
  }
    

  
  printf("Actual: %i\n", r.s.encodersL);

  return 0;
}
