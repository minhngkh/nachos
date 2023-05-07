// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "syscall.h"
#include "system.h"

#include <ctype.h>

#define BUFFER_SIZE 256
#define MAX_FILENAME_LENGTH 255

//----------------------------------------------------------------------
// Helper functions
//----------------------------------------------------------------------

/**
 * @brief Copy buffer from user memory space to kernel memory space
 *
 * @param virtAddr User memory space address
 * @param limit Limit of buffer
 * @return char* Kernel buffer
 * 
 * @ref [3] Cach Viet Mot SystemCall.pdf
 */
char *User2System(int virtAddr, int limit) {
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;

    kernelBuf = new char[limit + 1]; // need for terminal string

    if (kernelBuf == NULL)
        return kernelBuf;

    memset(kernelBuf, 0, limit + 1);

    // printf("\n Filename u2s:");

    for (i = 0; i < limit; i++) {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        // printf("%c",kernelBuf[i]);
        if (oneChar == 0)
            break;
    }

    return kernelBuf;
}

/**
 * @brief Copy buffer from kernel memory space to user memory space
 *
 * @param virtAddr User memory space address
 * @param len Length of buffer
 * @param buffer Kernel buffer
 * @return int Number of bytes copied
 * 
 * @ref [3] Cach Viet Mot SystemCall-1.pdf
 */
int System2User(int virtAddr, int len, char *buffer) {
    if (len < 0)
        return -1;
    if (len == 0)
        return len;

    int i = 0;
    int oneChar = 0;
    do {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);

    return i;
}

/**
 * @brief Increment the program counter
 *
 * @ref ../machine/mipssim.cc line 118, 561-564
 */
void IncrementPC() {
    int curPC = machine->ReadRegister(PCReg);
    int nextPC = machine->ReadRegister(NextPCReg);

    machine->WriteRegister(PrevPCReg, curPC);
    machine->WriteRegister(PCReg, nextPC);
    machine->WriteRegister(NextPCReg, nextPC + 4);
}

//----------------------------------------------------------------------
// Each individual system call handler
//----------------------------------------------------------------------
void HandleHaltSyscall() {
    DEBUG(dbgCustom, "\nShutdown, initiated by user program.\n");
    interrupt->Halt();
}

void HandleReadIntSyscall() {
    char buffer[BUFFER_SIZE];
    int actualSize = gSynchConsole->Read(buffer, BUFFER_SIZE);

    int value = atoi(buffer);

    char valueStr[actualSize];

    sprintf(valueStr, "%d", value);

    // Check for any error by reverse compare (not the most efficient)
    // e.g. over/underflow, trailing non-digit character, empty,...
    // Note that -0 will fail this check but luckily the return value of error read is also 0
    if (strcmp(buffer, valueStr) != 0) {

        DEBUG(dbgCustom, "\nInvalid number\n");
        machine->WriteRegister(2, 0);
        return;
    }

    DEBUG(dbgCustom, "\nValid Number: %d\n", value);
    machine->WriteRegister(2, value);
}

void HandlePrintIntSyscall() {
    int num = machine->ReadRegister(4);

    char numStr[BUFFER_SIZE];
    sprintf(numStr, "%d", num);

    gSynchConsole->Write(numStr, strlen(numStr));
}

void HandleReadCharSyscall() {
    // Set the default value in case the user enter nothing
    char c = '\0';
    gSynchConsole->Read(&c, 1);

    DEBUG(dbgCustom, "\nRead char: %c\n", c);
    machine->WriteRegister(2, c);
}

void HandlePrintCharSyscall() {
    char c = machine->ReadRegister(4);
    gSynchConsole->Write(&c, 1);
}

void HandleReadStringSyscall() {
    int addr = machine->ReadRegister(4);
    int size = machine->ReadRegister(5);

    if (size < 0) {
        printf("\nInvalid size\n");
        interrupt->Halt();
    }
    char *buffer = new char[size + 1];

    // +1 to reserve space for the null terminator
    memset(buffer, 0, size + 1);

    int actualSize = gSynchConsole->Read(buffer, size);

    // Add the null terminator to the end of string
    buffer[actualSize] = '\0';

    DEBUG(dbgCustom, "\nRead string: %s\n", buffer);

    // Copy the string to the user space
    System2User(addr, actualSize + 1, buffer);

    delete buffer;
}

void HandlePrintStringSyscall() {
    int addr = machine->ReadRegister(4);
    char *buffer;
    int size = 0;

    while (true) {
        DEBUG(dbgCustom, "\nWriting...");
        buffer = User2System(addr + size, BUFFER_SIZE);

        // Nothing to write
        if (strlen(buffer) == 0) {
            delete buffer;
            break;
        }

        gSynchConsole->Write(buffer, strlen(buffer));

        // Keep printing if the buffer is still full
        if (strlen(buffer) == BUFFER_SIZE) {
            size += BUFFER_SIZE;
            delete buffer;
        } else {
            delete buffer;
            break;
        }
    }
}


