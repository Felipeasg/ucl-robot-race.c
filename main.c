#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <regex.h>
#include "functions.h"

int main () {
  initSocket();
  
  sensors initial;
  sensors current;
  sensors toBe = {encodersL: 200, encodersR: 200};

  encodersGet(&initial);
  encodersGet(&current);
  while (sensorsToBe(&current, &initial, &toBe, SMELR)) {
    moveStraightAtVoltage(90);
    encodersGet(&current);
  }
}