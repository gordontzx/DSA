#include <bits/stdc++.h>
using namespace std;

template<typename T>
std::ostream& operator<<(std::ostream& out, const vector<T>& v) {
    for (const T& t : v)
        out << t << ' ';
    return out;
}

vector<int> buildSuffixArray(const string& s) {
    string t = s + '$';
    int n = t.size();

    vector<int> p(n), c(n), cnt(max('z'+1, n), 0);

    for (char c : t)
        cnt[c]++;
    for (int i = 1; i <= 'z'; i++)
        cnt[i] += cnt[i-1];
    for (int i = n-1; i >= 0; i--)
        p[--cnt[t[i]]] = i;
    c[p[0]] = 0;
    for (int i = 1; i < n; i++)
        c[p[i]] = c[p[i-1]] + (t[p[i]] != t[p[i-1]]);
    fill(cnt.begin(), cnt.end(), 0);

    vector<int> pn(n), cn(n);
    for (int k = 0; (1 << k) < n; k++) {
        for (int i = 0; i < n; i++) {
            pn[i] = (p[i] - (1 << k) + n) % n;
            cnt[c[pn[i]]]++;
        }

        int classes = c[p[n-1]] + 1;
        for (int i = 1; i < classes; i++)
            cnt[i] += cnt[i-1];
        for (int i = n-1; i >= 0; i--)
            p[--cnt[c[pn[i]]]] = pn[i];
        cn[p[0]] = 0;
        for (int i = 1; i < n; i++) {
            pair<int, int> prev = {c[p[i-1]], c[(p[i-1] + (1 << k)) % n]};
            pair<int, int> cur = {c[p[i]], c[(p[i] + (1 << k)) % n]};
            cn[p[i]] = cn[p[i-1]] + (prev != cur);
        }
        swap(c, cn);
        fill(cnt.begin(), cnt.begin() + classes, 0);
    }

    p.erase(p.begin());
    return p;
}

vector<int> buildLcp(string_view s, const vector<int>& suffixArray) {
    int n = s.size();
    vector<int> rank(n);
    for (int i = 0; i < n; i++)
        rank[suffixArray[i]] = i;

    vector<int> lcp(n);
    for (int i = 0, h = 0; i < n; i++) {
        if (rank[i] == n-1) {
            h = 0;
            continue;
        }

        int j = suffixArray[rank[i]+1];
        while (i+h < n && j+h < n && s[i+h] == s[j+h])
            h++;
        lcp[rank[i]] = h;
        if (h) h--;
    }

    return lcp;
}

pair<int, int> findPatternNaive(string_view pattern, string_view s, const vector<int>& suffixArray) {
    int n = s.size();

    // Find lower bound
    int l = 0, r = n-1;
    while (l < r) {
        int m = (l+r)/2;
        if (pattern <= s.substr(suffixArray[m], pattern.size()))
            r = m;
        else
            l = m+1;
    }
    if (pattern != s.substr(suffixArray[l], pattern.size()))
        return {-1, -1};
    int lo = l;

    // Find upper bound
    l = 0, r = n-1;
    while (l < r) {
        int m = (l+r+1)/2;
        if (pattern >= s.substr(suffixArray[m], pattern.size()))
            l = m;
        else
            r = m-1;
    }
    int hi = l;

    return {lo, hi};
}

bool hasPattern(string_view pattern, string_view s, const vector<int>& suffixArray) {
    int l = 0, r = s.size()-1;
    while (l <= r) {
        int m = (l+r)/2;
        int cmp = pattern.compare(s.substr(suffixArray[m], pattern.size()));
        if (cmp == 0)
            return true;
        else if (cmp > 0)
            l = m+1;
        else
            r = m-1;
    }
    return false;
}

int precompute_lcp(int l, int r, vector<int>& l_lcp, vector<int>& r_lcp, const vector<int>& lcp) {
    if (r-l <= 1) return lcp[l];
    int m = (l+r)/2;
    l_lcp[m] = precompute_lcp(l, m, l_lcp, r_lcp, lcp);
    r_lcp[m] = precompute_lcp(m, r, l_lcp, r_lcp, lcp);
    return min(l_lcp[m], r_lcp[m]);
}

int findPattern(string_view pattern, bool first, string_view s, const vector<int>& sa,
        const vector<int>& l_lcp, const vector<int>& r_lcp) {
    // IsFirst is true iff return the left bound

    auto extend_lcp = [&](int l, int i) {
        while (l < pattern.size() && sa[i]+l < s.size() && pattern[l] == s[sa[i]+l]) l++;
        return l;
    };

    int l = extend_lcp(0, 0), r = extend_lcp(0, s.size()-1);

    int L = 0, R = s.size()-1;
    while (R - L > 1) {
        int M = (L+R)/2;
        int m;

        if (l >= r) m = l_lcp[M] < l ? l_lcp[M] : extend_lcp(l, M);
        else m = r_lcp[M] < r ? r_lcp[M] : extend_lcp(r, M);

        if ((m < pattern.size() && sa[M]+m < s.size() && pattern[m] < s[sa[M]+m]) ||
            (first && m == pattern.size())) {
            R = M;
            r = m;
        } else {
            L = M;
            l = m;
        }
    }

    if (max(l, r) < pattern.size()) return -1;
    if (first) return l == pattern.size() ? L : R;
    else return r == pattern.size() ? R : L;
}

int main() {
    string s = "banana";
    vector<int> suffixArray = buildSuffixArray(s);
    vector<int> lcp = buildLcp(s, suffixArray);

    int n = s.size();
    // Print sorted suffixes
    for (int i = 0; i < s.size(); i++) {
        cout << lcp[i] << ' ' << suffixArray[i] << ' ' << s.substr(suffixArray[i]) << '\n';
    }

    vector<int> l_lcp(n), r_lcp(n);
    precompute_lcp(0, n-1, l_lcp, r_lcp, lcp);

    string pat = "an";
    int l = findPattern(pat, true, s, suffixArray, l_lcp, r_lcp);
    int r = findPattern(pat, false, s, suffixArray, l_lcp, r_lcp);
    cout << l << ' ' << r << '\n';
}
