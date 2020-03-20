int gcd(int u, int v) {
    int tmp;
    if (v == 0) {
        return u;
    } else {
        /* u-u/v*v == u mod v */
        tmp = u / v;
        tmp = tmp * v;
        return gcd(v, u - tmp);
    }
}

void main(void) {
    int x;
    int y;
    x = input();
    y = input();
    
    output(gcd(x, y));
}