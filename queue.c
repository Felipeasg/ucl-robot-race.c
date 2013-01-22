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
  if (++(l->index) > 4 ) l->index = 0;
  memcpy(&l->sensors[l->index], s, sizeof(sensors));
  printf("index %i eg: %i \n", l->index, l->sensors[l->index].us);
}