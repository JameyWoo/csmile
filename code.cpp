#include "code.h"

ofstream midCodeOut("Output-midcode.txt", ios::out);

void emitComment(string comment) {
    midCodeOut << "* " << comment << endl;
    return;
}

void emit(string code) {
    midCodeOut << code;
}