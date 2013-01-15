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

int main () {
  initSocket();

  sensors toBeTravelled = {.encodersL = 500, .encodersR = 200 };

  constAcceleration(127, 97, 60, 10, &toBeTravelled);

  stopMovement();
  return 0;
}