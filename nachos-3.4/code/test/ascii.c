/* help.c 
 *    Display a table of all visible ascii characters.
 */
#include "syscall.h"

int main() {
    int start = 33;
    int end = 126;
    int cols = 6;
    int i, j, rows;
    unsigned char c;

    // alternative to ceil()
    rows = ((end - start) / cols) + (((end - start) % cols) != 0);

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            c = start + (j * rows) + i;

            if (c > end) break;

            PrintInt(c);
            PrintChar(' ');
            PrintChar(c);

            if (j != cols - 1) {
                PrintChar('\t');
            }
        }
        PrintChar('\n');
    }

    PrintChar('\n');
    Halt();
}