/**
 * @brief Handle CreateFile syscall
 * 
 * @input:
 *  - r4 (addr): name 
 * @output:
 *  - r2: 0 if successful, -1 if failed
 * 
 * @ref schandle.rar -> schandle.cc
 */
void HandleCreateFileSyscall() {
    int virtAddr = machine->ReadRegister(4);
    char *fileName = User2System(virtAddr, MAX_FILENAME_LENGTH);

    if (strlen(fileName) == 0) {
        printf("\nEmpty Filename");

        machine->WriteRegister(2, -1);
        delete[] fileName;
        return;
    }

    if (strlen(fileName) >= MAX_FILENAME_LENGTH) {
        printf("\nFile name exceeds length limit");
        DEBUG(dbgCustom, "\n file name: %s", fileName);

        machine->WriteRegister(2, -1);
        delete[] fileName;
        return;
    }

    //  Create file with size = 0
    if (!fileSystem->Create(fileName, 0)) {
        printf("\nError creating file '%s'", fileName);

        machine->WriteRegister(2, -1);
        delete[] fileName;
        return;
    }

    // Success
    DEBUG(dbgCustom, "\nCreated \"%s\"", fileName);

    machine->WriteRegister(2, 0);
    delete[] fileName;
}

/**
 * @brief Handle OpenFile syscall
 * 
 * @input:
 *  - r4 (addr): name
 *  - r5 (int): access type (0: read & write, 1: read-only)
 * @output:
 *  - r2: OpenFileId or -1 if failed
 * 
 * @ref schandle.rar -> schandle.cc
 */
void HandleOpenSyscall() {
    int virtAddr = machine->ReadRegister(4);
    int accessType = machine->ReadRegister(5);

    // Check if access type is valid
    if (accessType != READ_WRITE && accessType != READ_ONLY) {
        printf("\nInvalid access type");

        machine->WriteRegister(2, -1);
        return;
    }

    // Check if file name is valid
    char *fileName = User2System(virtAddr, MAX_FILENAME_LENGTH);

    if (strlen(fileName) == 0 || strlen(fileName) >= MAX_FILENAME_LENGTH) {
        printf("\nInvalid file name");

        machine->WriteRegister(2, -1);
        delete[] fileName;
        return;
    }

    // Check if file descriptor table have any free slot
    if (fTab->IsFull()) {
        printf("\nExceed opened files limit");

        machine->WriteRegister(2, -1);
        delete[] fileName;
        return;
    }

    // Check if file exists
    OpenFile *file = fileSystem->Open(fileName);

    if (file == NULL) {
        printf("\nError opening file:  %s", fileName);

        machine->WriteRegister(2, -1);
        delete fileName;
        return;
    }

    int fileID = fTab->Add(file, accessType);

    // Success
    DEBUG(dbgCustom, "\nOpened \"%s\"", fileName);

    machine->WriteRegister(2, fileID);
    delete[] fileName;
}

/**
 * @brief Handle Close syscall
 * 
 * @input:
 * - r4 (int): OpenFileId
 * @output:
 * - r2: 0 if successful, -1 if failed
 */
void HandleCloseSyscall() {
    int id = machine->ReadRegister(4);    

    // File is automatically closed when being removed from the table
    if (!fTab->Remove(id)) {
        machine->WriteRegister(2, -1);
        return;
    }

    // Success
    DEBUG(dbgCustom, "\nClosed file with ID: %d", id);
    machine->WriteRegister(2, 0);
}

/**
 * @brief Handle Read syscall
 * 
 * @input:
 *  - r4 (addr): buffer to write to
 *  - r5 (int): char count
 *  - r6 (OpenFileId): file ID
 * @output:
 *  - r2: number of bytes read or -1 if failed, -2 if reached EOF
 */
void HandleReadSyscall() {
    int virtAddr = machine->ReadRegister(4);
    int size = machine->ReadRegister(5);
    int id = machine->ReadRegister(6);

    // Check if buffer is valid
    if (size <= 0) {
        DEBUG(dbgCustom, "\nInvalid buffer size");

        machine->WriteRegister(2, -1);
        return;
    }

    // +1 to reserve space for nul terminator
    char *temp = new char[size + 1];
    memset(temp, 0, size + 1);
    
    int actualSize = fTab->Read(temp, size, id);

    // Failed
    if (actualSize == -1) {
        machine->WriteRegister(2, -1);
        delete[] temp;
        return;
    }
    // Reach EOF
    if (actualSize == 0) {
        machine->WriteRegister(2, -2);
        delete[] temp;
        return;
    }

    // again, +1 to reserve space for null terminator
    System2User(virtAddr, actualSize + 1, temp);

    delete[] temp;
    machine->WriteRegister(2, actualSize);
}

/**
 * @brief Handle Write syscall
 * 
 * @input:
 * - r4 (addr): buffer to read from
 * - r5 (int): char count
 * - r6 (OpenFileId): file ID
 * @output:
 * - r2: number of bytes written or -1 if failed
 * 
 */
