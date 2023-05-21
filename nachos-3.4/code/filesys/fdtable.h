#ifndef FDTABLE_H
#define FDTABLE_H

#include "filesys.h"

typedef int OpenFileID;

// Different types of file access
#define READ_WRITE 0
#define READ_ONLY 1
#define WRITE_ONLY 2

// Size of the file descriptor table
#define TABLE_SIZE 10

// Reserved table slots for stdin and stdout
#define STDIN 0
#define STDOUT 1

struct FileDescriptor {
    bool inUse = false;
    OpenFile *file = nullptr;
    int accessType;
};

class FDTable {
  public:
    FDTable();
    ~FDTable();

    bool Exist(int id);
    bool IsFull();
    OpenFileID Add(OpenFile *file, int accessType);
    bool Remove(OpenFileID id);
    int Read(char *buffer, int charcount, OpenFileID id);
    int Write(char *buffer, int charcount, OpenFileID id);

  private:
    FileDescriptor table[TABLE_SIZE];

    int FindFreeSlot();
};

#endif /* FDTABLE_H */