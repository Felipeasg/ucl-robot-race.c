#ifndef RANGEFINDERS_INCLUDED
#define RANGEFINDERS_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>
#include "common.h"

bool moveIR (int angle);
int moveIL (int angle);
int moveILR (int angle1, int angle2);
void rangeFCmd();
void rangeFGet(sensors* Sensors);
void rangeFSet(sensors* Sensors, int l, int r);
double rangeFLSide(double distance);
double rangeFRSide(double distance);
double rangeFLFront(double distance);
double rangeFRFront(double distance);
int rangeFParse(char* elaborated[], sensors* Sensors);
bool rangeFToBe(sensors* current, sensors* initial, sensors* toBe);


void rangeSCmd();
void rangeSGet(sensors* Sensors);
void rangeSSet(sensors* Sensors, int l, int r);
int rangeSParse(char* elaborated[], sensors* Sensors);
bool rangeSToBe(sensors* current, sensors* initial, sensors* toBe);

#endif