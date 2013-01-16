#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>

#define DEBUG 1
#include "common.h"

// global vars holdin velocity

int main () {
  initSocket();

  sensors toBeTravelled1 = {.encodersL = 300, .encodersR = 300 };
  sensors toBeTravelled2 = {.encodersL = 1000, .encodersR = 1000 };
  encodersReset();
  // 
  // considerSlide(0, 0, 100, 100, &toBeTravelled1);
  // considerSlide(100, 100, 50, 50, &toBeTravelled2);
  // changeVelocity(100, 100, 50, 50, &toBeTravelled1, &toBeTravelled2);
  // stopMovement();

  considerSlide(0, 0, 10, 10, &toBeTravelled1);
  constAcceleration (10, 10, 100, 100, &toBeTravelled1, 10);

  return 0;
}