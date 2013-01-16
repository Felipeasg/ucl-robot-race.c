#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <regex.h>
#include "common.h"


void coefficientSquare(int x) {
  
  sensors toBeSpinning1 = {.encodersL = 1000, .encodersR = 1000 };
  sensors toBeSpinning2 = {.encodersL = 200, .encodersR = 200 };
  while(1) {    
    changeVelocity(-127, 127, -10, 10, &toBeSpinning1, &toBeSpinning2);
  }
  // or using turnOnSpotAtVoltage(100); in the while loop
}

int main () {
  initSocket();
  coefficientSquare(1);
}