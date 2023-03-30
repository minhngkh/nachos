/* help.c 
 *    Display information about our team and available programs.
 */
#include "syscall.h"

int main() {
    PrintString("Members:\n");
    PrintString("1. 19127336 - La Gia Bao\n");
    PrintString("2. 21127357 - Nguyen Khac Nhat Minh\n");
    PrintString("3. 21127472 - Vo Minh Tuan\n");
    PrintString("4. 21127498 - Tran Quang Dao\n");
    PrintString("5. 21127591 - Nguyen Hien Dat\n");
    PrintString("--------------------------------------\n");
    PrintString("Programs:\n");
    PrintString("1. sort: User inputs size of an array and its elements, the program then sorts it using bubble sort and displays the result on the screen.\n");
    PrintString("2. ascii: Display a table of all visible ascii characters on the screen.\n");

}