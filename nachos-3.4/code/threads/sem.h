#ifndef SEM_H
#define SEM_H

#include "synch.h"
#include "string.h"

class Sem {
  public:
    Sem(char *na, int i) {
        strcpy(this->name, na);
        sem = new Semaphore(name, i);
    }

    ~Sem() { delete sem; }

    void wait() { sem->P(); }

    void signal() { sem->V(); }

    char *GetName() { return name; }

  private:
    char name[50];
    Semaphore *sem;
};

#endif // SEM_H
