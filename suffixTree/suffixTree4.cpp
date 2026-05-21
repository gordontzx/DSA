#include <bits/stdc++.h>
using namespace std;

string repeat(string str, int count) {
    string res;
    for (int i = 0; i < count; i++)
        res += str;
    return res;
}

/*
 * O(m) construction of a suffix tree, with on-demand suffix link construction.
 * Uses a parent pointer on each node.
 * Each node also stores the substring indices for the edge connecting its parent and itself.
 */
class SuffixTree {
private:
    struct TrieNode {
        int l, r;   // s[l..r) is the edge label on the edge from parent node
        int next[27];
        int par;
        int link;

        TrieNode(int l, int r, int par = -1, int link = -1)
            : l(l), r(r), par(par), link(link) {}

        int len() {
            return r-l;
        }
    };

    struct Point {
        int node;
        int pos;    // Number of characters matched on edge

        Point(int node, int pos) : node(node), pos(pos) {}
    };

    static constexpr char SENTINEL = 'z' + 1;

    vector<TrieNode> trie;
    int nextId = 1;
    int root = 0;
    string s;

    Point go(Point p, int l, int r) {
        // Return the point with path-label of (path-label of p + s[l..r))
        // -1 node if no such point
        while (l < r) {
            if (p.pos == trie[p.node].len()) {
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
        if (p.pos == 0)
            return trie[p.node].par;
        if (p.pos == trie[p.node].len())
            return p.node;

        int l = trie[p.node].l, r = trie[p.node].r;
        int mid = nextId++;
        trie.emplace_back(l, l+p.pos, trie[p.node].par);
        trie[trie[p.node].par].next[s[l]-'a'] = mid;
        trie[mid].next[s[l+p.pos]-'a'] = p.node;
        trie[p.node].par = mid;
        trie[p.node].l += p.pos;

        return mid;
    }

    int get_link(int node) {
        if (trie[node].link != -1) return trie[node].link;
        if (node == root) return 0;
        int par_link = get_link(trie[node].par);
        trie[node].link = split(go(Point(par_link, trie[par_link].len()), trie[node].l + (trie[node].par == root), trie[node].r));
        return trie[node].link;
    }

    void build() {
        int m = s.size();
        Point activePoint{0, 0};

        for (int i = 0; i < m; i++) {
            while (true) {
                // Check rule 3
                Point nextPoint = go(activePoint, i, i+1);
                if (nextPoint.node != -1) {
                    activePoint = nextPoint;
                    break;
                }

                int splitId = split(activePoint);
                int leaf = nextId++;
                trie.emplace_back(i, m, splitId);
                trie[splitId].next[s[i]-'a'] = leaf;

                activePoint.node = get_link(splitId);
                activePoint.pos = trie[activePoint.node].len();

                if (splitId == 0)
                    break;
            }
        }
    }

    string getLabel(int l, int r) {
        string res;
        for (int i = l; i <= r; i++) {
            res += (s[i] == SENTINEL ? '$' : s[i]);
        }
        return res;
    }

    void print(int node, string prefix) {
        cout << node << '\n';

        int lastChild = -1;
        int longestEdge = 0;
        for (int i = 0; i < 27; i++) {
            int nextNode = trie[node].next[i];
            if (nextNode != 0) {
                lastChild = i;
                longestEdge = max(longestEdge, trie[nextNode].r - trie[nextNode].l);
            }
        }

        if (lastChild == -1) return;

        for (int i = 0; i < 27; i++) {
            int childId = trie[node].next[i];
            if (childId == 0) continue;

            TrieNode& child = trie[childId];
            string rightPadding = repeat("─", longestEdge - (child.r - child.l) + 1);
            cout << prefix << (i == lastChild ? "└─" : "├─") << getLabel(child.l, child.r-1) << rightPadding;
            string nextPrefix = prefix + (i == lastChild ? " " : "│") + string(longestEdge+2, ' ');
            print(childId, nextPrefix);
        }
    }

public:
    SuffixTree(const string& s) : s(s + SENTINEL) {
        trie.reserve(2 * s.size());
        trie.emplace_back(0, 0, 0);
        build();
        print();
    }

    void print() {
        print(0, "");
    }
};

int main() {
    string s = "xabcxab";
    SuffixTree st(s);
}
