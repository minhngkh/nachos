#include "pcb.h"
#include "system.h"

PCB::PCB(int id) {
    if (id == 0) {
        parentID = -1;
    } else {
        parentID = currentThread->processID;
    }

    joinSem = new Semaphore("joinSem", 0);
    exitSem = new Semaphore("exitSem", 0);
    mutex = new Semaphore("mutex", 1);
    exitCode = 0;
    numWait = 0;

    fileName = new char[MAX_FILENAME_LENGTH + 1];
    memset(fileName, 0, MAX_FILENAME_LENGTH + 1);
}

PCB::~PCB() {
    delete joinSem;
    delete exitSem;
    delete mutex;

    if (thread) {
        thread->FreeSpace();
        thread->Finish();
    }

    delete[] fileName;
}

/**
 * @brief Custom start process for PCB::Exec
 *
 * @param pid process id
 */
void CustomStartProcess(int pid) {
    char *fileName = pTab->GetFileName(pid);
    AddrSpace *space = new AddrSpace(fileName);

    if (space == NULL) {
        printf("\nPCB::Exec: Not enough memory to read file\n");
        return;
    }

    currentThread->space = space;

    space->InitRegisters(); // set the initial register values
    space->RestoreState();  // load page table register

    machine->Run(); // jump to the user program
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}

int PCB::Exec(char *filename, int pid) {
    // using mutex to avoid mutiple processes executing at the same time
    mutex->P();

    thread = new Thread(filename);

    if (!thread) {
        printf("\nPCB::Exec: Not enough memory to start process\n");

        mutex->V();
        return -1;
    }

    // set pid for the new process
    thread->processID = pid;

    // set parent id to the current calling process
    parentID = currentThread->processID;

    thread->Fork(CustomStartProcess, pid);

    mutex->V();

    // TODO: this or the calling process PID?
    return pid;
}

int PCB::GetID() { return thread->processID; }

int PCB::GetNumWait() { return numWait; }

void PCB::JoinWait() { joinSem->P(); }

void PCB::ExitWait() { exitSem->P(); }

void PCB::JoinRelease() { joinSem->V(); }

void PCB::ExitRelease() { exitSem->V(); }

void PCB::IncNumWait() {
    mutex->P();
    ++numWait;
    mutex->V();
}

void PCB::DecNumWait() {
    mutex->P();
    if (numWait > 0)
        --numWait;
    mutex->V();
}

void PCB::SetExitCode(int ec) { exitCode = ec; }

int PCB::GetExitCode() { return exitCode; }

void PCB::SetFileName(char *fn) { strcpy(fileName, fn); }

char *PCB::GetFileName() { return fileName; }