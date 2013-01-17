#ifndef RANGEFINDERS_INCLUDED
#define RANGEFINDERS_INCLUDED

void rangeFCmd();
void rangeFGet(sensors* Sensors);
int rangeFParse(char**, sensors*);
void rangeFSet(sensors*, int, int);
bool rangeFToBe(sensors*, sensors*, sensors*);

#endif