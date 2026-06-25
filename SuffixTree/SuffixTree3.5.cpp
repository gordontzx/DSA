#include <bits/stdc++.h>
using namespace std;

string repeat(string s, int count) {
    string res;
    for (int i = 0; i < count; i++) {
        res += s;
    }
    return res;
}

/*
 * Suffix Tree with O(m) construction time.
 */
class SuffixTree {
private:
    struct Node {
        int l, r;
        int next[27] = {};
        int link = -1;

        Node(int l, int r) : l(l), r(r) {}

        int len() {
            return r-l;
        }
    };

    struct Point {
        int node;
        int edge;
        int len;

        Point(int node, int edge, int len)
            : node(node), edge(edge), len(len) {}
    };

    static constexpr char SENTINEL = 'z' + 1;
    string s;
    vector<Node> trie;
    int nextId = 1;

    /* Constructs an implicit suffix tree of `s`. */
    void build() {
        int m = s.size();

        Point activePoint(0, 0, 0);
        int remainder = 0;

        for (int i = 0; i < m; i++) {
            remainder++;
            int lastInserted = -1;

            while (remainder > 0) {
                if (activePoint.len == 0)
                    activePoint.edge = i;

                int childId = trie[activePoint.node].next[s[activePoint.edge]-'a'];

                // Check for rule 3
                if (childId != 0 && s[trie[childId].l + activePoint.len] == s[i]) {
                    if (lastInserted != -1)
                        trie[lastInserted].link = activePoint.node;
                    activePoint.len++;
                    activePoint = canonize(activePoint);
                    break;
                }

                remainder--;

                int mid = split(activePoint);
                if (lastInserted != -1)
                    trie[lastInserted].link = mid;
                lastInserted = trie[mid].link == -1 ? mid : -1;

                int leaf = nextId++;
                trie.emplace_back(i, m);
                trie[mid].next[s[i]-'a'] = leaf;

                if (activePoint.node == 0) {
                    if (activePoint.len > 0) {
                        activePoint.edge++;
                        activePoint.len--;
                    }
                } else {
                    activePoint.node = trie[activePoint.node].link;
                }
                activePoint = canonize(activePoint);
            }
        }
    }

    int split(Point p) {
        if (p.len == 0) return p.node;

        int childId = trie[p.node].next[s[p.edge]-'a'];
        Node& v = trie[childId];

        int mid = nextId++;
        trie.emplace_back(v.l, v.l+p.len);
        trie[p.node].next[s[p.edge]-'a'] = mid;

        trie[mid].next[s[v.l+p.len]-'a'] = childId;
        trie[childId].l += p.len;

        return mid;
    }

    Point canonize(Point p) {
        while (p.len > 0) {
            int childId = trie[p.node].next[s[p.edge]-'a'];
            if (childId == 0) break;

            int edgeLen = trie[childId].len();
            if (p.len < edgeLen) break;

            p.node = childId;
            p.edge += edgeLen;
            p.len -= edgeLen;
        }
        return p;
    }

    void print(int node, string prefix) {
        // │ ─ ├ └
        cout << node << '\n';

        int lastChild = -1;
        int longestLen = 0;
        for (int i = 0; i < 27; i++) {
            int nextId = trie[node].next[i];
            if (nextId != 0) {
                lastChild = i;
                longestLen = max(longestLen, trie[nextId].len());
            }
        }

        if (lastChild == -1) return;

        for (int i = 0; i < 27; i++) {
            int nextId = trie[node].next[i];
            if (nextId == 0) continue;
            Node& nextNode = trie[nextId];

            string rightPadding = repeat("─", longestLen - nextNode.len() + 1);
            cout << prefix << (i == lastChild ? "└─" : "├─") << convertLabel(nextNode.l, nextNode.r) << rightPadding;
            string nextPrefix = prefix + (i == lastChild ? " " : "│") + string(longestLen + 2, ' ');

            print(nextId, nextPrefix);
        }
    }

    string convertLabel(int l, int r) {
        string res; res.reserve(r - l);
        for (int i = l; i < r; i++) {
            res += s[i] == SENTINEL ? '$' : s[i];
        }
        return res;
    }

public:
    SuffixTree(const string& s) : s(s + SENTINEL) {
        trie.reserve(2 * s.size());
        trie.emplace_back(0, 0);
        build();
        print();
    }

    void print() {
        print(0, "");
    }

    bool contains(const string& word) {
        int i = 0;
        int cur = 0;

        while (i < word.size()) {
            int nextId = trie[cur].next[word[i] - 'a'];
            if (nextId == 0)
                return false;
            Node& nextNode = trie[nextId];

            int j = nextNode.l;
            for (; j < nextNode.r && i < word.size(); i++, j++) {
                if (s[j] != word[i])
                    return false;
            }

            cur = nextId;
        }

        return true;
    }
};

int main() {
    string s = "xabcxad";
    SuffixTree st(s);
}
