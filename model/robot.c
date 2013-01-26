#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "common.h"
#include "robot.h"

double slideE(int m, int n) {
  m = m/10; printf("%i\n",m);
  n = n/10; printf("%i\n",n);
  double series1 = ((double)m/2)*(10+(double)m*10)/20; printf("%lF\n",series1);
  double series2 = ((double)n/2)*(10+(double)n*10)/20; printf("%lF\n",(series2 > series1) ? series2-series1 : series1-series2);
  return (series2 > series1) ? series2-series1 : series1-series2;
}

double angle(double sl, double sr) { return (sr > sl) ? (sr-sl)/WHEELDISTANCE : (sl - sr)/WHEELDISTANCE; }
double sm(double sl, double sr) { return (sl+sr)/2; }

double posX(double sm, double teta) { return sm * cos(teta); }
double posY(double sm, double teta) { return sm * cos(teta); }
double relative(double x, double x0) { return x+x0; }

double ticks_to_cm(int ticks) { return T2CM * ticks; }
double cm_to_ticks(double cm) { return cm / T2CM; }

double range_side(double angle, double distance) { return cos(angle)*distance;};
double range_front(double angle, double distance) { return sin(angle)*distance;};



double range_angle_offset(double angle) { return (abs(angle) <= 45) ? 8*cos((angle+45)*M_PI/180) : 8*sin((angle+45)*M_PI/180); }

//2 IR sensors on side
int gp2d120_ir_to_dist(int ir) { return (ir > 80) ? (2914 / (ir + 4)) - 1 : 40; }
int gp2d12_ir_to_dist(int ir) { return (ir >35) ? (6787 / (ir - 3)) - 4 : 200; }

int time_to_ticks (int unit, int voltage) { return unit * voltage* 0.156; }