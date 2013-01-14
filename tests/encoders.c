#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <regex.h>
#include <assert.h>
#include "../common.h"
#include "minunit.h"

int tests_run = 0;
int tests_fail = 0;


static char * encodersParse_test() {
  /* Scenario */
    char* elaborated[80];
    sensors Sensors;
    elaborated[0] = "S";
    elaborated[1] = "MELR";
    elaborated[2] = "123";
    elaborated[3] = "456";
  /* Function */
    encodersParse(elaborated, &Sensors);
  /* Assertion */
    should("String converted to number for l", Sensors.encodersL == 123);
    should("String converted to number for r", Sensors.encodersR == 456);
    return 0;
}

static char * all_tests() {
    it("encodersParse", encodersParse_test);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (tests_fail != 0) {
      printf("\n%d assertion failed, %d in tests\n", tests_fail, tests_run);
    }
    else {
        printf("\nALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}