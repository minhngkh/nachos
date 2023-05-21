#include "stable.h"

STable::STable()
{
    bm = new BitMap(MAX_SEMAPHORE);
    for (int i = 0; i < MAX_SEMAPHORE; i++)
        semTab[i] = NULL;
}

STable::~STable() {
    delete bm;

    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        if (semTab[i] != NULL)
            delete semTab[i];
    }
}

int STable::Create(char *name, int init) {
    if (Exist(name)) // name already exist 
        return -1;

    int index = FindFreeSlot();
    if (index == -1) // no free slot
        return -1;

    semTab[index] = new Sem(name, init);
    return 0;
}

int STable::Wait(char *name) {
    // find the corresponding semaphore index
    int index = -1;
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        if (semTab[i] != NULL && strcmp(semTab[i]->GetName(), name) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) // not found
        return -1;

    semTab[index]->wait();
    return 0;
}

int STable::Signal(char *name) {
    int index = -1;
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        if (semTab[i] != NULL && strcmp(semTab[i]->GetName(), name) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) // not found
        return -1;

    semTab[index]->signal();
    return 0;
}

int STable::FindFreeSlot() {
    return bm->Find();
}

bool STable::Exist(char *name) {
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        if (semTab[i] != NULL && strcmp(semTab[i]->GetName(), name) == 0)
            return true;
    }

    return false;
}