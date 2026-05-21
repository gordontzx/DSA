#include <bits/stdc++.h>
using namespace std;

string repeat(string str, int count) {
    string res;
    for (int i = 0; i < count; i++)
        res += str;
    return res;
}

/*
 * O(m) construction of suffix trees, suffix links are stitched together during extensions.
 */
class SuffixTree {
private:
    struct Node {
        int l, r;
        int next[27];
        int par;
        int link;

        Node(int l, int r, int par = -1, int link = -1)
            : l(l), r(r), par(par), link(link) {}

        int len() {
            return r-l;
        }
    };

    struct Point {
        int node;
        int pos;

        Point(int node, int pos) : node(node), pos(pos) {}
    };

    static constexpr char SENTINEL = 'z' + 1;
    string s;
    vector<Node> trie;
    int nextId = 1;

    Point go(Point p, int l, int r) {
        while (l < r) {
            if (p.pos == trie[p.node].len()) {
                // At child node
                p.node = trie[p.node].next[s[l]-'a'];
                if (p.node == 0)
                    return Point(-1, -1);
                p.pos = 0;
            } else {
                if (s[trie[p.node].l + p.pos] != s[l])
                    return Point(-1, -1);
                if (r-l < trie[p.node].len() - p.pos)
                    return Point(p.node, p.pos + r-l);
                l += trie[p.node].len() - p.pos;
                p.pos = trie[p.node].len();
            }
        }
        return p;
    }

    int split(Point p) {
        if (p.pos == trie[p.node].len()) return p.node;
        if (p.pos == 0) return trie[p.node].par;

        const Node& v = trie[p.node];
        int mid = nextId++;
        trie.emplace_back(v.l, v.l+p.pos, v.par);
        trie[v.par].next[s[v.l]-'a'] = mid;

        trie[p.node].l += p.pos;
        trie[p.node].par = mid;
        trie[mid].next[s[v.l]-'a'] = p.node;

        return mid;
    }

    void build() {
        int m = s.size();
        Point activePoint(0, 0);

        for (int i = 0; i < m; i++) {
            int lastInserted = -1;

            Point nextPoint = go(activePoint, i, i+1);
            if (nextPoint.node != -1) {
                activePoint = nextPoint;
                if (lastInserted != -1)
                    trie[lastInserted].link = trie[nextPoint.node].par;
                break;
            }

            int oldPar = trie[activePoint.node].par;
            int mid = split(activePoint);
            bool isNewNode = mid != oldPar && mid != activePoint.node;
            int leaf = nextId++;
            trie.emplace_back(i, m, mid);
            if (lastInserted != -1)
                trie[lastInserted].link = mid;
            lastInserted = isNewNode ? mid : -1;

            activePoint = go(Point(trie[oldPar].par, trie[trie[oldPar].par].len()), i, i+1);
            if (activePoint.node == 0) break;
        }
    }

    string getLabel(int l, int r) {
        string res;
        for (int i = l; i < r; i++) {
            res += s[l] == SENTINEL ? '$' : s[l];
        }
        return res;
    }

    void print(int node, string prefix) {
        cout << node << '\n';

        int lastChild = -1;
        int longestEdge = 0;
        for (int i = 0; i < 27; i++) {
            int nextId = trie[node].next[i];
            if (nextId == 0) continue;
            lastChild = i;
            longestEdge = trie[nextId].len();
        }

        if (lastChild == -1)
            return;

        for (int i = 0; i < 27; i++) {
            if (trie[node].next[i] == 0) continue;
            int nextId = trie[node].next[i];

            string rightPadding = repeat("─", longestEdge - trie[nextId].len() + 1);
            cout << prefix << (i == lastChild ? "└─" : "├─") << getLabel(trie[nextId].l, trie[nextId].r) << rightPadding;

            string nextPrefix = prefix + (i == lastChild ? "│" : "") + string(longestEdge+2, ' ');
            print(trie[node].next[i], nextPrefix);
        }
    }

public:
    SuffixTree(const string& s) : s(s + SENTINEL) {
        trie.reserve(2 * s.size());
        trie.emplace_back(0, 0);
        build();
    }

    void print() {
        print(0, "");
    }
};

int main() {
    string s = "abcdef";
    SuffixTree st(s);
    st.print();
}
