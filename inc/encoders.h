#ifndef ENCODERS_INCLUDED
#define ENCODERS_INCLUDED

void encodersCmd();
void encodersGet(sensors* Sensors);
int encodersParse(char**, sensors*);
void encodersSet(sensors*, int, int);
void encodersReset();
bool encodersToBe(sensors*, sensors*, sensors*);

#endif