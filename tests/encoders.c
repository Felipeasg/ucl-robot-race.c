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
#include "../common.h"
#include "minunit.h"

int tests_run = 0;
int tests_fail = 0;

static char * encodersParse_test();
static char * encodersToBe_test();
static char * encodersCmd_test();
static char * encodersGet_test();

static char * all_tests() {
    it("encodersParse", encodersParse_test);
    it("encodersToBe", encodersToBe_test);
    it("encodersCmd", encodersCmd_test);
    it("encodersGet", encodersGet_test);
    return 0;
}

static char * encodersGet_test() {
  /* Scenario */
    sensors Sensors;

    describe("In normal case");
    encodersGet(&Sensors);
    should("populate encodersL encodersR", Sensors.encodersR > 0 && Sensors.encodersL > 0);
}

static char * encodersCmd_test() {
  /* Scenario */
    char s[3];
    char melr[5];
    int value1, value2;
    initSocket();

    describe("In normal case");
    encodersReset();
    encodersCmd();
    sscanf (buf,"%s %s %d %d",s, melr, &value1, &value2);
    should("have a buffer with format \"S MELR 0 0 at connection\"", !strcmp(s,"S") && !strcmp(melr, "MELR") && !value1 && !value2);
    
    moveAtVoltage(10,10);
    encodersCmd();
    sscanf (buf,"%s %s %d %d",s, melr, &value1, &value2);
    should("have a buffer with format \"S MELR >0 >0 after movement\"", !strcmp(s,"S") && !strcmp(melr, "MELR") && value1 >= 0 && value2 >= 0);
}


static char * encodersToBe_test() {
  /* Scenario */
    sensors current;
    sensors initial;
    sensors toBe;
    bool status;
    
    describe("In normal case");
    current = (sensors){.encodersL = 200, .encodersR = 200};
    initial = (sensors){.encodersL = 0, .encodersR = 0};
    toBe = (sensors){.encodersL = 200, .encodersR = 200};
    status = encodersToBe(&current, &initial, &toBe);
    should("be false when toBe is exactly as (current-initial) - LR", status==false);

    current = (sensors){.encodersL = 100, .encodersR = 100};
    initial = (sensors){.encodersL = 0, .encodersR = 0};
    toBe = (sensors){.encodersL = 200, .encodersR = 200};
    status = encodersToBe(&current, &initial, &toBe);
    should("be true when toBe is less than toBe - LR", status==true);

    current = (sensors){.encodersL = 201, .encodersR = 201};
    initial = (sensors){.encodersL = 0, .encodersR = 0};
    toBe = (sensors){.encodersL = 200, .encodersR = 200};
    status = encodersToBe(&current, &initial, &toBe);
    should("be false when toBe is more than toBe - LR", status==false);
    
    current = (sensors){.encodersL = 250, .encodersR = 150};
    initial = (sensors){.encodersL = 0, .encodersR = 0};
    toBe = (sensors){.encodersL = 200, .encodersR = 200};
    status = encodersToBe(&current, &initial, &toBe);
    should("be true when L is more and R is les than toBe", status==true);

    current = (sensors){.encodersL = 150, .encodersR = 250};
    initial = (sensors){.encodersL = 0, .encodersR = 0};
    toBe = (sensors){.encodersL = 200, .encodersR = 200};
    status = encodersToBe(&current, &initial, &toBe);
    should("be true when R is more and L is les than toBe", status==true);
    
    current = (sensors){.encodersL = 200, .encodersR = 100};
    initial = (sensors){.encodersL = 0, .encodersR = 0};
    toBe = (sensors){.encodersL = 200, .encodersR = 100};
    status = encodersToBe(&current, &initial, &toBe);
    should("be false when toBe is set diffently for each encoder", status==false);

    current = (sensors){.encodersL = 0, .encodersR = 0};
    initial = (sensors){.encodersL = 0, .encodersR = 0};
    toBe = (sensors){.encodersL = 0, .encodersR = 0};
    status = encodersToBe(&current, &initial, &toBe);
    should("return false if toBe is set as 0", status==false);

    describe("In case wheels go in opposite direction");
    
    current = (sensors){.encodersL = -200, .encodersR = 200};
    initial = (sensors){.encodersL = 0, .encodersR = 0};
    toBe = (sensors){.encodersL = 200, .encodersR = 200};
    status = encodersToBe(&current, &initial, &toBe);
    should("be OK when wheels go in different direction -L +R", status==false);

    current = (sensors){.encodersL = 200, .encodersR = -200};
    initial = (sensors){.encodersL = 0, .encodersR = 0};
    toBe = (sensors){.encodersL = 200, .encodersR = 200};
    status = encodersToBe(&current, &initial, &toBe);
    should("be OK when wheels go in different direction +L -R", status==false);
    
    current = (sensors){.encodersL = -200, .encodersR = -200};
    initial = (sensors){.encodersL = 0, .encodersR = 0};
    toBe = (sensors){.encodersL = 200, .encodersR = 200};
    status = encodersToBe(&current, &initial, &toBe);
    should("be OK when wheels go in different direction -L -R", status==false);

    describe("In case toBe is negative");
    current = (sensors){.encodersL = 200, .encodersR = 200};
    initial = (sensors){.encodersL = 0, .encodersR = 0};
    toBe = (sensors){.encodersL = -200, .encodersR = -200};
    status = encodersToBe(&current, &initial, &toBe);
    should("be OK when toBe is negative and current is positive", status==false);

    current = (sensors){.encodersL = -100, .encodersR = -100};
    initial = (sensors){.encodersL = 0, .encodersR = 0};
    toBe = (sensors){.encodersL = -200, .encodersR = -200};
    status = encodersToBe(&current, &initial, &toBe);
    should("be OK when toBe is negative and current is negative", status==true);

    return 0;
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