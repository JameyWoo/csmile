int gcd(int u, int v) {
    if (v == 0) {
        return u;
    } else {
        /* u-u/v*v == u mod v */
        return gcd(v, u - u / v * v);
    }
}

void main(void) {
    int x;
    int y;
    x = input();
    y = input();
    output(x);
    output(y);
    output(gcd(x, y));
}