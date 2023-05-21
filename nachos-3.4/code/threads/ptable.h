#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "pcb.h"
#include "synch.h"

#define MAX_PROCESS 10

class PTable {
  public:
    PTable(int size);
    ~PTable();

    int ExecUpdate(char *name); // handle SC_Exec
    int ExitUpdate(int ec);     // handle SC_Exit
    int JoinUpdate(int id);     // handle SC_Join
    int GetFreeSlot();          // find free slot to save new process info
    bool IsExist(int pid);      // check if given pid exists
    void Remove(int pid);       // remove pid out of the table after the process ended
    char *GetFileName(int id);  // return name of the process

  private:
    BitMap *bm; // mark used position in pcb array
    PCB *pcb[MAX_PROCESS];
    int psize;
    Semaphore *bmSem; // to prevent exec'ing multiple processes at once
};

#endif // PTABLE_H