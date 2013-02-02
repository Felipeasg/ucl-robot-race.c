#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "common.h"
#include "robot.h"

const int INTERIOR = 1;
const int EXTERIOR = 2;
const int ILL = 3;
const int PORTAL = 4;
const int SPLITL = 5;
const int SPLITF = 6;
const int TERMINATE = 7;

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
double wallAngle (int b, int c, int A) { return asin( (c * sin(A)) / (sqrt( pow(b,2) + pow(c,2) - 2*b*c ) ) ); }


void move_to_wall() {
  // checkSensors();
  // check if something on the left
  // check if something on the right
  // check if there is something in front
  // if it is further, go straight, otherwise turn right and hold the left
}

void wall_follow() {
  // there is a rangemin and a rangemax
  // there is a range0
  // robot is distant range0 on the L/R
  // check opposite sensor in case go to hall follow
}
void hall_follow() {
  // mantains equidistance 
  // check for SPLIT or PORTAL
}

void hall_turning_point() {
  
}

double ratios(sensors *Sensors) {
  
  // int differenceBack = r.s.rangeSR - r.s.rangeSL; int differenceSide = r.s.rangeFR - r.s.rangeFL;
  // double ratioBack = (double) r.s.rangeSR / r.s.rangeSL;
  double ratioSide = (double) Sensors->rangeFR / Sensors->rangeFL;

  // dist expectDifferenceBack = { 0 - r.s.rangeSL, 0 - r.s.rangeSR}; dist expectDifferenceSide = {27 - r.s.rangeFL,27 - r.s.rangeFR};
  // dist expectRatioBack = {27.0 / r.s.rangeSL,27.0 / r.s.rangeSR}; dist expectRatioSide = {27.0 / r.s.rangeFL,27.0 / r.s.rangeFR};

  // printf("PARSE sl%i sr%i fl%i fr%i us%i\n", r.s.rangeSL, r.s.rangeSR, r.s.rangeFL, r.s.rangeFR, r.s.us);
  return ratioSide;
  
}

