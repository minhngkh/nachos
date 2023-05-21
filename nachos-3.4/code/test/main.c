#include "syscall.h"

int ReadNum(OpenFileId file);
void ExtractLine(OpenFileId in, OpenFileId out);

int main() {
    int times, i;
    OpenFileId f_input, f_temp;
    int sinhvien, voinuoc;

    CreateSemaphore("get_info", 1);
    CreateSemaphore("have_info", 0);
    CreateSemaphore("available", 1);
    CreateSemaphore("request", 0);

    sinhvien = Exec("./test/sinhvien");
    voinuoc = Exec("./test/voinuoc");

    CreateFile("output.txt");

    f_input = Open("input.txt", 1); // Open to read

    times = ReadNum(f_input);
    for (i = 0; i < times; i++) {
        Wait("get_info");

        // PrintString("getting info\n");

        // write the current line to temp.txt
        CreateFile("temp.txt");
        f_temp = Open("temp.txt", 0);
        ExtractLine(f_input, f_temp);
        Close(f_temp);

        // PrintString("having info\n");

        Signal("have_info");
    }

    Close(f_input);

    Wait("get_info");

    // notify sinhvien to stop
    CreateFile("temp.txt");
    f_temp = Open("temp.txt", 0);
    Write("!", 1, f_temp);
    Close(f_temp);
    Signal("have_info");

    Join(sinhvien);
    Join(voinuoc);
}

int ReadNum(OpenFileId file) {
    char c;
    int num;

    num = 0;

    while (1) {
        if (Read(&c, 1, file) == -2) // reached EOF
            break;

        if (c >= '0' && c <= '9') {
            num = num * 10 + (c - '0');
        } else if (c == '#') {
            return -1;
        } else if (c == '!') {
            return -2;
        } else
            break;
    }

    return num;
}

void ExtractLine(OpenFileId in, OpenFileId out) {
    char c;
    int i;

    i = 0;
    while (1) {
        if (Read(&c, 1, in) == -2)
            return;
        if (c == '\n')
            return;

        Write(&c, 1, out);
    }
}