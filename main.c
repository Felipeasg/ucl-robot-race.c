#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "functions.h"

int main () {
  initSocket();
  
  turnOnSpotAtVoltage(90);
}