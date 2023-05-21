#ifndef PCB_H
#define PCB_H

#include "synch.h"
#include "thread.h"

#define MAX_FILENAME_LENGTH 255

class PCB {
  public:
    int parentID; // ID of parent process

    PCB(int id);
    ~PCB();

    int Exec(char *filename, int pid); // Create a new thread with given filename and process id
    int GetID();                       // return id of the the calling process
    int GetNumWait();                  // return number of waiting process

    void JoinWait();    // 1. parent process waits for child process to finish
    void ExitWait();    // 4. child process finishes
    void JoinRelease(); // 2. tell parent process to continue
    void ExitRelease(); // 3. allow child process to finish

    void IncNumWait(); // increase the number of waiting processes
    void DecNumWait(); // decrease the number of waiting processes

    void SetExitCode(int ec); // set exit code of child process
    int GetExitCode();        // return exit code of child process

    void SetFileName(char *fn); // set name for the process
    char *GetFileName();        // return name of the process

  private:
    Semaphore *joinSem; // semaphore for join process
    Semaphore *exitSem; // semaphore for exit process
    Semaphore *mutex;   // semaphore for mutual exclusion
    char *fileName; // name of the process
    Thread *thread;     // thread for the process
    int exitCode;
    int numWait; // number of joined processes
};

#endif // PCB_H