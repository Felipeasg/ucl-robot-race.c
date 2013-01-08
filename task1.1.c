#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "functions.h"

int main () {
  int i;
  
  initSocket();
  for (i = 0; i<=30; i++) {
    moveStraightAtVoltage(100);
  }
  stopMovement();
}