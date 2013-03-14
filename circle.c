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
  
  // Setting up the robot
  initSocket();
  sleep(3);
  encodersReset();
  moveILR(-16,16);

  // Creating a history
  sensors *history = NULL;
  sensors *first_value = record(&history, &r.s);

  while (absDouble(r.s.angle) < 2*M_PI*1.05) {
    fflush(stdout);
    printf("angle: %lF\n", r.s.angle);
    passage_drive(&history, 20);
  }
  replay(&first_value, 20);
  
  stopMovement();
  
  return 0;
}
