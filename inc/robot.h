#ifndef ROBOT_INCLUDED
#define ROBOT_INCLUDED

#define WHEELDISTANCE 22.5
#define WHEELDIAMETER 10.5
#define T2CM 0.091630

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "common.h"
#include "robot.h"

double slideE(int m, int n);

double angle(double sl, double sr);
dist delta (dist current, dist previous);
double deltaAngle(dist delta);
double deltaDistance(dist delta);

double posX(double middle, double angle);
double posY(double middle, double angle);

double relative(double x, double x0);

double ticks_to_cm(int ticks);
double cm_to_ticks(double cm);

double range_side(double angle, double distance);
double range_front(double angle, double distance);



double range_angle_offset(double angle);

//2 IR sensors on side
int gp2d120_ir_to_dist(int ir);
int gp2d12_ir_to_dist(int ir);

int time_to_ticks (int unit, int voltage);
double wallAngle (int b, int c, int A);


void move_to_wall();

void wall_follow();
void hall_follow();

void hall_turning_point();

double ratios(sensors *Sensors);


#endif