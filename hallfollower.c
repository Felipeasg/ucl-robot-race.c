#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include <math.h>
#include "common.h"

#include "rangefinders.h"
#include "encoders.h"
#include "bumpers.h"
#include "us.h"

#include "robot.h"
#include "check.h"

int main () {
  
  initSocket();
  sleep(3);
  encodersReset();

  sensors *history = NULL;
  
//  dead_end(&history, 20);
//  playback(&history, 20);

  while(1) {
    sensors initial = r.s;
    encodersGet(&r.s);
    position(&r.s, &initial);
  }
  
  return 0;
}
