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
 * Suffix Tree with O(m) construction time. Nodes are allocated using an arena allocator.
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

        Point active_point(0, 0, 0);
        int remainder = 0;

        for (int i = 0; i < m; i++) {
            remainder++;
            int last_inserted = -1;

            while (remainder > 0) {
                if (active_point.len == 0)
                    active_point.edge = i;

                int child_id = trie[active_point.node].next[s[active_point.edge]-'a'];

                // Check for rule 3
                if (child_id != 0 && s[trie[child_id].l + active_point.len] == s[i]) {
                    if (last_inserted != -1)
                        trie[last_inserted].link = active_point.node;
                    active_point.len++;
                    active_point = canonize(active_point);
                    break;
                }

                remainder--;

                int mid = split(active_point);
                if (last_inserted != -1)
                    trie[last_inserted].link = mid;
                last_inserted = trie[mid].link == -1 ? mid : -1;

                int leaf = nextId++;
                trie.emplace_back(i, m);
                trie[mid].next[s[i]-'a'] = leaf;

                if (active_point.node == 0) {
                    if (active_point.len > 0) {
                        active_point.edge++;
                        active_point.len--;
                    }
                } else {
                    active_point.node = trie[active_point.node].link;
                }
                active_point = canonize(active_point);
            }
        }
    }

    int split(Point p) {
        if (p.len == 0) return p.node;

        int child_id = trie[p.node].next[s[p.edge]-'a'];
        Node& v = trie[child_id];

        int mid = nextId++;
        trie.emplace_back(v.l, v.l+p.len);
        trie[p.node].next[s[p.edge]-'a'] = mid;

        trie[mid].next[s[v.l+p.len]-'a'] = child_id;
        trie[child_id].l += p.len;

        return mid;
    }

    Point canonize(Point p) {
        while (p.len > 0) {
            int child_id = trie[p.node].next[s[p.edge]-'a'];
            if (child_id == 0) break;

            int edge_len = trie[child_id].len();
            if (p.len < edge_len) break;

            p.node = child_id;
            p.edge += edge_len;
            p.len -= edge_len;
        }
        return p;
    }

    void print(int node, string prefix) {
        // │ ─ ├ └
        cout << node << '\n';

        int last_child = -1;
        int longest_len = 0;
        for (int i = 0; i < 27; i++) {
            int next_id = trie[node].next[i];
            if (next_id != 0) {
                last_child = i;
                longest_len = max(longest_len, trie[next_id].len());
            }
        }

        if (last_child == -1) return;

        for (int i = 0; i < 27; i++) {
            int next_id = trie[node].next[i];
            if (next_id == 0) continue;
            Node& next_node = trie[next_id];

            string right_padding = repeat("─", longest_len - next_node.len() + 1);
            cout << prefix << (i == last_child ? "└─" : "├─") << convert_label(next_node.l, next_node.r) << right_padding;
            string next_prefix = prefix + (i == last_child ? " " : "│") + string(longest_len + 2, ' ');

            print(next_id, next_prefix);
        }
    }

    string convert_label(int l, int r) {
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
            int next_id = trie[cur].next[word[i] - 'a'];
            if (next_id == 0)
                return false;
            Node& next_node = trie[next_id];

            int j = next_node.l;
            for (; j < next_node.r && i < word.size(); i++, j++) {
                if (s[j] != word[i])
                    return false;
            }

            cur = next_id;
        }

        return true;
    }
};

int main() {
    string s = "xabcxad";
    SuffixTree st(s);
}
