#include "FDTable.h"

#include "system.h"

FDTable::FDTable() {
    // stdin
    table[STDIN].inUse = true;
    table[STDIN].accessType = READ_ONLY;

    // stdout
    table[STDOUT].inUse = true;
    table[STDOUT].accessType = WRITE_ONLY;
}

FDTable::~FDTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table[i].file != nullptr) {
            delete table[i].file;
        }
    }
}

int FDTable::FindFreeSlot() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (!table[i].inUse) {
            return i;
        }
    }

    return -1;
}

bool FDTable::Exist(int id) {
    if (id < 0 && id >= TABLE_SIZE)
        return false;

    if (table[id].inUse)
        return true;

    return false;
}

bool FDTable::IsFull() { return FindFreeSlot() == -1; }

/**
 * @brief Add a file to the file descriptor table
 *
 * @param file pointer to the file instance
 * @param accessType read-only or read-write
 * @return OpenFileID index in the table
 */
OpenFileID FDTable::Add(OpenFile *file, int accessType) {
    int freeSlot = FindFreeSlot();

    if (freeSlot == -1) {
        return -1;
    }

    table[freeSlot].inUse = true;
    table[freeSlot].file = file;
    table[freeSlot].accessType = accessType;

    return freeSlot;
}

/**
 * @brief Remove a file from the file descriptor table
 *
 * @param id index of the file in the table
 * @return true if successful otherwise false
 */
bool FDTable::Remove(OpenFileID id) {
    if (id < 0 || id >= TABLE_SIZE) {
        printf("\nInvalid ID");
        DEBUG(dbgCustom, ": %d", id);

        return false;
    }

    if (!table[id].inUse) {
        printf("\nFile not found");
        DEBUG(dbgCustom, " - ID: %d", id);

        return false;
    }

    if (id == STDIN || id == STDOUT) {
        printf("\nError closing file");
        DEBUG(dbgCustom, " - Cannot remove stdin or stdout");
        return false;
    }


    if (table[id].file != nullptr) {
        // OpenFile class have destructor that closes the file when the instance is deleted
        delete table[id].file;
    }
    table[id].inUse = false;
    table[id].file = nullptr;

    return true;
}

int FDTable::Read(char *buffer, int charcount, OpenFileID id) {
    if (id < 0 || id >= TABLE_SIZE) {
        return -1;
    }

    if (table[id].accessType == WRITE_ONLY) {
        DEBUG(dbgCustom, "Doesn't have read access");
        return -1;
    }

    if (id == STDIN) {
        return gSynchConsole->Read(buffer, charcount);
    }

    return table[id].file->Read(buffer, charcount);
}

int FDTable::Write(char *buffer, int charcount, OpenFileID id) {
    if (id < 0 || id >= TABLE_SIZE) {
        return -1;
    }

    if (table[id].accessType == READ_ONLY) {
        DEBUG(dbgCustom, "Doesn't have write access");
        return -1;
    }

    if (id == STDOUT) {
        return gSynchConsole->Write(buffer, charcount);
    }

    return table[id].file->Write(buffer, charcount);
}