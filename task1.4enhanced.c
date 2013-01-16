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
  
  sensors toBeStraight1 = {.encodersL = x*500, .encodersR = x*500 };
  sensors toBeStraight2 = {.encodersL = 100, .encodersR = 100 };
  sensors toBeSpinning1 = {.encodersL = 100, .encodersR = 100 };
  sensors toBeSpinning2 = {.encodersL = 96, .encodersR = 96 };

  
  considerSlide(10, 10, 60, 60, &toBeStraight1);
  considerSlide(60, 60, 10, 10, &toBeStraight2);
  while(1) {    
    changeVelocity(60, 60, 10, 10, &toBeStraight1, &toBeStraight2);
    changeVelocity(10, -10, 10, -10, &toBeSpinning1, &toBeSpinning2);
  }
}

int main () {
  initSocket();
  coefficientSquare(1);
}