void HandleWriteSyscall() {
    int virtAddr = machine->ReadRegister(4);
    int size = machine->ReadRegister(5);
    int id = machine->ReadRegister(6);

    // Check if buffer is valid
    if (size <= 0) {
        DEBUG(dbgCustom, "\nInvalid buffer size");

        machine->WriteRegister(2, -1);
        return;
    }

    char *temp = User2System(virtAddr, size);
    

    int actualSize = fTab->Write(temp, size, id);

    // Failed
    if (actualSize == -1) {
        machine->WriteRegister(2, -1);
        delete[] temp;
        return;
    }

    DEBUG(dbgCustom, "\nWrote %d bytes to file with ID: %d\n", actualSize, id);

    delete[] temp;
    machine->WriteRegister(2, actualSize);
}

//----------------------------------------------------------------------
// Each individual exception handler
//----------------------------------------------------------------------
void HandleSyscallException() {
    // Read the type file system call from the register
    int type = machine->ReadRegister(2);

    switch (type) {
    case SC_Halt:
        HandleHaltSyscall();
        break;

    case SC_ReadInt:
        HandleReadIntSyscall();
        break;

    case SC_PrintInt:
        HandlePrintIntSyscall();
        break;

    case SC_ReadChar:
        HandleReadCharSyscall();
        break;

    case SC_PrintChar:
        HandlePrintCharSyscall();
        break;

    case SC_ReadString:
        HandleReadStringSyscall();
        break;

    case SC_PrintString:
        HandlePrintStringSyscall();
        break;
    
    case SC_CreateFile:
        HandleCreateFileSyscall();
        break;
    case SC_Open:
        HandleOpenSyscall();
        break;
    case SC_Close:
        HandleCloseSyscall();
        break;
    case SC_Read:
        HandleReadSyscall();
        break;
    case SC_Write:
        HandleWriteSyscall();
        break;

    // All the system calls below has not been implemented
    case SC_Exit:
    case SC_Exec:
    case SC_Join:
    case SC_Fork:
    case SC_Yield:
        printf("\nUnimplemented system call. Code: %d\n", type);
        interrupt->Halt();
        break;

    default:
        printf("\nUnexpected system call. Code: %d\n", type);
        interrupt->Halt();
        break;
    }

    // Increment the program counter to avoid infinite loop
    IncrementPC();
}

void HandlePageFaultException() {
    DEBUG(dbgCustom, "\nPage fault exception.");
    printf("\nNo valid translation found.\n");
    interrupt->Halt();
}

void HandleReadOnlyException() {
    DEBUG(dbgCustom, "\nRead-only exception.");
    printf("\nWrite attempted to page marked \"read-only\".\n");
    interrupt->Halt();
}

void HandleBusErrorException() {
    DEBUG(dbgCustom, "\nBus error exception.");
    printf("\nTranslation resulted in an invalid physical address.\n");
    interrupt->Halt();
}

void HandleAddressErrorException() {
    DEBUG(dbgCustom, "\nAddress error exception.");
    printf("\nUnaligned reference or one that was beyond the end file the address space.\n");
    interrupt->Halt();
}

void HandleOverflowException() {
    DEBUG(dbgCustom, "\nOverflow exception.");
    printf("\nInteger overflow in add or sub.\n");
    interrupt->Halt();
}

void HandleIllegalInstrException() {
    DEBUG(dbgCustom, "\nIllegal instruction exception.");
    printf("\nUnimplemented or reserved instruction.\n");
    interrupt->Halt();
}

void HandleNumExceptionTypes() {
    DEBUG(dbgCustom, "\nNumber exception types.");
    printf("\nNumber exception types.\n");
    interrupt->Halt();
}

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which) {
    // int type = machine->ReadRegister(2);

    // if ((which == SyscallException) && (type == SC_Halt)) {
    //     DEBUG('a', "Shutdown, initiated by user program.\n");
    //     interrupt->Halt();
    // } else {
    //     printf("Unexpected user mode exception %d %d\n", which, type);
    //     ASSERT(FALSE);
    // }

    switch (which) {
    case NoException:
        break;

    case SyscallException:
        HandleSyscallException();
        break;

    // All the exceptions below will simply notify the user about the exception then halt the
    // program right after
    case PageFaultException:
        HandlePageFaultException();
        break;

    case ReadOnlyException:
        HandleReadOnlyException();
        break;

    case BusErrorException:
        HandleBusErrorException();
        break;

    case AddressErrorException:
        HandleAddressErrorException();
        break;

    case OverflowException:
        HandleOverflowException();
        break;

    case IllegalInstrException:
        HandleIllegalInstrException();
        break;

    case NumExceptionTypes:
        HandleNumExceptionTypes();
        break;

    default:
        int type = machine->ReadRegister(2);
        printf("\nUnexpected user mode exception %d %d\n", which, type);
        interrupt->Halt();
        break;
    }
}
