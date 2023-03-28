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
    DEBUG('a', "Shutdown, initiated by user program.\n");
    interrupt->Halt();
}

void HandleReadIntSyscall() {
    printf("test");
}

void HandlePrintIntSyscall() {}

void HandleReadCharSyscall() {}

void HandlePrintCharSyscall() {}

void HandleReadStringSyscall() {}

void HandlePrintStringSyscall() {}

//----------------------------------------------------------------------
// Each individual exception handler
//----------------------------------------------------------------------
void HandleSyscallException() {
    // Read the type of system call from the register
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

    // All the system calls below has not been implemented
    case SC_Exit:
    case SC_Exec:
    case SC_Join:
    case SC_Create:
    case SC_Open:
    case SC_Write:
    case SC_Close:
    case SC_Fork:
    case SC_Yield:
        printf("Unimplemented system call #%d\n", type);
        interrupt->Halt();
        break;

    default:
        printf("Unexpected system call %d\n", type);
        interrupt->Halt();
        break;
    }

    // Increment the program counter to avoid infinite loop
    IncrementPC();
}

void HandlePageFaultException() {
    DEBUG('a', "Page fault exception.\n");
    printf("No valid translation found.\n");
    interrupt->Halt();
}

void HandleReadOnlyException() {
    DEBUG('a', "Read-only exception.\n");
    printf("Write attempted to page marked \"read-only\".\n");
    interrupt->Halt();
}

void HandleBusErrorException() {
    DEBUG('a', "Bus error exception.\n");
    printf("Translation resulted in an invalid physical address.\n");
    interrupt->Halt();
}

void HandleAddressErrorException() {
    DEBUG('a', "Address error exception.\n");
    printf("Unaligned reference or one that was beyond the end of the address space.\n");
    interrupt->Halt();
}

void HandleOverflowException() {
    DEBUG('a', "Overflow exception.\n");
    printf("Integer overflow in add or sub.\n");
    interrupt->Halt();
}

void HandleIllegalInstrException() {
    DEBUG('a', "Illegal instruction exception.\n");
    printf("Unimplemented or reserved instruction.\n");
    interrupt->Halt();
}

void HandleNumExceptionTypes() {
    DEBUG('a', "Number exception types.\n");
    printf("Number exception types.\n");
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
        printf("Unexpected user mode exception %d %d\n", which, type);
        interrupt->Halt();
        break;
    }
}
