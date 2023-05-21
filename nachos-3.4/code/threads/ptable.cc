#include "ptable.h"

#include "system.h"

PTable::PTable(int size) {
    if (size < 0 || size > MAX_PROCESS) {
        printf("\nPTable: Invalid size\n");

        ASSERT(false);
        return;
    }

    bm = new BitMap(size);
    psize = size;

    for (int i = 0; i < MAX_PROCESS; i++) {
        pcb[i] = NULL;
    }

    // Set the current (aka parent) process as the first one
    bm->Mark(0);
    pcb[0] = new PCB(0);

    bmSem = new Semaphore("bmSem", 1);
}

PTable::~PTable() {
    if (bm)
        delete bm;

    for (int i = 0; i < psize; i++) {
        if (pcb[i]) {
            DEBUG(dbgCustom, "\nDeleting process %d, - %s", i, pcb[i]->GetFileName());
            delete pcb[i];
        }
    }

    if (bmSem)
        delete bmSem;
}

int PTable::ExecUpdate(char *name) {
    bmSem->P();

    if (!name) {
        printf("\nPTable: Invalid file name\n");

        bmSem->V();
        return -1;
    }

    // avoid executing itself
    if (strcmp(name, currentThread->getName()) == 0) {
        printf("\nPTable: Cannot execute itself");

        bmSem->V();
        return -1;
    }

    int idx = GetFreeSlot();

    if (idx == -1) {
        printf("\nPTable: Reached maximum running processes\n");

        bmSem->V();
        return -1;
    }

    // found a free slot
    pcb[idx] = new PCB(idx);
    pcb[idx]->SetFileName(name);
    pcb[idx]->parentID = currentThread->processID;

    // execute
    int pid = pcb[idx]->Exec(name, idx);

    bmSem->V();
    return pid;
}

int PTable::ExitUpdate(int ec) {
    int pid = currentThread->processID;

    // if the main process calls then simply halt
    if (pid == 0) {
        currentThread->FreeSpace();
        interrupt->Halt();

        return ec;
    }

    if (!IsExist(pid)) {
        printf("\nPTable: Invalid process ID\n");

        return -1;
    }

    // set exit code
    pcb[pid]->SetExitCode(ec);
    pcb[pcb[pid]->parentID]->DecNumWait();

    // wake up all processes waiting for this process to exit
    pcb[pid]->JoinRelease();

    pcb[pid]->ExitWait();
    Remove(pid);

    return ec;
}

int PTable::JoinUpdate(int id) {
    if (!IsExist(id)) {
        printf("\nPTable: Invalid process ID\n");

        return -1;
    }

    if (pcb[id]->parentID != currentThread->processID) {
        printf("\nPTable: Process %d is not a child of process %d\n", id, currentThread->processID);

        return -1;
    }

    // Increase the number of processes that the parent process have to wait
    pcb[pcb[id]->parentID]->IncNumWait();

    // Wait for the child process to exit
    pcb[id]->JoinWait();

    // Get the exit code of the child process
    int ec = pcb[id]->GetExitCode();

    // Allow the child process to exit (finish)
    pcb[id]->ExitRelease();

    return ec;
}

int PTable::GetFreeSlot() { return bm->Find(); }

bool PTable::IsExist(int id) {
    if (id < 0 || id >= psize)
        return false;

    return bm->Test(id);
}

void PTable::Remove(int id) {
    bm->Clear(id);

    if (pcb[id])
        delete pcb[id];
}

char *PTable::GetFileName(int id) {
    if (!IsExist(id))
        return NULL;

    return pcb[id]->GetFileName();
}