/*
求 n 的阶乘
n 从终端输入
*/

void main(void) {
    int a;
    int b;
    b = 1;
    a = input();
    while (a > 0) {
        b = b * a;
        a = a - 1;
    }
    output(b);
}