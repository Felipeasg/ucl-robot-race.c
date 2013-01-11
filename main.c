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
  sensors toBe;

void coefficientSquare (int x) {
  while(1) {
    toBe.encodersL = 199;
    toBe.encodersR = 199;
    encodersGet(&initial);
    encodersGet(&current);
    while (sensorsToBe(&current, &initial, &toBe, SMELR)) {
      turnOnSpotAtVoltage(20);
      encodersGet(&current);
    }
  
    toBe.encodersL = x*500;
    toBe.encodersR = x*500;
    encodersGet(&initial);
    encodersGet(&current);
    while (sensorsToBe(&current, &initial, &toBe, SMELR)) {
      moveStraightAtVoltage(20);
      encodersGet(&current);
    }
  }
}
