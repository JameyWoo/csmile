#include <iostream>
using namespace std;

int gcd(int u, int v) {
    if (v == 0) {
        return u;
    }
    else {
        /* u-u/v*v == u mod v */
        return gcd(v, u - u / v * v);
    }
}

int main() {
    int x;
    int y;
    cin >> x >> y;
    cout << gcd(x, y);
}