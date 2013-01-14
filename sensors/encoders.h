#ifndef ENCODERS_INCLUDED
#define ENCODERS_INCLUDED

void encordersCmd();
void encodersGet(sensors* Sensors);
int encodersParse(char**, sensors*);
bool encodersToBe(sensors*, sensors*, sensors*);

#endif