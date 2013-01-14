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

static char * encodersParse_test();
static char * encodersToBe_test();

static char * all_tests() {
    it("encodersParse", encodersParse_test);
    it("encodersToBe", encodersToBe_test);
    return 0;
}

static char * encodersToBe_test() {
  /* Scenario */
    sensors current = {.encodersL = 200, .encodersR = 200};
    sensors initial = {.encodersL = 200, .encodersR = 200};
    sensors toBe = {.encodersL = 200, .encodersR = 200};
    encodersToBe(current, initial, toBe)
}

static char * encodersParse_test() {
  /* Scenario */
    char* elaborated[80];
    char* elaborated2[80];
    sensors Sensors1;
    sensors Sensors2;
    sensors Sensors3;
    int status;


    describe("In normal case");
    elaborated[0] = "S";
    elaborated[1] = "MELR";
    elaborated[2] = "123";
    elaborated[3] = "456";
    status = encodersParse(elaborated, &Sensors1);
    should("be converted to number for l", Sensors1.encodersL == 123);
    should("be converted to number for r", Sensors1.encodersR == 456);

    describe("Exceptions");
    elaborated[0] = "S";
    elaborated[1] = "MELR";
    elaborated[2] = "ZZZ";
    elaborated[3] = "ZZZ";
    status = encodersParse(elaborated, &Sensors2);
    should("complain silently if chars were given instead",  Sensors2.encodersL == 0 &&  Sensors2.encodersR == 0 && status==1);

    elaborated[0] = "S";
    elaborated[1] = "MELR";
    elaborated[2] = "";
    elaborated[3] = "";
    status = encodersParse(elaborated, &Sensors2);
    should("complain silently if empty string is given instead",  status==1);

    elaborated2[0] = "S";
    elaborated2[1] = "MELR";
    status = encodersParse(elaborated2, &Sensors3);
    should("complain silently if null is given", status==1);


    return 0;
}


int main(int argc, char **argv) {
    char *result = all_tests();
    if (tests_fail != 0) { printf("\n%d assertion failed, %d in tests\n", tests_fail, tests_run); }
    else { printf("\nALL TESTS PASSED\n"); }
    printf("Tests run: %d\n", tests_run);
    return result != 0;
}