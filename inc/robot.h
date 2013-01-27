#ifndef ROBOT_INCLUDED
#define ROBOT_INCLUDED

#define WHEELDISTANCE 23.65
#define WHEELDIAMETER 10.5
#define T2CM 0.091630

double slideE(int m, int n);

double angle(double sl, double sr);
double sm(double sl, double sr);

double posX(double sm, double teta);
double posY(double sm, double teta);
double relative(double x, double x0);

double ticks_to_cm(int ticks);
double cm_to_ticks(double cm);

double range_side(double angle, double distance);
double range_front(double angle, double distance);

double range_angle_offset(double angle);

int gp2d120_ir_to_dist(int ir);
int gp2d12_ir_to_dist(int ir);

int time_to_ticks (int unit, int voltage);
double wallAngle (int b, int c, int A);
#endif