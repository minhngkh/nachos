#include "syscall.h"

int ReadNum(OpenFileId file);
int ExtractNum(OpenFileId in, OpenFileId out);

#define MAX_DIGIT 3

int main() {
    OpenFileId f_request, f_output;
    int volume;
    int done = 0;
    int faucet1, faucet2;
    char buffer[MAX_DIGIT];

    f_output = Open("output.txt", 0);

    while (done == 0) {
        faucet1 = faucet2 = 0; // volume of water that was filled

        while (1) {
            Wait("request");

            // read request.txt
            f_request = Open("request.txt", 1);
            volume = ExtractNum(f_request, f_output);
            Close(f_request);

            if (volume == -1) {
                Write("\n", 1, f_output);
                Signal("available");
                break;
            }

            if (volume == -2) {
                done = 1;
                break;
            }

            // accept request, at least 1 faucet is empty at this point
            if (faucet1 == 0) {
                faucet1 = volume;
                Write("1 ", 2, f_output);
            } else {
                faucet2 = volume;
                Write("2 ", 2, f_output);
            }

            // simulate filling water until 1 faucet is empty
            if (faucet1 > faucet2) {
                faucet1 -= faucet2;
                faucet2 = 0;
            } else {
                faucet2 -= faucet1;
                faucet1 = 0;
            }

            // at least 1 faucet is empty by now
            Signal("available");
        }
    }
}

// return -1 to indicate reset, -2 end, others are normal num
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

// Return 1 to signal EOF, 0 to signal normal, 2 to signal no info
int ExtractNum(OpenFileId in, OpenFileId out) {
    char c;
    int num;

    num = 0;

    while (1) {
        if (Read(&c, 1, in) == -2) // reached EOF
            break;

        if (c >= '0' && c <= '9') {
            Write(&c, 1, out);
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