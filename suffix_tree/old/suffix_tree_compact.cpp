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
    int next_id = 1;

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
        int mid = next_id++;
        trie.emplace_back(v.l, v.l+p.pos, v.par);
        trie[v.par].next[s[v.l]-'a'] = mid;

        trie[p.node].l += p.pos;
        trie[p.node].par = mid;
        trie[mid].next[s[v.l]-'a'] = p.node;

        return mid;
    }

    void build() {
        int m = s.size();
        Point active_point(0, 0);

        for (int i = 0; i < m; i++) {
            int last_inserted = -1;

            Point next_point = go(active_point, i, i+1);
            if (next_point.node != -1) {
                active_point = next_point;
                if (last_inserted != -1)
                    trie[last_inserted].link = trie[next_point.node].par;
                break;
            }

            int old_par = trie[active_point.node].par;
            int mid = split(active_point);
            bool is_new_node = mid != old_par && mid != active_point.node;
            int leaf = next_id++;
            trie.emplace_back(i, m, mid);
            if (last_inserted != -1)
                trie[last_inserted].link = mid;
            last_inserted = is_new_node ? mid : -1;

            active_point = go(Point(trie[old_par].par, trie[trie[old_par].par].len()), i, i+1);
            if (active_point.node == 0) break;
        }
    }

    string get_label(int l, int r) {
        string res;
        for (int i = l; i < r; i++) {
            res += s[l] == SENTINEL ? '$' : s[l];
        }
        return res;
    }

    void print(int node, string prefix) {
        cout << node << '\n';

        int last_child = -1;
        int longest_edge = 0;
        for (int i = 0; i < 27; i++) {
            int next_id = trie[node].next[i];
            if (next_id == 0) continue;
            last_child = i;
            longest_edge = trie[next_id].len();
        }

        if (last_child == -1)
            return;

        for (int i = 0; i < 27; i++) {
            if (trie[node].next[i] == 0) continue;
            int next_id = trie[node].next[i];

            string right_padding = repeat("─", longest_edge - trie[next_id].len() + 1);
            cout << prefix << (i == last_child ? "└─" : "├─") << get_label(trie[next_id].l, trie[next_id].r) << right_padding;

            string next_prefix = prefix + (i == last_child ? "│" : "") + string(longest_edge+2, ' ');
            print(trie[node].next[i], next_prefix);
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
