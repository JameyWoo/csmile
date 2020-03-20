int func(int a, int b) {
    return b;
}

void main(void) {
    int a;
    int b;
    a = input();
    b = func(4, a);
    output(b);
}