#include <stdio.h>
#include <math.h>

int abs (int value) {
  if (value < 0) return -value;
  return value;
}

float calculateSlide(int m, int n) {
  m = m/10;
  printf("%i\n",m);
  n = n/10;
  printf("%i\n",n);
  float series1, series2;
  series1 = ((float)m/2)*(10+(float)m*10)/20;
  printf("%f\n",series1);
  series2 = ((float)n/2)*(10+(float)n*10)/20;
  printf("%f\n",series2);
  return abs(series2-series1);
}

int main () {
  int due = 12;

  printf("%f\n",calculateSlide(50,100));

  return 0;
}
