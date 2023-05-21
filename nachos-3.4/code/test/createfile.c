#include "syscall.h"

int main() {
    int f1, f2, c;
    char *str;

    CreateFile("test1.txt");
    CreateFile("test2.txt");
    CreateFile("test3.txt");
    CreateFile("test4.txt");

    f1 = Open("test1.txt", 0);
    f2 = Open("test2.txt", 0);
    Open("test3.txt", 0);
    Open("test4.txt", 0);

    Write("Hello world\ntest ", 16, f1);
    Close(f1);
    f1 = Open("test1.txt", 1);

    PrintChar('\n');

     while (1) {
        c = Read(str, 10, f1);
		if (c == -2) break; // Reached EOF

		PrintString("\nWrite: ");
        PrintInt(Write(str, c, f2));
		PrintString(" chars\n");
    }

    Halt();
}