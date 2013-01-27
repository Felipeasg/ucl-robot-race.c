#ifndef CHECK_INCLUDED
#define CHECK_INCLUDED

dist checkBack(bool accurate);
double checkFront(bool accurate);
dist checkSide(bool accurate, int angleL, int angleR);
void considerRotation(int direction);

#endif