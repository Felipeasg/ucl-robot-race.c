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

double slideE(int m, int n) {
  m = m/10; printf("%i\n",m);
  n = n/10; printf("%i\n",n);
  double series1 = ((double)m/2)*(10+(double)m*10)/20; printf("%lF\n",series1);
  double series2 = ((double)n/2)*(10+(double)n*10)/20; printf("%lF\n",(series2 > series1) ? series2-series1 : series1-series2);
  return (series2 > series1) ? series2-series1 : series1-series2;
}

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
