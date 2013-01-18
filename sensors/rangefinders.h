#ifndef RANGEFINDERS_INCLUDED
#define RANGEFINDERS_INCLUDED

void rangeFCmd();
void rangeFGet(sensors* Sensors);
int rangeFParse(char**, sensors*);
void rangeFSet(sensors*, int, int);
bool rangeFToBe(sensors*, sensors*, sensors*);

void rangeSCmd();
void rangeSGet(sensors* Sensors);
int rangeSParse(char**, sensors*);
void rangeSSet(sensors*, int, int);
bool rangeSToBe(sensors*, sensors*, sensors*);

#endif