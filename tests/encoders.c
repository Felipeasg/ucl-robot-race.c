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


static void encodersCmdTest() {
    assert(0);
}

static void encodersParseTest() {

    char* elaborated[5];
    sensors* Sensors;
    elaborated[0] = "S";
    elaborated[1] = "MELR";
    elaborated[2] = "123";
    elaborated[3] = "456";
    encodersParse(elaborated, Sensors);
    assert(Sensors->encodersL == 123);
    assert(Sensors->encodersR == 456);
}

int main (){

//    encodersCmdTest();
    encodersParseTest();
    
}
