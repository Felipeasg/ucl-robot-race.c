#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "common.h"

void addLog(sensors* s, logs* l) {
  if (++(l->index) > 4 ) {
    if (l->empty == true) l->empty = false;
    l->index = 0;
  }
  memcpy(&l->sensors[l->index], s, sizeof(sensors));
  printf("index %i count %d eg: %i \n", l->index, l->empty,  l->sensors[l->index].us);
}