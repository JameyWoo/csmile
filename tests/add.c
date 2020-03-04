int add(int a, int b) {
    int c;
    c = a + b;
    return c;
}

/* 如果函数没有参数, 那么需要使用void, 而不能为空 */
int main(void) {
    int ans;
    ans = add(5, 3);
}