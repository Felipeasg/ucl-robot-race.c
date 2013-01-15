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

  sensors toBeTravelled1 = {.encodersL = 200, .encodersR = 200 };
  sensors toBeTravelled2 = {.encodersL = 200, .encodersR = 200 };
  constAcceleration(60, -60, -60, 60, &toBeTravelled1, &toBeTravelled2);


  return 0;
}