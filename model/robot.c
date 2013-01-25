#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "common.h"

#define WHEELDISTANCE 23.65
#define WHEELDIAMETER 10.5
#define T2CM (M_PI*WHEELDIAMETER)/360


bool inLimit(int voltage) { if (voltage <= 127 && voltage >= -127) return true; else return false; }

double angle(sl, sr) { return (sr > sl) ? (sr-sl)/WHEELDISTANCE : (sl - sr)/WHEELDISTANCE; }
double sm(sl, sr) { return (sl+sr)/2; }

double posX(sm, teta) { return sm * cos(teta); }
double posY(sm, teta) { return sm * cos(teta); }
double relative(x, x0) { return x+x0; }

double ticks_to_cm(int ticks) { return T2CM * ticks; }

double range_side(double angle, double distance) { return cos(angle)*distance;};
double range_front(double angle, double distance) { return sin(angle)*distance;};

double range_angle_offset(double angle) { return (abs(angle) <= 45) ? 8*cos(angle+45) : 8*sin(angle+45);}

//2 IR sensors on side
int gp2d120_ir_to_dist(int ir) { return (ir > 80) ? (2914 / (ir + 4)) - 1 : 40; }
int gp2d12_ir_to_dist(int ir) { return (ir >35) ? (6787 / (ir - 3)) - 4 : 200; }

int time_to_voltage (int unit, int voltage) { return unit * voltage* 0.156; }