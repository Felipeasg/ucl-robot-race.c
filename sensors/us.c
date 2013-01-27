#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
<<<<<<< HEAD
#include "common.h"
=======
#include "../common.h"
>>>>>>> 3bc8e3c27e3b0568d0311c901159394c11bafc79


void usCmd() {
  snprintf(buf, 80, "S US\n");
  #ifdef DEBUG
  printf("S US\n");
  #endif
  nextCmd();
}

void usGet(sensors* Sensors) {
  char* elaborated[80];
  usCmd();
  parseCmd(buf, elaborated, SUS, Sensors);
}

void usSet(sensors* Sensors, int us) {
  Sensors->us = us;
  //TODO something better than atoi
}

int usParse(char* elaborated[], sensors* Sensors) {
  if (elaborated[2] == NULL ||
      !strcmp(elaborated[2],"") ) return 1;

<<<<<<< HEAD
  usSet(Sensors, atoi(elaborated[2])-8);
=======
  usSet(Sensors, atoi(elaborated[2]));
>>>>>>> 3bc8e3c27e3b0568d0311c901159394c11bafc79
  // it should be 0 for failure, 1 for silent, 2 for OK
  return 2;
}

/* encodersToBe
// return true when is not yet toBe, return false when it goes over toBe
*/

bool usToBe(sensors* current, sensors* initial, sensors* toBe) { //TODO refactor
  int us;

  if (sensorToBe(current->encodersL, initial->encodersL, toBe->encodersL)) {us=0;}
  else {us=1;}

  if (us == 1) {
    return false;
  }
  return true; 
}