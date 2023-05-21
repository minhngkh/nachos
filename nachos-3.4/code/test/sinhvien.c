#include "syscall.h"

int ExtractNum(OpenFileId in, OpenFileId out);

int main() {
    OpenFileId f_temp, f_request;
    int code;
    int done = 0;

    // get_info loop
    while (done == 0) {
        Wait("have_info");

        // PrintString("Receiving info\n");

        f_temp = Open("temp.txt", 0);

        // request loop
        while (1) {
            Wait("available");

            // PrintString("having empty faucet\n");

            // write request, which contains the volume of 1 student, to request.txt
            CreateFile("request.txt");
            f_request = Open("request.txt", 0);
            code = ExtractNum(f_temp, f_request);
            Close(f_request);

            // reached EOF
            if (code == 1) {
                PrintString("sending EOF\n");

                CreateFile("request.txt");
                f_request = Open("request.txt", 0);
                Write("#", 1, f_request);
                Close(f_request);

                Signal("request");
                break;
            }

            // no more info
            if (code == 2) {
                CreateFile("request.txt");
                f_request = Open("request.txt", 0);
                Write("!", 1, f_request);
                Close(f_request);

                Signal("request");

                done = 1;
                break;
            }

            // PrintString("Getting water\n");

            Signal("request");
        }

        Close(f_temp);


        // PrintString("Requesting info\n");
            

        Signal("get_info");
    }
}

// Return 1 to signal EOF, 0 to signal normal, 2 to signal no info
int ExtractNum(OpenFileId in, OpenFileId out) {
    char c;
    int charRead = 0;

    while (1) {
        if (Read(&c, 1, in) == -2) {// reached EOF
            // check if we've already extracted but faced EOF later
            if (charRead == 0)
                return 1;
            else
                return 0;
        }

        if (c >= '0' && c <= '9') {
            Write(&c, 1, out);
            ++charRead;
        } else if (c == '!') {
            return 2;
        } else
            return 0;
    }
}