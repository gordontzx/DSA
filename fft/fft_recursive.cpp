/*
 * https://cp-algorithms.com/algebra/fft.html
 */

#include <cmath>
#include <complex>
#include <vector>

using complex_type = std::complex<double>;

const double PI = acos(-1);

std::vector<complex_type> fft(const std::vector<complex_type>& a, bool invert) {
    int n = a.size();
    if (n == 1) return a;

    std::vector<complex_type> a0(n/2), a1(n/2);
    for (int i = 0; i < n/2; i++) {
        a0[i] = a[2*i];
        a1[i] = a[2*i+1];
    }

    std::vector<complex_type> y0 = fft(a0, invert);
    std::vector<complex_type> y1 = fft(a1, invert);

    double angle = 2*PI/n * (invert ? -1 : 1);
    complex_type w(1);                          // 0th root
    complex_type w1(cos(angle), sin(angle));    // 1st root

    std::vector<complex_type> res(n);
    for (int i = 0; i < n/2; i++) {
        res[i] = y0[i] + w*y1[i];
        res[i+n/2] = y0[i] - w*y1[i];
        if (invert) {
            res[i] /= 2;
            res[i+n/2] /= 2;
        }
        w *= w1;
    }

    return res;
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

    std::vector<complex_type> poly_inv = fft(y, true);

    std::vector<T> res(n);
    for (int i = 0; i < n; i++) {
        res[i] = static_cast<T>(round(poly_inv[i].real()));
    }

    // Trim trailing zeros
    while (!res.empty() && res.back() == 0) {
        res.pop_back();
    }

    return res;
}
