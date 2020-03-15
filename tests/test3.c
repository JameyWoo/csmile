int func1(int a, int b, int c) {
    /* int a;*/
    if (a == 1) {
        b = 0;
        while (b < 10) {
            b = b + 1;
        }
    } else {
        c = func1(1, 2, c);
        return c;
    }

    return a + b + c;
}

int main(void) {
    int a;
    int b;
    int c;

    a = 1;
    c = 4;
    return func1(a, b, c);
}