#ifndef BUMPERS_INCLUDED
#define BUMPERS_INCLUDED

void bumpersCmd();
void bumpersGet(sensors* Sensors);
int bumpersParse(char**, sensors*);
void bumpersSet(sensors*, int, int);
bool bumpersToBe(sensors*, sensors*);

#endif