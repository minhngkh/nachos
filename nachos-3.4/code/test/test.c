#include "syscall.h"

int main() {
	int pid1, pid2;
	CreateSemaphore("test", 4);
	Wait("test");
    pid1 = Exec("./test/test-1");	
	pid2 = Exec("./test/test-2");
	Signal("test");

	Join(pid1);
	Join(pid2);
}