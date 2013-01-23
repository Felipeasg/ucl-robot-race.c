#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "queue.h"
#include "common.h"

void sensorsDifference(sensors* last, sensors* prev, sensors* new) {
  new->encodersL = last->encodersL - prev->encodersL;
  new->encodersR = last->encodersR - prev->encodersR;
  new->rangeSL = last->rangeSL - prev->rangeSL;
  new->rangeSR = last->rangeSR - prev->rangeSR;
  new->rangeFL = last->rangeFL - prev->rangeFL;
  new->rangeFR = last->rangeFR - prev->rangeFR;
  new->us = last->us - prev->us;
}

void sensorsGetOneStep(sensors* Sensors, sensors* New, int steps) {

  // TODO use sensorsSet instead
  if (Sensors->encodersL != 0) New->encodersL = Sensors->encodersL/steps;
  if (Sensors->encodersR != 0) New->encodersR = Sensors->encodersR/steps;
  if (Sensors->rangeSL != 0) New->rangeSL = Sensors->rangeSL/steps;
  if (Sensors->rangeSR != 0) New->rangeSR = Sensors->rangeSR/steps;
  if (Sensors->rangeFL != 0) New->rangeFL = Sensors->rangeFL/steps;
  if (Sensors->rangeFR != 0) New->rangeFR = Sensors->rangeFR/steps;
  if (Sensors->us != 0) New->us = Sensors->us/steps;
}

bool sensorToBe(int current, int initial, int toBe ) {

  if (abs(abs(current) - abs(initial)) >= abs(toBe)) // TODO
    return false;

  return true; 
}

bool sensorsToBe(sensors* Sensors, sensors* initial, sensors* toBe) {
  // TODO SENSOR CHECKING
  bool encoders = false, bumpers = false, rangeF = false, rangeS = false, us = false;

  if (toBe->encodersL != 0 || toBe->encodersR != 0) { encoders = encodersToBe(Sensors, initial, toBe); }
  if (toBe->rangeFL != 0 || toBe->rangeFR != 0) { rangeF = rangeFToBe(Sensors, initial, toBe); }
  if (toBe->rangeSL != 0 || toBe->rangeSR != 0) { rangeS = rangeSToBe(Sensors, initial, toBe); }
  if (toBe->us != 0) { us = usToBe(Sensors, initial, toBe); }
  bumpers = bumpersToBe(Sensors, toBe); //TODO improve this

  if (!encoders && !rangeF && !rangeS && !bumpers && !us) return false;
  else return true;
}

void parseCmd (char* buf, char* elaborated[], int funcNumber, sensors* Sensors) {
  // implement expectations
  int i = 0;
  int sensorInvolved = 0;

  elaborated[i] = strtok (buf," ");
  
  if (!strcmp(elaborated[i], ".\n")) return;
  while (elaborated[i++] != NULL) {
    elaborated[i] = strtok (NULL, " ");
  }
//  if (!funcNumber) return; // if 0 no function if 9 this function...
  
  if (!strcmp(elaborated[0], "W")) {
    memset(elaborated, 0, 80);
    memset(buf, 0, 80);
    read(sock, buf, 80);
    return parseCmd (buf, elaborated, funcNumber, Sensors);
  }

  if (funcNumber == SMELR || (!strcmp(elaborated[0], "S") && !strcmp(elaborated[1], "MELR"))) {
    encodersParse(elaborated, Sensors);
    sensorInvolved++;
  }
  if (funcNumber == SBFLR || (!strcmp(elaborated[0], "S") && !strcmp(elaborated[1], "SBFLR"))) {
    bumpersParse(elaborated, Sensors);
    sensorInvolved++;
  }
  if (funcNumber == SIFLR || (!strcmp(elaborated[0], "S") && !strcmp(elaborated[1], "SIFLR"))) {
    rangeFParse(elaborated, Sensors);
    sensorInvolved++;
  }
  if (funcNumber == SISLR || (!strcmp(elaborated[0], "S") && !strcmp(elaborated[1], "SISLR"))) {
    rangeSParse(elaborated, Sensors);
    sensorInvolved++;
  }
  if (funcNumber == SUS || (!strcmp(elaborated[0], "S") && !strcmp(elaborated[1], "US"))) {
    usParse(elaborated, Sensors);
    sensorInvolved++;
  }

  if (sensorInvolved == 0)
    printf("PARSE %s %s %s %s\n", elaborated[0], elaborated[1], elaborated[2], elaborated[3]);
}