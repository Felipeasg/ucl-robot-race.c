#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <regex.h>
#include <assert.h>
#include <math.h>
#include "common.h"
#include "robot.h"
#include "minunit.h"

int tests_run = 0;
int tests_fail = 0;

static char * position_test();
static char * findPivot_test();

static char * all_tests() {
    it("position", position_test);
    it("findPivot", findPivot_test);
    return 0;
}

static char * findPivot_test() {
  dist pivot;
  describe("Find point at 0,0 angle 90, heading 45 right");
  {
    pivot = findPivot(cm_to_ticks(22.5), M_PI/2, -(M_PI/4), 0, 0);
    should("x = half wheeldistance", pivot.l == cm_to_ticks(22.5)/2);
    should("y = 0", pivot.r == 0);
  }


  describe("Find point at 0,0 angle 90, heading 45 left");
  {
    pivot = findPivot(cm_to_ticks(22.5), M_PI/2, (M_PI/4), 0, 0);
    should("x = -half wheeldistance", pivot.l == -cm_to_ticks(22.5)/2);
    should("y = 0", pivot.r == 0);
  }
  

  describe("Find point at 10,10 angle 90, heading 45 left");
  {
    pivot = findPivot(cm_to_ticks(22.5), M_PI/2, (M_PI/4), 10, 10);
    should("x = -half wheeldistance", pivot.l == -cm_to_ticks(22.5)/2+10);
    should("y = 0", pivot.r == 0+10);
  }


}
  
static char * position_test() {
  /* Scenario */
    sensors current = DEFAULT_SENSORS;
    sensors initial = DEFAULT_SENSORS;
    dist pivot = {0,0};
    double r1;
    current.encodersL = 10;
    current.encodersR = 10;
    describe("Straight line for 10 encorders");
    {
      position(&current, &initial);
      should("angle is 90", r.s.angle == M_PI/2);
      should("coords x=0", (int)r.s.x == 0); // STACKOVERFLOW
      should("coords y=0", r.s.y == 10);
      printf("\tr.s.x %lF, r.s.y %lF", r.s.x, r.s.y);
    }

    current = DEFAULT_SENSORS;
    initial = DEFAULT_SENSORS;
    r.s.x = 0;
    r.s.y = 0;
    current.encodersL = 10;
    current.encodersR = 0;
    describe("Move 10,0");
    {
      position(&current, &initial);
      should("x < y", r.s.x < r.s.y);
      should("x is positive", r.s.x > 0);
      should("y is positive", r.s.y > 0);
      should("angle < 90", r.s.angle < M_PI/2);
    }
    
    current = DEFAULT_SENSORS;
    initial = DEFAULT_SENSORS;
    r.s.x = 0;
    r.s.y = 0;
    current.encodersL = 0;
    current.encodersR = 10;
    describe("Move 0, 10");
    {
      position(&current, &initial);
      should("x < y", absDouble(r.s.x) < r.s.y);
      should("x is negative", r.s.x < 0);
      should("y is positive", r.s.y > 0);
      should("angle > 90", r.s.angle > M_PI/2);
    }

}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (tests_fail != 0) { printf("\n%d assertion failed, %d in tests\n", tests_fail, tests_run); }
    else { printf("\nALL TESTS PASSED\n"); }
    printf("Tests run: %d\n", tests_run);
    return result != 0;
}