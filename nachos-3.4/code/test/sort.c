/* sort.c 
 *    Sort a user-input array using bubble sort.
 */

#include "syscall.h"


void Swap(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}
 
// A function to implement bubble sort
void BubbleSort(int arr[], int n)
{
    int i, j;
    for (i = 0; i < n - 1; i++)
 
        // Last i elements are already in place
        for (j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1])
                Swap(&arr[j], &arr[j + 1]);
}

int A[100];

int main()
{
    int i, size;

    PrintString("Enter size of array: ");
    size = ReadInt();

    for (i = 0; i < size; i++) {
        PrintString("Enter element #");
        PrintInt(i + 1);
        PrintString(": ");

        A[i] = ReadInt();
    }

    BubbleSort(A, size);

    PrintString("\nSorted array: ");

    for (i = 0; i < size; i++) {
        PrintInt(A[i]);
        if (i != size - 1) {
            PrintChar(', ');
        } else {
            PrintChar('\n');
        }
    }

    PrintChar('\n');
    Halt();
}
