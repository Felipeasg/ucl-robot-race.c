#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "../common.h"


void bumpersCmd() {
  sprintf(buf, "S BFLR\n");
  #ifdef DEBUG
  printf("S BFLR\n");
  #endif
  nextCmd();
}

void bumpersGet(sensors* Sensors) {
  char* elaborated[80];
  bumpersCmd();
  parseCmd(buf, elaborated, SBFLR, Sensors);
}

void bumpersSet(sensors* Sensors, int l, int r) {
  Sensors->bumpersL = l;
  Sensors->bumpersR = r;
  //TODO something better than atoi
}

int bumpersParse(char* elaborated[], sensors* Sensors) {
  if (elaborated[2] == NULL || elaborated[3] == NULL ||
      !strcmp(elaborated[2],"") || !strcmp(elaborated[3],"") ) return 1;

  bumpersSet(Sensors, atoi(elaborated[2]), atoi(elaborated[3]));
  // it should be 0 for failure, 1 for silent, 2 for OK
  return 2;
}

/* encodersToBe
// return true when is not yet toBe, return false when it goes over toBe
*/

bool bumpersToBe(sensors* current, sensors* toBe) {
  int l,r;

  if (current->bumpersL != toBe->bumpersL) {l=0;}
  else {l=1;}

  if (current->bumpersR != toBe->bumpersR) {r=0;}
  else {r=1;}

  if (l == 1 && r == 1) {
    return false;
  }
  return true; 
}