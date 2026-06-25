/*
 * https://cp-algorithms.com/algebra/fft.html
 */

#include <cmath>
#include <complex>
#include <math.h>
#include <vector>

using complex_type = std::complex<double>;

const double PI = acos(-1);

std::vector<complex_type> fft(const std::vector<complex_type>& a, bool invert) {
    int n = a.size();

    std::vector<complex_type> y(a.begin(), a.end());

    // Create bit-reversal permutation
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;

        if (i < j)
            swap(y[i], y[j]);
    }

    // fft
    for (int len = 2; len <= n; len *= 2) {
        double angle = 2*PI/len * (invert ? -1 : 1);
        complex_type w1(cos(angle), sin(angle));    // 1st root

        for (int i = 0; i < n; i += len) {
            complex_type w(1);  // 0th root
            for (int j = 0; j < len/2; j++) {
                complex_type y0 = y[i+j] + w*y[i+j+len/2];
                complex_type y1 = y[i+j] - w*y[i+j+len/2];
                y[i+j] = y0;
                y[i+j+len/2] = y1;
                w *= w1;
            }
        }
    }

    if (invert) {
        for (complex_type& x : y)
            x /= n;
    }

    return y;
}

template <typename T>
std::vector<T> multiply_polynomials(const std::vector<T>& a, const std::vector<T>& b) {
    if (a.empty() || b.empty()) return {};

    int n = 1;
    while (n < a.size() + b.size() - 1) n *= 2;

    std::vector<complex_type> poly_a(a.begin(), a.end()), poly_b(b.begin(), b.end());
    poly_a.resize(n);
    poly_b.resize(n);

    std::vector<complex_type> y_a = fft(poly_a, false);
    std::vector<complex_type> y_b = fft(poly_b, false);

    std::vector<complex_type> y(n);
    for (int i = 0; i < n; i++) {
        y[i] = y_a[i] * y_b[i];
    }

    std::vector<complex_type> poly = fft(y, true);

    std::vector<T> res(n);
    for (int i = 0; i < n; i++) {
        res[i] = static_cast<T>(round(poly[i].real()));
    }

    // Trim trailing zeros
    while (!res.empty() && res.back() == 0) {
        res.pop_back();
    }

    return res;
}
