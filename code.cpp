#include "code.h"

void emitComment(string comment) {
    cout << "* " << comment << endl;
    return;
}

void emit(string code) {
    cout << code;
}