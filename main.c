#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include "common.h"
#include "model/robot.h"

int main () {
  initSocket();
  encodersReset();
  int counts = 120;
  printf("Expected: %lF and slides %lF,%lF\n", counts * 30* 0.156, slideE(0,30), slideE(30,0));

  while (counts >= 0) {
    moveAtVoltage(30, 30);
    counts--;
  }
  
  stopMovement();
  encodersGet(&r.s);
  
  printf("Actual: %i\n", r.s.encodersL);

  return 0;
}
