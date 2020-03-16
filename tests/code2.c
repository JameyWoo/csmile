int main(void) {
    int a;
    int b;
    int c;
    c = 123;
    b = 666;
    a = 2;
    output(a);
    a = c;
    output(a);
    a = c = b;
    output(a);
    a = b + c;
    output(a);
    a = b - c;
    output(a);
    a = 6;
    c = 2;
    a = a * c;
    output(a);
    a = a / c;
    output(a);
    output(1);
    a = 1;
    if (a == 1) {
        output(999);
    } else {
        output(555);
    }
}