#ifndef US_INCLUDED
#define US_INCLUDED

void usCmd();
void usGet(sensors* Sensors);
int usParse(char**, sensors*);
void usSet(sensors*, int);
bool usToBe(sensors*, sensors*, sensors*);

#endif