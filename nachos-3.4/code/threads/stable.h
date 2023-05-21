#ifndef STABLE_H
#define STABLE_H

#include "bitmap.h"
#include "synch.h"
#include "sem.h"

#define MAX_SEMAPHORE 10

class STable {
  public:
    STable();
    ~STable();

    int Create(char *name, int init);
    int Wait(char *name);
    int Signal(char *name);
    int FindFreeSlot();


  private:
    BitMap *bm; // manage free slots
    Sem *semTab[MAX_SEMAPHORE]; // manage maximum of 10 semaphores

    bool Exist(char *name);
};

#endif // STABLE_H